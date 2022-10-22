/**
 * @file alloc_tracker.h
 * @author Solodovnikov Ivan (solodovnikov.ia@phystech.edu)
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

//* Maximum number of allocations including 0-th element.
const size_t MAX_ALLOCATIONS = 2048 + 1;

typedef void dtor_t(void* subject);

/**
 * @brief Add allocation to the track list.
 * 
 * @param subject allocation subject
 * @param dtor destructor
 */
void track_allocation(void* subject, dtor_t *dtor);

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

#endif