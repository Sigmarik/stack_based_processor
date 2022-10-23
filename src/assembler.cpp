/**
 * @file main.c
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Program for translating human-readable lines of text into processor commands (assembler).
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
#include "procinfo.h"
#include "proccmd.h"
#include "lib/alloc_tracker/alloc_tracker.h"
#include "utils/common.h"
#include "utils/argworks.h"

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
 * @brief Binary buffer with its size.
 * 
 * @param content storage content of the p-file
 * @param size number of filled bytes
 */
struct PseudoFile {
    char* content = NULL;
    size_t size = 0;
} output_content;

/**
 * @brief JMP destination, marked in code with HERE word.
 * 
 * @param hash label name hash
 * @param point label jump dastination
 * @param name (unused) label name
 */
struct CodeLabel {
    hash_t hash = 0;
    uintptr_t point = 0;
    const char* name = NULL;
};

/**
 * @brief Set of code labels.
 * 
 * @param array list of labels
 * @param max_size max number of labels the set can store
 */
struct LabelSet {
    CodeLabel* array = NULL;
    size_t size = 0;
    size_t max_size = 1024;
};

void LabelSet_ctor(LabelSet* set);

void LabelSet_dtor(LabelSet* set);

/**
 * @brief Read all lines of text and write their interpretation to file.
 * 
 * @param labels set of labels to modify
 * @param output file to write the result to
 * @param listing listing file
 * @param lines array of lines of text
 * @param line_count number of lines in text
 * @param err_code variable to use as errno
 */
void assemble(LabelSet* labels, FILE* listing, char** lines, size_t line_count, int* err_code = NULL);

/**
 * @brief Read one line of text and put it into binary file.
 * 
 * @param labels set of labels to modify
 * @param line command to process
 * @param output output binary file
 * @param listing output listing file
 * @param err_code variable to use as errno
 */
void process_line(LabelSet* labels, const char* line, FILE* listing = NULL, int* const err_code = NULL);

/**
 * @brief Add label to label table.
 * 
 * @param labels set of labels to add the label to
 * @param hash label name hash
 * @param point label value
 * @param err_code variable to use as errno
 */
void add_label(LabelSet* labels, hash_t hash, uintptr_t point, int* const err_code = NULL);

/**
 * @brief Get the label object from label name hash.
 * 
 * @param labels set of labels to get the label from
 * @param hash label name hash
 * @param err_code variable to use as errno
 * @return uintptr_t 
 */
uintptr_t get_label(LabelSet* labels, hash_t hash, int* const err_code = NULL);

const size_t MAX_COMMAND_SIZE = 128;

const int NUMBER_OF_OWLS = 10;

