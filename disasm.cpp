/**
 * @file main.c
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Program for disassembling binary processor instruction files.
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

#include "lib/util/dbg/debug.h"
#include "lib/util/argparser.h"
#include "lib/file_proc.h"
#include "lib/procinfo.h"
#include "lib/proccmd.h"

//* warning: stack protector not protecting function: all local arrays are less than 8 bytes long [-Wstack-protector]
#pragma GCC diagnostic ignored "-Wstack-protector"

#define DISASSEMBLER

typedef long long stack_content_t;
stack_content_t STACK_CONTENT_POISON = (stack_content_t) 0xDEADBABEC0FEBEEF;
#include "lib/stackworks.h"

static const size_t STACK_START_SIZE = 1024;
static const size_t ADDR_STACK_START_SIZE = 16;

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
 * @brief Read header and check if file matches requirements.
 * 
 * @param ptr pointer to the start of the file's content
 * @param output file to write information about the program to
 * @param err_code variable to use as errno
 * @return size_t required pointer shift
 */
size_t read_header(char* ptr, FILE* output, int* err_code = NULL);

/**
 * @brief Disassemble one command and return pointer shift.
 * 
 * @param ptr pointer to the pointer to the command
 * @param stack stack to operate on
 * @param addr_stack address stack
 * @param err_code error code
 * @return size_t
 */
int execute_command(const char* prog_start, const char* ptr, FILE* file, int* const err_code = NULL);

