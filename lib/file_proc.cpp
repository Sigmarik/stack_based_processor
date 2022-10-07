#include "file_proc.h"

#include <sys/stat.h>
#include "util/dbg/debug.h"

size_t flength(int fd) {
    struct stat buffer;
    fstat(fd, &buffer);
    return buffer.st_size;
}

int parse_lines(FILE* file, char** *text, char* *buffer, int* error_code) {
    _LOG_FAIL_CHECK_(file, "error", ERROR_REPORTS, return 0, error_code, EFAULT);

    int file_size = flength(fileno(file));

    int line_count = 1;

    *buffer = (char*)calloc(file_size + 1, sizeof(**buffer));
    _LOG_FAIL_CHECK_(*buffer, "error", ERROR_REPORTS, return 0, error_code, ENOMEM);

    for (int char_id = 0; char_id < file_size; char_id++) {
        (*buffer)[char_id] = fgetc(file);
        if ((*buffer)[char_id] == '\n') {
            line_count++;
            (*buffer)[char_id] = '\0';
        }
    }

    *text = (char**)calloc(line_count, sizeof(**text));
    _LOG_FAIL_CHECK_(*text, "error", ERROR_REPORTS, return 0, error_code, ENOMEM);

    (*text)[0] = *buffer;
    int line_id = 0;
    for (int char_id = 0; char_id < file_size; char_id++) {
        if ((*buffer)[char_id] == '\0') {
            (*buffer)[char_id] = 0;
            (*text)[++line_id] = (*buffer) + char_id + 1;
        }
    }

    return line_count;
}