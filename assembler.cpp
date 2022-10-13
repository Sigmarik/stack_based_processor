/**
 * @file main.c
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Program for reading text file and sorting its lines.
 * @version 0.1
 * @date 2022-08-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <clocale>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "lib/util/dbg/debug.h"
#include "lib/util/argparser.h"
#include "lib/file_proc.h"
#include "lib/procinfo.h"
#include "lib/proccmd.h"

#pragma GCC diagnostic ignored "-Wcast-align"

#define ASSEMBLER

/**
 * @brief Print a bunch of owls.
 * 
 * @param argc unimportant
 * @param argv unimportant
 * @param argument unimportant
 */
void print_owl(const int argc, void** argv, const char* argument);

/**
 * @brief Print program label and build date/time to console and log.
 * 
 */
void print_label();

/**
 * @brief Get the input file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_input_file_name(const int argc, const char** argv);

/**
 * @brief Get the output file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_output_file_name(const int argc, const char** argv);

/**
 * @brief Print binary header to the file.
 * 
 * @param output 
 */
void put_header(FILE* output);

/**
 * @brief Read one line of text and put it into binary file.
 * 
 * @param line command to process
 * @param output output binary file
 * @param listing output listing file
 * @param err_code variable to use as errno
 */
void process_line(const char* line, FILE* output, FILE* listing = NULL, int* const err_code = NULL);

static size_t label_max_count = 1024;
static hash_t* label_hashes = NULL;
static uintptr_t* label_values = NULL;

/**
 * @brief Add label to label table.
 * 
 * @param hash label name hash
 * @param point label value
 * @param err_code variable to use as errno
 */
void add_label(hash_t hash, uintptr_t point, int* const err_code = NULL);

/**
 * @brief Get the label object from label name hash.
 * 
 * @param hash label name hash
 * @param err_code variable to use as errno
 * @return uintptr_t 
 */
uintptr_t get_label(hash_t hash, int* const err_code = NULL);

// Ignore everything less or equaly important as status reports.
static unsigned int log_threshold = STATUS_REPORTS + 1;
static int gen_listing = 1;
static char listing_name[1024] = "listing.txt";

static const size_t MAX_COMMAND_SIZE = 128;

static const int NUMBER_OF_OWLS = 10;

static const struct ActionTag LINE_TAGS[] = {
    {
        .name = {'O', "owl"}, 
        .action = {
            .parameters = {}, 
            .parameters_length = 0, 
            .function = print_owl, 
        },
        .description = "print 10 owls to the screen."
    },
    {
        .name = {'I', ""}, 
        .action = {
            .parameters = (void*[]) {&log_threshold},
            .parameters_length = 1, 
            .function = edit_int,
        },
        .description = "set log threshold to the specified number.\n"
                        "\tDoes not check if integer was specified."
    },
    {
        .name = {'L', ""}, 
        .action = {
            .parameters = (void*[]) {&gen_listing},
            .parameters_length = 1, 
            .function = edit_int,
        },
        .description = "set if program should generate listing or not.\n"
                        "\tDoes not check if integer was specified."
    },
    {
        .name = {'S', ""}, 
        .action = {
            .parameters = (void*[]) {&label_max_count},
            .parameters_length = 1, 
            .function = edit_int,
        },
        .description = "set maximum number of labels.\n"
                        "\tDoes not check if integer was specified."
    },
};
static const int NUMBER_OF_TAGS = sizeof(LINE_TAGS) / sizeof(*LINE_TAGS);

