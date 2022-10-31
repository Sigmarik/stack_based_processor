/**
 * @file procinfo.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Information about processor like version, prefix e.t.c.
 * @version 0.1
 * @date 2022-10-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>

typedef int version_t;
const version_t PROC_VERSION = 1;
const size_t HEADER_SIZE = 16; // TODO: how have you calculated header size? Explain, so
                               //       reader knows can he change it. I think it's constrained!
const char FILE_PREFIX[] = "KITy";
const size_t PREFIX_SIZE = sizeof(FILE_PREFIX) - 1;