#define DEFAULT_OUTPUT_NAME "a.bin"
#define DEFAULT_LISTING_NAME "listing.txt"

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    //* Ignore everything less or equaly important as status reports.
    static unsigned int log_threshold = STATUS_REPORTS + 1;
    static int gen_listing = 1;
    //* Maximum number of labels.
    static LabelSet labels = {};
    //* Output file size.
    static size_t out_size = 0xFFFF;
    static char listing_name[1024] = DEFAULT_LISTING_NAME;

    ActionTag line_tags[] = {
        #include "cmd_flags/assembler_flags.h"
    };
    const int number_of_tags = sizeof(line_tags) / sizeof(*line_tags);

    parse_args(argc, argv, number_of_tags, line_tags);
    log_init("program_log.log", log_threshold, &errno);
    print_label();

    LabelSet_ctor(&labels);
    _LOG_FAIL_CHECK_(labels.array, "error", ERROR_REPORTS, return EXIT_FAILURE, &errno, ENOMEM);

    track_allocation(&labels, (dtor_t*)LabelSet_dtor);

    output_content.content = (char*) calloc(out_size, sizeof(char));
    _LOG_FAIL_CHECK_(output_content.content, "error", ERROR_REPORTS, return EXIT_FAILURE, &errno, ENOMEM);

    track_allocation(&output_content, (dtor_t*)LabelSet_dtor);

    const char* file_name = get_input_file_name(argc, argv);
    _LOG_FAIL_CHECK_(file_name, "error", ERROR_REPORTS, {
        printf("Input file was not specified, terminating...\n");
        printf("To execute program stored in a file run\n%s [file name]\n", argv[0]);
        
        return_clean(EXIT_FAILURE);

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
        
        return_clean(EXIT_FAILURE);

    }, NULL, 0);
    setvbuf(input, NULL, _IOFBF, flength(fileno(input)));

    char* buffer = NULL;
    char** lines = NULL;
    size_t line_count = parse_lines(input, &lines, &buffer, &errno);

    log_printf(STATUS_REPORTS, "status", "LINES value is %p.\n", lines);
    track_allocation(&lines, (dtor_t*)free_var);
    track_allocation(&buffer, (dtor_t*)free_var);

    fclose(input); input = NULL;

    log_printf(STATUS_REPORTS, "status", "Opening output file %s.\n", out_name);
    FILE* output = fopen(out_name, "wb");
    _LOG_FAIL_CHECK_(output, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to create/open output file \"%s\", terminating...\n", out_name);

        return_clean(EXIT_FAILURE);

    }, NULL, 0);

    track_allocation(&output, (dtor_t*)fclose_var);

    FILE* listing = NULL;
    if (gen_listing) {
        log_printf(STATUS_REPORTS, "status", "Opening listing file .\n", listing_name);

        listing = fopen(listing_name, "w");

        _LOG_FAIL_CHECK_(listing, "warning", WARNINGS, {
            log_printf(WARNINGS, "warning", "Failed to create/open listing file \"%s\". "
                                            "No listing will be generated.\n", out_name);
        }, NULL, 0);

        if (listing) track_allocation(&listing, (dtor_t*)fclose_var);

    } else log_printf(STATUS_REPORTS, "status", "Listing files were disabled.\n");

    log_printf(STATUS_REPORTS, "status", "Entering first pass...\n");

    assemble(&labels, listing, lines, line_count, &errno);

    log_printf(STATUS_REPORTS, "status", "Entering final pass...\n");
    log_printf(STATUS_REPORTS, "status", "Resetting listing file...\n");
    fseek(listing, 0, SEEK_SET);

    output_content.size = 0;

    assemble(&labels, listing, lines, line_count, &errno);

    log_printf(STATUS_REPORTS, "status", "Writing header to the output file.\n");
    put_header(output);
    log_printf(STATUS_REPORTS, "status", "Writing content to the file.\n");
    fwrite(output_content.content, sizeof(char), output_content.size, output);

    return_clean(errno == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
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


void LabelSet_ctor(LabelSet* set) {
    set->array = (CodeLabel*)calloc(set->max_size, sizeof(*set->array));
    set->size = 0;
}

void LabelSet_dtor(LabelSet* set) {
    if (set->array) free(set->array);
    set->array = NULL;
    set->size = 0;
}

void assemble(LabelSet* labels, FILE* listing, char** lines, size_t line_count, int* err_code) {
    _LOG_FAIL_CHECK_(output_content.content, "error", ERROR_REPORTS, return, err_code, ENOENT);
    for (size_t line_id = 0; line_id < line_count; ++line_id) {
        log_printf(STATUS_REPORTS, "status", "Processing line %s.\n", lines[line_id]);
        process_line(labels, lines[line_id], listing, &errno);
    }
}

#define DEF_CMD(name, parse_script, exec_script, disasm_script) \
    if (hash == CMD_HASHES[CMD_##name]) {sequence[0] = (CMD_##name << 2); ++cmd_size; parse_script;} else

#define ARG_PTR                 ( line + shift )
#define GET_LABEL(arg)          get_label(arg, err_code)
#define CUR_ID                  ( output_content.size + HEADER_SIZE )
#define BUF_PTR                 sequence
#define BUF_WRITE(ptr, length)  { memcpy(sequence + cmd_size, ptr, length); cmd_size += length; }
#define ERRNO                   err_code
#define LABEL_LIST              labels

#define if_cmd_not_defined

void process_line(LabelSet* labels, const char* line, FILE* listing, int* const err_code) {
    _LOG_FAIL_CHECK_(line, "error", ERROR_REPORTS, return, NULL, 0);

    char first_char = '\0';
    sscanf(line, " %c", &first_char);

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
            add_label(labels, lbl_hash, output_content.size + HEADER_SIZE, err_code);
            
            log_printf(STATUS_REPORTS, "status", "Label %s was set to %0*X.\n", lbl_name, sizeof(uintptr_t), output_content.size);
            
            break;
        }

        #include "cmddef.h"

        if_cmd_not_defined log_printf(ERROR_REPORTS, "error", "Unknown command %s.\n", code);
    } while (0);

    log_printf(STATUS_REPORTS, "status", "Writing command to the file, cmd size -> %ld.\n", cmd_size);
    memcpy(output_content.content + output_content.size, sequence, cmd_size);
    
    output_content.size += cmd_size;
    if (listing) {
        fprintf(listing, "| %-36.36s  | [0x%0*lX] ", line, (int)sizeof(uintptr_t), output_content.size - cmd_size + HEADER_SIZE);
        for (int id = 0; id < (int)cmd_size; ++id) {
            fprintf(listing, " %02X", (unsigned int)sequence[id] & 0xFF);
        }
        fputc('\n', listing);
    }
}

#undef DEF_CMD

void add_label(LabelSet* labels, hash_t hash, uintptr_t point, int* const err_code) {
    _LOG_FAIL_CHECK_(labels->array, "error", ERROR_REPORTS, return, err_code, ENOENT);
    _LOG_FAIL_CHECK_(labels->size < labels->max_size, "error", ERROR_REPORTS, return, err_code, ENOMEM);

    for (size_t label_id = 0; label_id < labels->size; ++label_id) {
        if (hash == labels->array[label_id].hash) {
            labels->array[label_id].point = point;
            return;
        }
    }

    labels->array[labels->size].hash  = hash;
    labels->array[labels->size].point = point;
    ++labels->size;
}

uintptr_t get_label(LabelSet* labels, hash_t hash, int* const err_code) {
    _LOG_FAIL_CHECK_(labels->array, "error", ERROR_REPORTS, return 0, err_code, ENOENT);

    for (size_t label_id = 0; label_id < labels->size; ++label_id) {
        if (hash == labels->array[label_id].hash) return labels->array[label_id].point;
    }

    int status = 0;
    add_label(labels, hash, 0, &status);
    return status == 0 ? labels->array[labels->size - 1].point : 0;
}
