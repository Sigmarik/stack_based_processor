/**
 * @file alloc_tracker.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief Allocation tracker.
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ALLOC_TRACKER_H
#define ALLOC_TRACKER_H

#include "stdlib.h"
#include "lib/util/dbg/logger.h"
#include "lib/util/dbg/debug.h"

//* Maximum number of allocations including 0-th element.
const size_t MAX_ALLOCATIONS = 1024 + 1;

typedef void dtor_t(void* subject);

/**
 * @brief Add allocation to the track list.
 * 
 * @param subject allocation subject
 * @param dtor destructor
 */
#define track_allocation(subject, dtor) do { \
    log_printf(STATUS_REPORTS, "status", "Processing tracking request " #subject \
                                         " with destructor "#dtor" in %s in %s:%d.\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); \
    _track_allocation(subject, dtor); \
} while (0)

void _track_allocation(void* subject, dtor_t *dtor);

/**
 * @brief Untrack allocation by variable.
 * 
 * @param subject 
 */
void untrack_allocation(void* subject);

/**
 * @brief Destroy given allocation.
 * 
 * @param subject allocation subject
 */
void free_allocation(void* subject);

/**
 * @brief Destroy all given allocations.
 * 
 */
void free_all_allocations();

/**
 * @brief Call free() on pointer and set its value to zero.
 * 
 * @param ptr pointer to the variable to reset
 */
void free_var(void** ptr);

/**
 * @brief Safely call free() on pointer.
 * 
 * @param ptr
 */
void void_free(void* ptr);

#endif