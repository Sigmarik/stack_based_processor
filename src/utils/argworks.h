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

enum USAGE_TYPES { // TODO: Read about enum naming I wrote in stackreports.h
    USE_REGISTER = 1 << 0,
    USE_MEMORY = 1 << 1,
};

struct PPArgument { // TODO: What is PPA? Don't use confusing abbrivations, just use full names
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
 * @param usage command usage tag
 * @param arg command argument
 * @param ram RAM memory segment
 * @param reg register
 * @param err_code variable to use as errno
 * @return int* operation subject
 */
int* link_argument(char usage, int *arg, MemorySegment ram, MemorySegment reg, int *err_code = NULL);

/**
 * @brief Write disassembled argument to file.
 * 
 * @param dest write destination
 * @param usage command usage tag
 * @param argument command argument
 */
void write_argument(FILE* dest, char usage, int argument);

#endif