const char* DEFAULT_OUTPUT_NAME = "a.instr";

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    parse_args(argc, argv, NUMBER_OF_TAGS, LINE_TAGS);
    log_init("program_log.log", log_threshold, &errno);
    print_label();

    label_hashes = (hash_t*)   calloc(label_max_count, sizeof(*label_hashes));
    label_values = (uintptr_t*)calloc(label_max_count, sizeof(*label_values));
    _LOG_FAIL_CHECK_(label_hashes, "error", ERROR_REPORTS, return EXIT_FAILURE, &errno, ENOMEM);
    _LOG_FAIL_CHECK_(label_values, "error", ERROR_REPORTS, return EXIT_FAILURE, &errno, ENOMEM);

    const char* file_name = get_input_file_name(argc, argv);
    _LOG_FAIL_CHECK_(file_name, "error", ERROR_REPORTS, {
        printf("Input file was not specified, terminating...\n");
        printf("To execute program stored in a file run\n%s [file name]\n", argv[0]);
        free(label_hashes);
        free(label_values);
        return EXIT_FAILURE;
    }, NULL, 0);

    const char* out_name = get_output_file_name(argc, argv);
    if (out_name == NULL) {
        out_name = DEFAULT_OUTPUT_NAME;
    }

    log_printf(STATUS_REPORTS, "status", "Opening input file %s.\n", file_name);
    //* We need bufferization for windows capability (sigh...)
    FILE* input = fopen(file_name, "r");
    _LOG_FAIL_CHECK_(input, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to open input file \"%s\", terminating...\n", file_name);
        free(label_hashes);
        free(label_values);
        return EXIT_FAILURE;
    }, NULL, 0);
    setvbuf(input, NULL, _IOFBF, flength(fileno(input)));
    char* buffer = NULL;
    char** lines = NULL;
    size_t line_count = parse_lines(input, &lines, &buffer, &errno);
    fclose(input);

    log_printf(STATUS_REPORTS, "status", "Opening output file %s.\n", out_name);
    FILE* output = fopen(out_name, "wb");
    _LOG_FAIL_CHECK_(output, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to create/open output file \"%s\", terminating...\n", out_name);
        free(label_hashes);
        free(label_values);
        return EXIT_FAILURE;
    }, NULL, 0);

    FILE* listing = NULL;
    if (gen_listing) {
        log_printf(STATUS_REPORTS, "status", "Opening listing file .\n", listing_name);
        listing = fopen(listing_name, "w");
        _LOG_FAIL_CHECK_(output, "warning", WARNINGS, {
            log_printf(WARNINGS, "warning", "Failed to create/open listing file \"%s\". "
                                            "No listing will be generated.\n", out_name);
        }, NULL, 0);
    } else {
        log_printf(STATUS_REPORTS, "status", "Listing files were disabled.\n");
    }

    log_printf(STATUS_REPORTS, "status", "Writing header to the output file.\n");
    put_header(output);

    log_printf(STATUS_REPORTS, "status", "Entering first search...\n");

    for (size_t line_id = 0; line_id < line_count; ++line_id) {
        log_printf(STATUS_REPORTS, "status", "Processing line %s.\n", lines[line_id]);
        process_line(lines[line_id], output, listing, &errno);
    }

    log_printf(STATUS_REPORTS, "status", "Entering final loop...\n");
    log_printf(STATUS_REPORTS, "status", "Reseting output file...\n");
    fseek(output, 0, SEEK_SET);
    put_header(output);
    log_printf(STATUS_REPORTS, "status", "Reseting listing file...\n");
    fseek(listing, 0, SEEK_SET);

    for (size_t line_id = 0; line_id < line_count; ++line_id) {
        log_printf(STATUS_REPORTS, "status", "Processing line %s.\n", lines[line_id]);
        process_line(lines[line_id], output, listing, &errno);
    }

    free(buffer);
    free(lines);
    free(label_hashes);
    free(label_values);
    fclose(output);

    return EXIT_SUCCESS;
}

// Офигенно, ничего не менять.
// Дополнил сову, сорри.
void print_owl(const int argc, void** argv, const char* argument) {
    UNUSE(argc); UNUSE(argv); UNUSE(argument);
    printf("-Owl argument detected, dropping emergency supply of owls.\n");
    for (int index = 0; index < NUMBER_OF_OWLS; index++) {
        puts(R"(    A_,,,_A    )");
        puts(R"(   ((O)V(O))   )");
        puts(R"(  ("\"|"|"/")  )");
        puts(R"(   \"|"|"|"/   )");
        puts(R"(     "| |"     )");
        puts(R"(      ^ ^      )");
    }
}

void print_label() {
    printf("Assembler for a stack-based processor by Ilya Kudryashov.\n");
    printf("Program assembles human-readable code into binary program.\n");
    printf("Build from\n%s %s\n", __DATE__, __TIME__);
    log_printf(ABSOLUTE_IMPORTANCE, "build info", "Build from %s %s.\n", __DATE__, __TIME__);
}

const char* get_input_file_name(const int argc, const char** argv) {
    const char* file_name = NULL;

    for (int argument_id = 1; argument_id < argc; ++argument_id) {
        if (*argv[argument_id] == '-') continue;
        file_name = argv[argument_id];
        break;
    }

    return file_name;
}

const char* get_output_file_name(const int argc, const char** argv) {
    const char* file_name = NULL;

    bool enc_first_name = false;
    for (int argument_id = 1; argument_id < argc; ++argument_id) {
        if (*argv[argument_id] == '-') continue;
        file_name = argv[argument_id];
        if (enc_first_name) return file_name;
        else enc_first_name = true;
    }

    return NULL;
}

