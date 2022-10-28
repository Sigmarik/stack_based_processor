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

/**
 * @brief Array with stored size.
 * 
 */
struct MemorySegment {
    int* content = NULL;
    size_t size = 1024;
};

void MemorySegment_ctor(MemorySegment* segment);
void MemorySegment_dtor(MemorySegment* segment);

#define MemorySegment_dump(segment, importance) do { \
    log_printf(importance, "dump", "Memory segment at %p. Size: %ld Content: %p\n", segment, (segment)->size, (segment)->content); \
    _MemorySegment_dump(segment, importance); \
} while (0)

void _MemorySegment_dump(MemorySegment* segment, unsigned int importance);

/**
 * @brief Pseudo-2D array with defined dimensions.
 * 
 */
struct FrameBuffer {
    int* content = NULL;
    size_t width = 64;
    size_t height = 32;
};

void FrameBuffer_ctor(FrameBuffer* buffer);
void FrameBuffer_dtor(FrameBuffer* buffer);

/**
 * @brief Limit specified value between left and right border.
 * 
 * @param value variable to check
 * @param left left border
 * @param right right border
 * @return 
 */
int clamp(const int value, const int left, const int right);

/**
 * @brief Print a bunch of owls.
 * 
 * @param argc unimportant
 * @param argv unimportant
 * @param argument unimportant
 */
void print_owl(const int argc, void** argv, const char* argument);

/**
 * @brief Get the input file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_input_file_name(const int argc, const char** argv);

/**
 * @brief Get the output file name from the list of command line arguments.
 * 
 * @param argc argument count
 * @param argv argument values
 * @return const char* 
 */
const char* get_output_file_name(const int argc, const char** argv);

#endif