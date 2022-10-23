/**
 * @file common.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief Utility features useful in all programs.
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include "lib/alloc_tracker/alloc_tracker.h"

/**
 * @brief Return value but clean all allocations first.
 * 
 */
#define return_clean(value) do { free_all_allocations(); return value; } while (0)

//* We need this function as it is impossible to create static constant lists from local functions.
//* So... What it does is it creates a dynamic array and tracks it for later deletion.
/**
 * @brief Create list of void* pointers.
 * 
 * @param count number of elements
 * @param ... elements
 * @return array of elements
 */
void** bundle(size_t count, ...);

#endif