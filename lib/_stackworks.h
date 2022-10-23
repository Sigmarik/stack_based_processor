/**
 * @file stackworks.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief Stack implementation
 * @version 0.1
 * @date 2022-09-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef STACKWORKS_H
#define STACKWORKS_H

//* In case of a compilation error on this line check if [stack_content_t STACK_CONTENT_POISON] is defined before the library include.
static const stack_content_t ___STACK_CONTENT_OR_POISON_IS_NOT_DEFINED___ = STACK_CONTENT_POISON;
//* Define [stack_content_t] and [stack_content_t STACK_CONTENT_POISON] with the value you want stack to consider poisonous.
//* Example:
//*   typedef char stack_content_t;
//*   static const stack_content_t STACK_CONTENT_POISON = '\0';
//*   #include "stackworks.h"

#include <stdalign.h>
#include <fcntl.h>
#include <stdint.h>
#include "util/dbg/debug.h"
#include "util/dbg/logger.h"
#include "stackreports.h"

#ifndef NCANARY
#define ON_CANARY(...) __VA_ARGS__
#else
#define ON_CANARY(...)
#endif

#ifndef NHASH
#define ON_HASH(...) __VA_ARGS__
#else
#define ON_HASH(...)
#endif

// TODO: Make a separate structure for stack statuses.
static const char* const STACK_STATUS_DESCR[] = {
    "Stack pointer is invalid.",
    "Stack size is bigger than its capacity.",
    "Stack has no buffer.",
    "Stack left canary is corrupt.",
    "Stack right canary is corrupt.",
    "Stack buffer left canary is corrupt.",
    "Stack buffer right canary is corrupt.",
    "Stack hash was wrong."
};

#define STACK_CANARY_VALUE "CANARY"
typedef char stack_canary_t[7];
typedef hash_t stack_hash_t;

static const size_t STACK_BUFFER_INCREASE = 2;

struct Stack {
    ON_CANARY(stack_canary_t _canary_left = STACK_CANARY_VALUE;)

    char* buffer = NULL;
    uintptr_t size = 0;
    uintptr_t capacity = 0;

    ON_HASH(stack_hash_t _hash = 0;)
    ON_CANARY(stack_canary_t _canary_right = STACK_CANARY_VALUE;)
};

/**
 * @brief Initialize stack.
 * 
 * @param stack structure to initialize
 * @param size starting size of the stack
 * @param err_code variable to fill with error code
 */
void stack_init(Stack* const stack, const size_t size, int* const err_code = NULL);

/**
 * @brief Destroy stack.
 * 
 * @param stack structure to destroy
 * @param err_code variable to fill with error code
 */
void stack_destroy(Stack* const stack, int* const err_code = NULL);

/**
 * @brief Push one element to stack.
 * 
 * @param stack structure to push value into
 * @param value element to push
 * @param err_code variable to fill with error code
 */
void stack_push(Stack* const stack, const stack_content_t value, int* const err_code = NULL);

/**
 * @brief Remove lest element from the stack.
 * 
 * @param stack structure to modify
 * @param err_code variable to fill with error code
 */
void stack_pop(Stack* const stack, int* const err_code = NULL);

/**
 * @brief Get last element of the stack.
 * 
 * @param stack 
 * @param err_code 
 * @return stack_content_t 
 */
stack_content_t stack_get(Stack* const stack, int* const err_code = NULL);

/**
 * @brief Return status of the stack.
 * 
 * @param stack structure to check
 * @return stack_report_t 
 */
stack_report_t stack_status(const Stack* const stack);

/**
 * @brief Check if variable stores canary value.
 * 
 * @param value variable to check
 * @return true
 * @return false 
 */
bool stack_check_canary(const stack_canary_t value);

#ifndef STACK_DUMP_MAX_LINES
#define STACK_DUMP_MAX_LINES 64
#endif

#ifndef STACK_DUMP_MAX_BITES
#define STACK_DUMP_MAX_BITES 32
#endif

/**
 * @brief Print detailed information about the stack into logs.
 * 
 * @param stack
 */
#define stack_dump(stack, importance) _stack_dump(stack, importance, __PRETTY_FUNCTION__, __LINE__, __FILE__)
void _stack_dump(Stack* const stack, unsigned int importance, const char* function, const size_t line, const char* file);

/**
 * @brief Change the size of the stack.
 * 
 * @param stack structure to modify
 * @param new_size new capacity of the stack
 * @param err_code variable to fill with error code
 */
void _stack_change_size(Stack* const stack, const size_t new_size, int* const err_code = NULL);

/**
 * @brief Allocate memory for stack buffer and place canaries.
 * 
 * @param count element count
 * @param err_code variable to fill with error code
 * @return char* 
 */
char* _stack_alloc_space(const size_t count, int* const err_code = NULL);

/**
 * @brief Get pointer to the first element stored in the stack.
 * 
 * @param stack 
 * @return stack_content_t* 
 */
stack_content_t* _stack_content(const Stack* const stack);

/**
 * @brief Calculate stack hash.
 * 
 * @param stack 
 * @return stack_hash_t 
 */
stack_hash_t _stack_hash(const Stack* const  stack);

/**
 * @brief Same as stack_destroy, but capable with dtor_t.
 * 
 * @param stack stack to destroy
 */
void stack_destroy_void(Stack* stack);

#endif