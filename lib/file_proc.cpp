#include "file_proc.h"

#include <string.h>
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

PPArgument read_pparg(const char* arg_ptr) {
    _LOG_FAIL_CHECK_(arg_ptr, "error", ERROR_REPORTS, return (PPArgument){}, NULL, 0);

    PPArgument answer = {};
    int max_length = 0, length = 0;

    char first_char = '\0';
    sscanf(arg_ptr, " %c", &first_char);
    if (first_char == '\0') return answer;

    char buf_id = '\0';
    char op_sign = '\0';
    int value_prev = 0;

    sscanf(arg_ptr, " [ R%cX %c %d ]%n", &buf_id, &op_sign, &value_prev, &length);
    if (length > max_length) {
        max_length = length;
        if (op_sign == '-') value_prev *= -1;
        memcpy((char*)&answer.value + 1, &value_prev, sizeof(answer.value) - 1);
        answer.value |= buf_id - 'A';
        answer.props = USE_MEMORY | USE_REGISTER;
    }

    sscanf(arg_ptr, " [ R%cX ]%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length;
        answer.value = buf_id - 'A';
        answer.props = USE_MEMORY | USE_REGISTER;
    }

    sscanf(arg_ptr, " [ %d ]%n", &value_prev, &length);
    if (length > max_length) {
        max_length = length;
        answer.value = value_prev;
        answer.props = USE_MEMORY;
    }

    sscanf(arg_ptr, " R%cX%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length;
        answer.value = buf_id - 'A';
        answer.props = USE_REGISTER;
    }

    sscanf(arg_ptr, " %d%n", &value_prev, &length);
    if (length > max_length) {
        max_length = length;
        answer.value = value_prev;
        answer.props = 0;
    }

    sscanf(arg_ptr, " \'%c\'%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length;
        answer.value = buf_id;
        answer.props = 0;
    }

    sscanf(arg_ptr, " \'\\%c\'%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length;
        switch (buf_id) {
            case 'n': answer.value = '\n'; break;
            case 't': answer.value = '\t'; break;
            case 'r': answer.value = '\r'; break;
            case 'v': answer.value = '\v'; break;
            case 'a': answer.value = '\a'; break;
            case 'f': answer.value = '\f'; break;
            default:  answer.value=buf_id; break;
        }
        answer.props = 0;
    }
    return answer;
}

void fclose_var(FILE** file_var) {
    _LOG_FAIL_CHECK_(file_var, "error", ERROR_REPORTS, return, &errno, EFAULT);
    log_printf(STATUS_REPORTS, "status", "Closing file %p.\n", *file_var);
    if (*file_var) fclose(*file_var);
    *file_var = NULL;
}

void void_fclose(FILE* file) {
    if (file) fclose(file);
}