/**
 * @brief Get the file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_file_name(const int argc, const char** argv);

/**
 * @brief Get the output file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_output_file_name(const int argc, const char** argv);

// Ignore everything less or equaly important as status reports.
static unsigned int log_threshold = STATUS_REPORTS + 1;
static const int NUMBER_OF_OWLS = 10;

static const struct ActionTag LINE_TAGS[] = {
    {
        .name = {'O', "owl"}, 
        .action = {
            .parameters = {}, 
            .parameters_length = 0, 
            .function = print_owl, 
        },
        .description = "print 10 owls on the screen."
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
};

static const int NUMBER_OF_TAGS = sizeof(LINE_TAGS) / sizeof(*LINE_TAGS);

const char* DEFAULT_OUTPUT_NAME = "program.txt";

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    parse_args(argc, argv, NUMBER_OF_TAGS, LINE_TAGS);
    log_init("program_log.log", log_threshold, &errno);
    print_label();

    const char* file_name = get_file_name(argc, argv);
    _LOG_FAIL_CHECK_(file_name, "error", ERROR_REPORTS, {
        printf("File was not specified, terminating...\n");
        printf("To disassemble the program stored in a file run\n%s [file name]\n", argv[0]);
        return EXIT_FAILURE;
    }, NULL, 0);

    const char* out_name = get_output_file_name(argc, argv);
    if (out_name == NULL) {
        out_name = DEFAULT_OUTPUT_NAME;
    }

    log_printf(STATUS_REPORTS, "status", "Opening output file %s.\n", out_name);
    FILE* output = fopen(out_name, "w");
    _LOG_FAIL_CHECK_(output, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to create/open output file \"%s\", terminating...\n", out_name);
        return EXIT_FAILURE;
    }, NULL, 0);

    log_printf(STATUS_REPORTS, "status", "Opening file %s.\n", file_name);
    int fd = open(file_name, O_RDONLY);
    _LOG_FAIL_CHECK_(fd != -1, "error", ERROR_REPORTS, {
        printf("File was not opened, terminating...\n");
        return EXIT_FAILURE;
    }, NULL, 0);
    size_t size = flength(fd);

    log_printf(STATUS_REPORTS, "status", "Reading file content...\n");
    char* content = (char*) calloc(size, sizeof(*content));
    read(fd, content, size);
    close(fd);

    char* pointer = content;

    log_printf(STATUS_REPORTS, "status", "Reading file header...\n");
    size_t prefix_shift = read_header(pointer, output, &errno);
    pointer += prefix_shift;
    _LOG_FAIL_CHECK_(prefix_shift, "error", ERROR_REPORTS, return EXIT_FAILURE, NULL, 0);

    log_printf(STATUS_REPORTS, "status", "Starting disassembling commands...\n");
    int delta = 0;
    while ((delta = execute_command(content, pointer, output, &errno)) != 0) {
        pointer += delta;
        if (!(pointer > content && pointer < content + size)) break;
    }

    log_printf(STATUS_REPORTS, "status", "Disassembly finished, cleaning allocated memory...\n");
    free(content);

    if (errno) return EXIT_FAILURE;
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
    printf("Virtual processor instruction disassembler by Ilya Kudryashov.\n");
    printf("Program disassembles processor instructions into pseudo-source code.\n");
    printf("Build from\n%s %s\n", __DATE__, __TIME__);
    log_printf(ABSOLUTE_IMPORTANCE, "build info", "Build from %s %s.\n", __DATE__, __TIME__);
}

size_t read_header(char* ptr, FILE* output, int* err_code) {
    _LOG_FAIL_CHECK_(ptr, "error", ERROR_REPORTS, return 0, err_code, EFAULT);
    _LOG_FAIL_CHECK_(strncmp(FILE_PREFIX, ptr, PREFIX_SIZE) == 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Wrong file prefix, terminating. Prefix - \"%*s\", expected prefix - \"%*s\"\n",
                                            PREFIX_SIZE, ptr, PREFIX_SIZE, FILE_PREFIX);
        return 0;
    }, err_code, EIO);
    _LOG_FAIL_CHECK_(*(version_t*)(ptr+4) <= PROC_VERSION, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Wrong file version, terminating. File version - %d, disassembler version - %d.\n",
                                                                            *(version_t*)(ptr+4), PROC_VERSION);
        return 0;
    }, err_code, EIO);
    fprintf(output, "# Binary file version: %d\n", *(version_t*)(ptr+4));
    fprintf(output, "# Disassembler version: %d\n\n", PROC_VERSION);
    return HEADER_SIZE;
}

#define _LOG_EMPT_STACK_(command) do {                                                      \
    _LOG_FAIL_CHECK_(stack->size, "error", ERROR_REPORTS, {                                 \
        log_printf(ERROR_REPORTS, "error", "Request to the empty stack in " command ".\n"); \
        shift = 0;                                                                          \
        break;                                                                              \
    }, err_code, EFAULT);                                                                   \
} while (0)

#define DEF_CMD(name, parse_script, exec_script, disasm_script) \
case CMD_##name: {fprintf(file, #name " "); disasm_script; putc('\n', file);} break;

#define SHIFT shift
#define EXEC_POINT ptr
#define ARG_PTR ptr + 1
#define ERRNO err_code
#define OUT_FILE file

int execute_command(const char* prog_start, const char* ptr, FILE* file, int* const err_code) {
    _LOG_FAIL_CHECK_(ptr, "error", ERROR_REPORTS, return 0, err_code, EFAULT);

    log_printf(STATUS_REPORTS, "status", "Executing command %02X (mask %d) at 0x%0*X.\n", 
               (*ptr >> 2) & 0xFF, *ptr & 3, sizeof(prog_start), ptr - prog_start);
    
    _LOG_FAIL_CHECK_(file, "error", ERROR_REPORTS, return 0, err_code, EFAULT);

    int shift = 1;

    switch ((*ptr) >> 2) {
        #include "lib/cmddef.h"

        default:
            log_printf(ERROR_REPORTS, "error", "Unknown command [%0X]. Terminating.\n", (*ptr) >> 2);
            if (err_code) *err_code = EIO;
            shift = 0;
        break;
    }

    return shift;
}

#undef DEF_CMD

const char* get_file_name(const int argc, const char** argv) {
    const char* file_name = NULL;

    for (int argument_id = 1; argument_id < argc; ++argument_id) {
        if (*argv[argument_id] == '-') continue;
        file_name = argv[argument_id];
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