void put_header(FILE* output) {
    _LOG_FAIL_CHECK_(output, "error", ERROR_REPORTS, return, NULL, 0);
    fwrite(FILE_PREFIX, PREFIX_SIZE, 1, output);
    fwrite(&PROC_VERSION, sizeof(PROC_VERSION), 1, output);
    fseek(output, HEADER_SIZE, SEEK_SET);
}

#define DEF_CMD(name, parse_script, exec_script) \
if (hash == CMD_HASHES[CMD_##name]) {sequence[0] = CMD_##name; ++cmd_size; parse_script;} else

#define ARG_PTR (line + shift)
#define GET_LABEL(arg) get_label(arg, err_code)
#define CUR_ID ftell(output)
#define BUF_WRITE(ptr, length) {memcpy(sequence + cmd_size, ptr, length); cmd_size += length;}
#define ERRNO err_code

#define if_cmd_not_defined

void process_line(const char* line, FILE* output, FILE* listing, int* const err_code) {
    _LOG_FAIL_CHECK_(line,   "error", ERROR_REPORTS, return, NULL, 0);
    _LOG_FAIL_CHECK_(output, "error", ERROR_REPORTS, return, NULL, 0);

    char first_char = '\0';
    sscanf(line, "%c", &first_char);

    int shift = 0;
    char code[10] = "";
    char sequence[MAX_COMMAND_SIZE] = "";
    size_t cmd_size = 0;
    hash_t hash = 0;
    
    if (*line != '\0' && first_char != CMD_COMMENT_CHAR) do {

        sscanf(line, "%s%n", code, &shift);
        hash = get_hash(code, code + strlen(code));

        if (hash == CMD_LABEL_HASH) {
            char lbl_name[LABEL_MAX_NAME_LENGTH] = "";
            sscanf(line + shift, "%s", lbl_name);
            hash_t lbl_hash = get_hash(lbl_name, lbl_name + strlen(lbl_name));
            add_label(lbl_hash, (uintptr_t)ftell(output), err_code);
            log_printf(STATUS_REPORTS, "status", "Label %s was set to %0*X.\n", lbl_name, sizeof(uintptr_t), ftell(output));
            break;
        }

        #include "lib/cmddef.h"
        if_cmd_not_defined log_printf(ERROR_REPORTS, "error", "Unknown command %s.\n", code);
    } while (0);

    log_printf(STATUS_REPORTS, "status", "Writing command to the file, cmd size -> %ld.\n", cmd_size);
    fwrite(sequence, cmd_size, 1, output);
    if (listing) {
        fprintf(listing, "| %-36.36s  | [0x%0*lX] ", line, (int)sizeof(uintptr_t), (size_t)ftell(output) - cmd_size);
        for (int id = 0; id < (int)cmd_size; ++id) {
            fprintf(listing, " %02X", (unsigned int)sequence[id] & 0xFF);
        }
        fputc('\n', listing);
    }
}

#undef DEF_CMD

static size_t label_count = 0;

void add_label(hash_t hash, uintptr_t point, int* const err_code) {
    _LOG_FAIL_CHECK_(label_hashes, "error", ERROR_REPORTS, return, err_code, ENOENT);
    _LOG_FAIL_CHECK_(label_values, "error", ERROR_REPORTS, return, err_code, ENOENT);
    _LOG_FAIL_CHECK_(label_count < label_max_count, "error", ERROR_REPORTS, return, err_code, ENOMEM);

    for (size_t label_id = 0; label_id < label_count; ++label_id) {
        if (hash == label_hashes[label_id]) {
            label_values[label_id] = point;
            return;
        }
    }

    label_hashes[label_count] = hash;
    label_values[label_count] = point;
    ++label_count;
}

uintptr_t get_label(hash_t hash, int* const err_code) {
    _LOG_FAIL_CHECK_(label_hashes, "error", ERROR_REPORTS, return 0, err_code, ENOENT);
    _LOG_FAIL_CHECK_(label_values, "error", ERROR_REPORTS, return 0, err_code, ENOENT);

    for (size_t label_id = 0; label_id < label_count; ++label_id) {
        if (hash == label_hashes[label_id]) return label_values[label_id];
    }

    int status = 0;
    add_label(hash, 0, &status);
    return status == 0 ? label_values[label_count - 1] : 0;
}