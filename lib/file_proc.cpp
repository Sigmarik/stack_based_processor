#include "file_proc.h"

#include <sys/stat.h>
#include "util/dbg/debug.h"

size_t flength(int fd) {
    struct stat buffer;
    fstat(fd, &buffer);
    return (size_t)buffer.st_size;
}

size_t parse_lines(FILE* file, char** *text, char* *buffer, int* error_code) {
    _LOG_FAIL_CHECK_(file, "error", ERROR_REPORTS, return 0, error_code, EFAULT);

    size_t file_size = flength(fileno(file));

    size_t line_count = 1;

    *buffer = (char*)calloc(file_size + 1UL, sizeof(**buffer));
    _LOG_FAIL_CHECK_(*buffer, "error", ERROR_REPORTS, return 0, error_code, ENOMEM);

    for (size_t char_id = 0; char_id < file_size; char_id++) {
        (*buffer)[char_id] = (char)fgetc(file);
        if ((*buffer)[char_id] == '\n') {
            line_count++;
            (*buffer)[char_id] = '\0';
        }
    }

    *text = (char**)calloc(line_count, sizeof(**text));
    _LOG_FAIL_CHECK_(*text, "error", ERROR_REPORTS, return 0, error_code, ENOMEM);

    (*text)[0] = *buffer;
    int line_id = 0;
    for (size_t char_id = 0; char_id < file_size; char_id++) {
        if ((*buffer)[char_id] == '\0') {
            (*buffer)[char_id] = 0;
            (*text)[++line_id] = (*buffer) + char_id + 1;
        }
    }

    return line_count;
}