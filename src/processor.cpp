/**
 * @file main.c
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Program for executing commands from binary file (virtual processor).
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
#include "utils/common.h"

//* warning: stack protector not protecting function: all local arrays are less than 8 bytes long [-Wstack-protector]
#pragma GCC diagnostic ignored "-Wstack-protector"

#define PROCESSOR

typedef long long stack_content_t;
stack_content_t STACK_CONTENT_POISON = (stack_content_t) 0xDEADBABEC0FEBEEF;
#include "lib/stackworks.h"

struct MemorySegment {
    int* content = NULL;
    size_t size = 1024;
};

void MemorySegment_ctor(MemorySegment* segment);
void MemorySegment_dtor(MemorySegment* segment);

struct FrameBuffer {
    int* content = NULL;
    size_t width = 32;
    size_t height = 32;
};

void FrameBuffer_ctor(FrameBuffer* buffer);
void FrameBuffer_dtor(FrameBuffer* buffer);

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
 * @param err_code variable to use as errno
 * @return size_t required pointer shift
 */
size_t read_header(char* ptr, int* err_code = NULL);

/**
 * @brief Execute one command and return pointer shift.
 * 
 * @param ptr pointer to the pointer to the command
 * @param stack stack to operate on
 * @param addr_stack address stack
 * @param ram virtual RAM
 * @param reg virtual register
 * @param vmd virtual video memory device
 * @param err_code error code
 * @return size_t
 */
int execute_command(const char* prog_start, const char* ptr, 
                    Stack* const stack, Stack* const addr_stack,
                    MemorySegment ram, MemorySegment reg, FrameBuffer vmd, 
                    int* const err_code = NULL);

/**
 * @brief Get the file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_file_name(const int argc, const char** argv);

/**
 * @brief Make console empty.
 * 
 */
void clear_console();

/**
 * @brief Draw picture stored in VMD to the screen with UTF8 characters.
 * 
 */
void draw_vmd(FrameBuffer* buffer);

static const char PIX_STATES[] = R"($@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'. )";

static const int NUMBER_OF_OWLS = 10;

int main(const int argc, const char** argv) {
    atexit(log_end_program);

    // Ignore everything less or equaly important as status reports.
    unsigned int log_threshold = STATUS_REPORTS + 1;
    FrameBuffer vmd = {};
    MemorySegment ram = {};
    MemorySegment reg = {};

    static const struct ActionTag line_tags[] = {
        #include "cmd_flags/processor_flags.h"
    };
    static const int number_of_tags = sizeof(line_tags) / sizeof(*line_tags);

    parse_args(argc, argv, number_of_tags, line_tags);
    log_init("program_log.log", log_threshold, &errno);
    print_label();

    FrameBuffer_ctor(&vmd);
    MemorySegment_ctor(&ram);
    MemorySegment_ctor(&reg);
    track_allocation(&vmd, (dtor_t*)FrameBuffer_dtor);
    track_allocation(&ram, (dtor_t*)MemorySegment_dtor);
    track_allocation(&reg, (dtor_t*)MemorySegment_dtor);

    _LOG_FAIL_CHECK_(vmd.content && ram.content && reg.content, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to initialize virtual memory segments, terminating.\n");

        return_clean(EXIT_FAILURE);

    }, &errno, ENOMEM);

    if (ram.size > 0) ram.content[0] = (int)ram.size;
    if (ram.size > 1) ram.content[1] = (int)vmd.width;
    if (ram.size > 2) ram.content[2] = (int)vmd.height;
    if (ram.size > 3) ram.content[3] = (int)sizeof(PIX_STATES);

    const char* file_name = get_file_name(argc, argv);
    _LOG_FAIL_CHECK_(file_name, "error", ERROR_REPORTS, {
        printf("File was not specified, terminating...\n");
        printf("To execute program stored in a file run\n%s [file name]\n", argv[0]);

        return_clean(EXIT_FAILURE);

    }, NULL, 0);

    log_printf(STATUS_REPORTS, "status", "Opening file %s.\n", file_name);
    int fd = open(file_name, O_RDONLY);
    _LOG_FAIL_CHECK_(fd != -1, "error", ERROR_REPORTS, {
        printf("File was not opened, terminating...\n");

        return_clean(EXIT_FAILURE);

    }, NULL, 0);
    size_t size = flength(fd);

    log_printf(STATUS_REPORTS, "status", "Reading file content...\n");
    char* content = (char*) calloc(size, sizeof(*content));
    track_allocation(&content, (dtor_t*)free_var);
    read(fd, content, size);
    close(fd);

    char* pointer = content;

    log_printf(STATUS_REPORTS, "status", "Initializing stack...\n");

    Stack stack = {};
    Stack addr_stack = {};

    stack_init(&stack, STACK_START_SIZE, &errno);
    _LOG_FAIL_CHECK_(!stack_status(&stack), "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to initialize main stack.\n");
        stack_dump(&stack, ERROR_REPORTS);
    }, NULL, 0);

    track_allocation(&stack, (dtor_t*)stack_destroy_void);

    stack_init(&addr_stack, ADDR_STACK_START_SIZE, &errno);
    _LOG_FAIL_CHECK_(!stack_status(&addr_stack), "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Failed to initialize addr stack.\n");
        stack_dump(&addr_stack, ERROR_REPORTS);
    }, NULL, 0);

    track_allocation(&addr_stack, (dtor_t*)stack_destroy_void);

    log_printf(STATUS_REPORTS, "status", "Reading file header...\n");
    size_t prefix_shift = read_header(pointer, &errno);
    pointer += prefix_shift;
    _LOG_FAIL_CHECK_(prefix_shift, "error", ERROR_REPORTS, return_clean(EXIT_FAILURE), NULL, 0);

    log_printf(STATUS_REPORTS, "status", "Starting executing commands...\n");
    int delta = 0;
    while ((delta = execute_command(content, pointer, &stack, &addr_stack, ram, reg, vmd, &errno)) != 0) {

        char* prev_ptr = pointer;

        pointer += delta;

        _LOG_FAIL_CHECK_(pointer > content && pointer < content + size, "error", ERROR_REPORTS, {
            log_printf(ERROR_REPORTS, "error", "Invalid pointer value of 0x%0*X after executing command at 0x%0*X. Terminating.\n", 
                                                sizeof(uintptr_t), pointer - content, sizeof(uintptr_t), prev_ptr - content);

            return_clean(EXIT_FAILURE);

        }, &errno, EFAULT);
    }

    log_printf(STATUS_REPORTS, "status", "Execution finished, cleaning allocated memory...\n");

    return_clean(errno ? EXIT_FAILURE : EXIT_SUCCESS);
}

