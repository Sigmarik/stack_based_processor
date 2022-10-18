/**
 * @file file_proc.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Library for manipulating text and binary files.
 * @version 0.1
 * @date 2022-10-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef FILE_PROC_H
#define FILE_PROC_H

#include <cstdlib>
#include <stdio.h>

/**
 * @brief Get length of the file.
 * 
 * @param fd file descriptor
 * @return size_t 
 */
size_t flength(int fd);

/**
 * @brief Slice text stored in file into multiple lines.
 * 
 * @param file file to read
 * @param text where to put array of text lines
 * @param buffer where to store entirety of text
 * @param error_code variable to use as errno
 * @return size_t 
 */
size_t parse_lines(FILE* file, char** *text, char* *buffer, int* error_code = NULL);

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

#endif