/**
 * @file argworks.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief PUSH/MOVE argument parsing functions.
 * @version 0.1
 * @date 2022-10-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ARGWORKS_H
#define ARGWORKS_H

#include "common.h"

enum USAGE_TYPES {
    USE_REGISTER = 1 << 0,
    USE_MEMORY = 1 << 1,
};

struct PPArgument {
    int value = 0;
    char props = 0;
};

/**
 * @brief Extract push/pop argument from the string.
 * 
 * @param arg_ptr argument
 * @return PPArgument 
 */
PPArgument read_pparg(const char* arg_ptr);

/**
 * @brief Get operation subject by usage tags.
 * 
 * @param usage 
 * @param arg 
 * @param ram 
 * @param reg 
 * @param err_code 
 * @return 
 */
int* link_argument(char usage, int *arg, MemorySegment ram, MemorySegment reg, int *err_code = NULL);

#endif