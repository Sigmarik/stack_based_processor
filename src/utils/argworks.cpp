#include "argworks.h"

#include <stdio.h>
#include <string.h>

#include "lib/util/dbg/logger.h"
#include "lib/util/dbg/debug.h"

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

int* link_argument(char usage, int *arg, MemorySegment ram, MemorySegment reg, int *err_code) {
    switch (usage) {
        case 0: {
            return arg;
        } break;

        case USE_MEMORY: {
            _LOG_FAIL_CHECK_(0 <= *arg && *arg < (int)ram.size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d.\n", *arg);
                break;
            }, err_code, EFAULT);

            return ram.content + *arg;
        } break;

        case USE_REGISTER: {
            _LOG_FAIL_CHECK_(0 <= *arg && *arg < (int)reg.size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d.\n", *arg);
                break;
            }, err_code, EFAULT);

            log_printf(STATUS_REPORTS, "status", "Register address = %p.\n", reg.content + *arg);

            return reg.content + *arg;
        } break;

        case USE_REGISTER | USE_MEMORY: {
            int reg_id = *arg & 0xFF;

            int pointer_shift = 0;
            memcpy(&pointer_shift, (char*)arg + 1, sizeof(pointer_shift) - 1);
            if (*arg < 0) *((char*)&pointer_shift + sizeof(pointer_shift) - 1) = (char)0xFF;

            _LOG_FAIL_CHECK_(0 <= reg_id && reg_id < (int)reg.size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d.\n", reg_id);
                break;
            }, err_code, EFAULT);

            int ram_index = reg.content[reg_id] + pointer_shift;

            _LOG_FAIL_CHECK_(0 <= ram_index && ram_index < (int)ram.size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d.\n", ram_index);
                break;
            }, err_code, EFAULT);

            return ram.content + ram_index;
        } break;

        default: { log_printf(ERROR_REPORTS, "error", "Usage tag had an unexpected value of %d.\n", usage); }
    }
    return NULL;
}