void MemorySegment_ctor(MemorySegment* segment) {
    segment->content = (int*) calloc(segment->size, sizeof(*segment->content));
}

void MemorySegment_dtor(MemorySegment* segment) {
    free(segment->content);
    segment->content = NULL;
    segment->size = 0;
}

void FrameBuffer_ctor(FrameBuffer* buffer) {
    buffer->content = (int*) calloc(buffer->width * buffer->height, sizeof(*buffer->content));
}

void FrameBuffer_dtor(FrameBuffer* buffer) {
    free(buffer->content);
    buffer->content = NULL;
    buffer->width = 0;
    buffer->height = 0;
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

#define DEF_CMD(name, parse_script, exec_script, disasm_script) case CMD_##name: {exec_script;} break;

#define STACK       stack
#define ADDR_STACK  addr_stack
#define SHIFT       shift
#define EXEC_POINT  ptr
#define ARG_PTR     ptr + 1
#define ERRNO       err_code
#define REG_SIZE    reg.size
#define REG         reg.content
#define RAM_SIZE    ram.size
#define RAM         ram.content
#define VMD_SIZE    (vmd.width * vmd.height)
#define VMD         vmd.content

int execute_command(const char* prog_start, const char* ptr,
                    Stack* const stack, Stack* const addr_stack,
                    MemorySegment ram, MemorySegment reg, FrameBuffer vmd,
                    int* const err_code) {
    _LOG_FAIL_CHECK_(ptr, "error", ERROR_REPORTS, return 0, err_code, EFAULT);

    log_printf(STATUS_REPORTS, "status", "Executing command %02X (mask %d) at 0x%0*X.\n", 
               (*ptr >> 2) & 0xFF, *ptr & 3, sizeof(prog_start), ptr - prog_start);
    
    _LOG_FAIL_CHECK_(stack_status(stack) == 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Memory stack status check failed.\n");
        stack_dump(stack, ERROR_REPORTS);
        return 0;
    }, NULL, 0);

    _LOG_FAIL_CHECK_(stack_status(addr_stack) == 0, "error", ERROR_REPORTS, {
        log_printf(ERROR_REPORTS, "error", "Address stack status check failed.\n");
        stack_dump(addr_stack, ERROR_REPORTS);
        return 0;
    }, NULL, 0);

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

#ifdef __linux__
void clear_console() {
    system("clear");
}
#elif defined(WIN32) || defined(WIN64)
void clear_console() {
    system("cls");
}
#endif

void draw_vmd(FrameBuffer* buffer) {
    for (int id_y = 0; id_y < (int)buffer->height; ++id_y) {
        for (int id_x = 0; id_x < (int)buffer->width; ++id_x) {

            int brightness = buffer->content[id_y * (int)buffer->width + id_x];

            if (brightness < 0) brightness = 0;
            if (brightness >= (int)sizeof(PIX_STATES)) brightness = (int)sizeof(PIX_STATES) - 1;

            putc(PIX_STATES[brightness], stdout);
        }
        putc('\n', stdout);
    }
}