/**
 * @file debug.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Module with debugging information.
 * @version 0.1
 * @date 2022-08-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef DEBUGGING_H
#define DEBUGGING_H

// TODO: maybe naming it something like SILENCE_UNUSED would be a bit better choice)) 
#define UNUSE(var) do { (void)(var); } while (0)
// TODO: in case of one stmt you could just write (void) (var) without do...while :)
//       but ok, you're playing it safe, i can respect that.

/* NDEBUG should be here-> */ // Do not set NDEBUG in source code it's a pain,
                              // you'd have to do it in every translation unit,
                              // or create such adapters, not to say that
                              // maintaining release and debug build at the same
                              // time will be impossible!
                              //
                              // TODO: But it's a solved problem, do it with compiler option -D
//#define NDEBUG
#include <assert.h>
#include <errno.h>

#include "logger.h"

typedef unsigned long long hash_t;

/**
 * @brief List of error types to put into errno.
 */
enum ERRORS {
    INPUT_ERROR = -1,
    NULLPTR_ERROR = -2,
    FILE_ERROR = -3,
};

/**
 * @brief Print description to log file if equation fails.
 * 
 * @param equation value to use as an inverse trigger for assert
 * @param tag prefix to print before failure message
 * @param importance
 * @param action sequence to run of failure
 * @param errcode variable to write errtype in
 * @param arrtype error code
 */ // TODO: "equations" are a math thing, this is more of a "condition"
#define _LOG_FAIL_CHECK_(equation, tag, importance, action, errcode, errtype)                                           \
do {                                                                                                                    \
    if (!(equation)) {                                                                                                  \
        int* errptr = errcode;                                                                                          \
        if (errptr) *(errptr) = (errtype);                              \
        log_printf(importance, tag, "Equation `%s` in file %s at line %d failed.\n", #equation, __FILE__, __LINE__);\
        action;                                                                                                         \
    }                                                                                                                   \
} while(0)


/**
 * @brief Print errno value and its description and close log file.
 */
void log_end_program();

/**
 * @brief Check if pointer is readable.
 * 
 * @param ptr pointer to check
 * @return true if pointer is valid, 
 * @return false otherwise
 */
bool check_ptr(const void* ptr);

/**
 * @brief End program if errno is not zero.
 * 
 */
#define _ABORT_ON_ERRNO_() _LOG_FAIL_CHECK_(!errno, "FATAL ERROR", ABSOLUTE_IMPORTANCE, exit(EXIT_FAILURE);, NULL, 0);

/**
 * @brief Calculate hash value of the buffer.
 * 
 * @param start pointer to the start of the buffer
 * @param end pointer to the end of the buffer
 * @return hash_t 
 */
hash_t get_hash(const void* start, const void* end); // TODO: get_hash definitely doesn't belong in debug!

#endif
