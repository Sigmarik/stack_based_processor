#include "file_proc.h"

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util/dbg/debug.h"

size_t get_file_length(int fd) {
    struct stat buffer;
    fstat(fd, &buffer);
    return (size_t)buffer.st_size;
}

size_t parse_lines(FILE* file, char** *text, char* *buffer, int* error_code) {
    _LOG_FAIL_CHECK_(file, "error", ERROR_REPORTS, return 0, error_code, EFAULT);

    size_t file_size = get_file_length(fileno(file));

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

void fclose_var(FILE** file_var) {
    _LOG_FAIL_CHECK_(file_var, "error", ERROR_REPORTS, return, &errno, EFAULT);
    log_printf(STATUS_REPORTS, "status", "FClosing file %p.\n", *file_var);
    if (*file_var) fclose(*file_var);
    *file_var = NULL;
}

void close_var(int* fd_var) {
    _LOG_FAIL_CHECK_(fd_var, "error", ERROR_REPORTS, return, &errno, EFAULT);
    log_printf(STATUS_REPORTS, "status", "Closing file %D.\n", *fd_var);
    if (*fd_var) close(*fd_var);
    *fd_var = 0;
}

void void_fclose(FILE* file) {
    if (file) fclose(file);
}
