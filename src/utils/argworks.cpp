#include "argworks.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lib/util/dbg/logger.h"
#include "lib/util/dbg/debug.h"

PPArgument read_pparg(const char* arg_ptr) { // TODO: Imagine, I've read this, and still have no idea what pparg is
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
    if (length > max_length) { // TODO: How about repeat 6 lines 5 times, that seems fun
        max_length = length; // TODO: repeat
        answer.value = buf_id - 'A';
        answer.props = USE_MEMORY | USE_REGISTER;
    }

    sscanf(arg_ptr, " [ %d ]%n", &value_prev, &length);
    if (length > max_length) {
        max_length = length; // TODO: this 
        answer.value = value_prev;
        answer.props = USE_MEMORY;
    }

    sscanf(arg_ptr, " R%cX%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length; // TODO: more
        answer.value = buf_id - 'A';
        answer.props = USE_REGISTER;
    }

    sscanf(arg_ptr, " %d%n", &value_prev, &length);
    if (length > max_length) {
        max_length = length; // TODO: yes, more
        answer.value = value_prev;
        answer.props = 0;
    }

    sscanf(arg_ptr, " \'%c\'%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length; // TODO: yeeeeees
        answer.value = buf_id;
        answer.props = 0;
    }

    // TODO: like c'mon, extract this please

    sscanf(arg_ptr, " \'\\%c\'%n", &buf_id, &length);
    if (length > max_length) {
        max_length = length;
        switch (buf_id) {
            // TODO: have you hear about "macros"?
            case 'n': answer.value = '\n'; break;
            case 't': answer.value = '\t'; break;
            case 'r': answer.value = '\r'; break;
            case 'v': answer.value = '\v'; break;
            case 'a': answer.value = '\a'; break;
            case 'f': answer.value = '\f'; break;
            default:  answer.value=buf_id; break;
            // TODO:              ^ i know, i know 
            //                      but add spaces
        }
        answer.props = 0;
    }
    return answer;
}

// TODO: extract. arguments. in. struct.
int* link_argument(char usage, int *arg, MemorySegment ram, MemorySegment reg, int *err_code) {
    switch (usage) {
        case 0: {
            return arg;
        } break; // TODO: I've seen different style in your code, stick to one, please!

        case USE_MEMORY: {
            _LOG_FAIL_CHECK_(0 <= *arg && *arg < (int)ram.size, "error", ERROR_REPORTS, {
                log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d.\n", *arg);
                break;
            }, err_code, EFAULT);

            return ram.content + *arg; // TODO: you are returning ram.content + offset in every
                                       //       case, extract it!!
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

            // TODO:                                yeah, let's repeat again
            if (*arg < 0) *((char*)&pointer_shift + sizeof(pointer_shift) - 1) = (char)0xFF;
            // TODO: yeah, let's write important logic on one line, and than dedicate 4
            //       to a freaking logging statement

            _LOG_FAIL_CHECK_(0 <= reg_id && reg_id < (int)reg.size, "error", ERROR_REPORTS, {
                // TODO:     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ isn't bounds checking common?!
                log_printf(ERROR_REPORTS, "error", "Incorrect register index of %d.\n", reg_id);
                break;
            }, err_code, EFAULT);

            int ram_index = reg.content[reg_id] + pointer_shift;

            _LOG_FAIL_CHECK_(0 <= ram_index && ram_index < (int)ram.size, "error", ERROR_REPORTS, {
                // TODO:     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ literally not even 10 lines below same thing
                log_printf(ERROR_REPORTS, "error", "Incorrect memory index of %d.\n", ram_index);
                break;
            }, err_code, EFAULT);

            return ram.content + ram_index;
        } break;

        default: { log_printf(ERROR_REPORTS, "error", "Usage tag had an unexpected value of %d.\n", usage); }
        //       ^~ TODO: like why?
    }
    return NULL;
}

void write_argument(FILE* dest, char usage, int argument) {
    switch (usage) {
        case 0: {
            fprintf(dest, "%d", argument);
            if (isprint((char)argument)) {
                fprintf(dest, "\t\t# \'%c\'", (char)argument);
            }
        } break;

        case USE_MEMORY: {
            fprintf(dest, "[%d]", argument);
        } break;

        case USE_REGISTER: {
            fprintf(dest, "R%cX", (char)(argument + 'A'));
        } break;

        case USE_REGISTER | USE_MEMORY: {
            int reg_id = argument & 0xFF;

            int pointer_shift = 0;
            memcpy(&pointer_shift, (char*)&argument + 1, sizeof(pointer_shift) - 1);
            if (argument < 0) *((char*)&pointer_shift + sizeof(pointer_shift) - 1) = (char)0xFF;

            fprintf(dest, "[R%cX + %d]", (char)(reg_id + 'A'), pointer_shift);
        } break;

        default: {log_printf(ERROR_REPORTS, "error", "Usage tag had an unexpected value.\n");}
        //       ^~                                                                         ^~
        // TODO: roses are red, violates are blue, spaces are freeeee                       !
    }
}
