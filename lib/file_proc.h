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

size_t parse_lines(FILE* file, char** *text, char* *buffer, int* error_code = NULL);

#endif