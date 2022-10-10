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

#include "lib/util/dbg/debug.h"
#include "lib/util/argparser.h"
#include "lib/file_proc.h"
#include "lib/procinfo.h"
#include "lib/proccmd.h"

typedef long long stack_content_t;
stack_content_t STACK_CONTENT_POISON = 0xDEADBABEC0FEBEEF;
#include "lib/stackworks.h"

static const size_t STACK_START_SIZE = 1024;

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
 * @param err_code variable to use as errno
 * @return size_t required pointer shift
 */
size_t read_header(char* ptr, int* err_code = NULL);

/**
 * @brief Execute one command and return its length.
 * 
 * @param ptr pointer to the pointer to the command
 * @param stack stack to operate on
 * @param err_code error code
 * @return size_t
 */
size_t execute_command(const char* ptr, Stack* const stack, int* const err_code = NULL);

/**
 * @brief Get the file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_file_name(const int argc, const char** argv);

// Ignore everything less or equaly important as status reports.
static int log_threshold = STATUS_REPORTS + 1;
static const int NUMBER_OF_OWLS = 10;
static size_t reg_size = 8;

static int* reg = NULL;

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
    {
        .name = {'R', ""}, 
        .action = {
            .parameters = (void*[]) {&reg_size},
            .parameters_length = 1, 
            .function = edit_int,
        },
        .description = "set max. number of cells register can hold.\n"
                        "\tDoes not check if integer was specified."
    },
};
static const int NUMBER_OF_TAGS = sizeof(LINE_TAGS) / sizeof(*LINE_TAGS);

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    parse_args(argc, argv, NUMBER_OF_TAGS, LINE_TAGS);
    log_init("program_log.log", log_threshold, &errno);
    print_label();

    reg = (int*) calloc(reg_size, sizeof(*reg));

    const char* file_name = get_file_name(argc, argv);
    _LOG_FAIL_CHECK_(file_name, "error", ERROR_REPORTS, {
        printf("File was not specified, terminating...\n");
        printf("To execute program stored in a file run\n%s [file name]\n", argv[0]);
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

    log_printf(STATUS_REPORTS, "status", "Initializing stack...\n");
    Stack stack = {};
    stack_init(&stack, STACK_START_SIZE, &errno);
    _LOG_FAIL_CHECK_(!stack_status(&stack), "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to initialize stack.\n");
        stack_dump(&stack, ERROR_REPORTS);
    }, NULL, 0);
    
    log_printf(STATUS_REPORTS, "status", "Reading file header...\n");
    int prefix_shift = read_header(pointer, &errno);
    pointer += prefix_shift;
    _LOG_FAIL_CHECK_(prefix_shift, "error", ERROR_REPORTS, { free(content); return EXIT_FAILURE; }, NULL, 0);

    log_printf(STATUS_REPORTS, "status", "Starting executing commands...\n");
    size_t delta = 0;
    while ((delta = execute_command(pointer, &stack, &errno)) != 0) {
        char* prev_ptr = pointer;
        pointer += delta;
        _LOG_FAIL_CHECK_(pointer > content, "error", ERROR_REPORTS, {
            log_printf(ERROR_REPORTS, "error", "Negative pointer value of 0x%0*X after executing command at 0x%0*X. Terminating.\n", 
                                                sizeof(uintptr_t), pointer - content, sizeof(uintptr_t), prev_ptr - content);
            return EXIT_FAILURE;
        }, &errno, EFAULT);
    }

    log_printf(STATUS_REPORTS, "status", "Execution finished, cleaning allocated memory...\n");
    free(content);
    free(reg);
    stack_destroy(&stack, &errno);

    if (errno) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

// Офигенно, ничего не менять.
// Дополнил сову, сорри.
void print_owl(const int argc, void** argv, const char* argument) {
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
    printf("Stack-based processor by Ilya Kudryashov.\n");
    printf("Program implements stack-based virtual command processor.\n");
    printf("Build from\n%s %s\n", __DATE__, __TIME__);
    log_printf(ABSOLUTE_IMPORTANCE, "build info", "Build from %s %s.\n", __DATE__, __TIME__);
}

size_t read_header(char* ptr, int* err_code) {
    _LOG_FAIL_CHECK_(ptr, "error", ERROR_REPORTS, return 0, err_code, EFAULT);
    _LOG_FAIL_CHECK_(strncmp(FILE_PREFIX, ptr, PREFIX_SIZE) == 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Wrong file prefix, terminating. Prefix - \"%*s\", expected prefix - \"%*s\"\n",
                                            PREFIX_SIZE, ptr, PREFIX_SIZE, FILE_PREFIX);
        return 0;
    }, err_code, EIO);
    _LOG_FAIL_CHECK_(*(version_t*)(ptr+4) <= PROC_VERSION, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Wrong file version, terminating. File version - %d, processor version - %d.\n",
                                                                            *(version_t*)(ptr+4), PROC_VERSION);
        return 0;
    }, err_code, EIO);
    return HEADER_SIZE;
}

#define _LOG_EMPT_STACK_(command) do {                                                      \
    _LOG_FAIL_CHECK_(stack->size, "error", ERROR_REPORTS, {                                 \
        log_printf(ERROR_REPORTS, "error", "Request to the empty stack in " command ".\n"); \
        shift = 0;                                                                          \
        break;                                                                              \
    }, err_code, EFAULT);                                                                   \
} while (0)

size_t execute_command(const char* ptr, Stack* const stack, int* const err_code) {
    _LOG_FAIL_CHECK_(ptr, "error", ERROR_REPORTS, return 0, err_code, EFAULT);
    log_printf(STATUS_REPORTS, "status", "Executing command 0x%02X.\n", *ptr & 0xFF);
    _LOG_FAIL_CHECK_(stack_status(stack) == 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Stack status check failed.\n");
        stack_dump(stack, ERROR_REPORTS);
        return 0;
    }, NULL, 0);
    int var_a = 0, var_b = 0;
    int shift = 1;
    switch (*ptr) {
        case CMD_END: 
            shift = 0;
        break;
        case CMD_ABORT:
            shift = 0;
            if (err_code) *err_code = EAGAIN;
        break;
        case CMD_PUSH:
            shift = sizeof(int) + 1;
            stack_push(stack, *(int*)(ptr + 1), err_code);
        break;
        case CMD_POP:
            _LOG_EMPT_STACK_("POP");
            stack_pop(stack, err_code);
        break;
        case CMD_OUT:
            _LOG_EMPT_STACK_("OUT");
            printf("%lld", stack_get(stack));
        break;
        case CMD_ADD:
            _LOG_EMPT_STACK_("ADD[top]");
            var_b = stack_get(stack, err_code); stack_pop(stack, err_code);
            _LOG_EMPT_STACK_("ADD[bottom]");
            var_a = stack_get(stack, err_code); stack_pop(stack, err_code);
            stack_push(stack, var_a + var_b, err_code);
        break;
        case CMD_SUB:
            _LOG_EMPT_STACK_("SUB[top]");
            var_b = stack_get(stack, err_code); stack_pop(stack, err_code);
            _LOG_EMPT_STACK_("SUB[bottom]");
            var_a = stack_get(stack, err_code); stack_pop(stack, err_code);
            stack_push(stack, var_a - var_b, err_code);
        break;
        case CMD_MUL:
            _LOG_EMPT_STACK_("MUL[top]");
            var_b = stack_get(stack, err_code); stack_pop(stack, err_code);
            _LOG_EMPT_STACK_("MUL[bottom]");
            var_a = stack_get(stack, err_code); stack_pop(stack, err_code);
            stack_push(stack, var_a * var_b, err_code);
        break;
        case CMD_DIV:
            _LOG_EMPT_STACK_("DIV[top]");
            var_b = stack_get(stack, err_code); stack_pop(stack, err_code);
            _LOG_EMPT_STACK_("DIV[bottom]");
            var_a = stack_get(stack, err_code); stack_pop(stack, err_code);
            stack_push(stack, var_a / var_b, err_code);
        break;
        case CMD_DUP:
            _LOG_EMPT_STACK_("DUP");
            var_a = stack_get(stack, err_code);
            stack_push(stack, var_a, err_code);
        break;
        case CMD_JMP:
            shift = *(int*)(ptr + 1);
            _LOG_FAIL_CHECK_(shift != 0, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "JMP argument was 0, terminating.\n");
            }, err_code, EFAULT);
        break;
        case CMD_JMPG:
            _LOG_EMPT_STACK_("JMPG[top]");
            var_b = stack_get(stack, err_code); stack_pop(stack, err_code);
            _LOG_EMPT_STACK_("JMPG[bottom]");
            var_a = stack_get(stack, err_code); stack_pop(stack, err_code);
            shift = sizeof(int) + 1;
            _LOG_FAIL_CHECK_(shift != 0, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "JMPG argument was 0, terminating.\n");
            }, err_code, EFAULT);
            if (var_b > var_a) shift = *(int*)(ptr + 1);
        break;
        case CMD_RGET:
            shift = sizeof(int) + 1;
            _LOG_FAIL_CHECK_(0 <= *(int*)(ptr + 1) && *(int*)(ptr + 1) <= (int)reg_size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RGET, terminating.\n", *(int*)(ptr + 1));
                shift = 0;
            }, err_code, EFAULT);
            stack_push(stack, reg[*(int*)(ptr + 1)], err_code);
        break;
        case CMD_RSET:
            shift = sizeof(int) + 1;
            _LOG_EMPT_STACK_("RSET");
            _LOG_FAIL_CHECK_(0 <= *(int*)(ptr + 1) && *(int*)(ptr + 1) <= (int)reg_size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Invarid register index of %d in RSET, terminating.\n", *(int*)(ptr + 1));
                shift = 0;
            }, err_code, EFAULT);
            reg[*(int*)(ptr + 1)] = stack_get(stack, err_code);
        break;
        case CMD_OUTC:
            _LOG_EMPT_STACK_("OUTC");
            putc((char)stack_get(stack), stdout);
        break;
        default:
            log_printf(ERROR_REPORTS, "error", "Unknown command [%0X]. Terminating.\n", *ptr);
            if (err_code) *err_code = EIO;
            shift = 0;
        break;
    }
    return shift;
}

const char* get_file_name(const int argc, const char** argv) {
    const char* file_name = NULL;

    for (int argument_id = 1; argument_id < argc; ++argument_id) {
        if (*argv[argument_id] == '-') continue;
        file_name = argv[argument_id];
    }

    return file_name;
}