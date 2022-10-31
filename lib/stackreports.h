/**
 * @file stackreports.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Stack status report tags.
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef STACK_REPORTS_H
#define STACK_REPORTS_H

typedef int stack_report_t;
enum StackStatus {
    STACK_NULL = 1 << 0,
    STACK_BIG_SIZE = 1 << 1,
    STACK_NULL_CONTENT = 1 << 2,
    STACK_L_CANARY_FAIL = 1 << 3,
    STACK_R_CANARY_FAIL = 1 << 4,
    STACK_BL_CANARY_FAIL = 1 << 5,
    STACK_BR_CANARY_FAIL = 1 << 6,
    STACK_HASH_FAILURE = 1 << 7,
};

static const char* const STACK_STATUS_DESCR[] = {
    "Stack pointer is invalid.",
    "Stack size is bigger than its capacity.",
    "Stack has no buffer.",
    "Stack left canary is corrupt.",
    "Stack right canary is corrupt.",
    "Stack buffer left canary is corrupt.",
    "Stack buffer right canary is corrupt.",
    "Stack hash was invalid."
};

#endif
