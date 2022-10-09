/**
 * @file proccmd.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief List of processor commands
 * @version 0.1
 * @date 2022-10-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef PROC_CMD_H
#define PROC_CMD_H

#include "util/dbg/debug.h"

static const char CMD_COMMENT_CHAR = '#';

enum CMD_LIST {
    CMD_END,        //* End program.
    CMD_PUSH,       //* Push element to the stack.
    CMD_POP,        //* Remove element from the stack.
    CMD_OUT,        //* Print element of the stack to the screen.
    CMD_ADD,        //* Add last two elements.
    CMD_SUB,        //* Subtract last two elements from each other.
    CMD_MUL,        //* Multiply last two elements.
    CMD_DIV,        //* Divide last two elements.
    CMD_DUP,        //* Push copy of last element to the stack.
    CMD_ABORT,      //* Abort program execution.
    CMD_JMP,        //* Jump to specified point in the program.
    CMD_JMPG,       //* Jump to specified point in the program if last stack element is greater then the previous one.
};

//* Command as they should be written in source file.
static const char* CMD_SOURCE[] = {
    "END",
    "PUSH",
    "POP",
    "OUT",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "DUP",
    "ABORT",
    "JMP",
    "JMPG",
};

//* Command hashes.
static hash_t CMD_HASHES[sizeof(CMD_SOURCE) / sizeof(*CMD_SOURCE)];

/**
 * @brief [DO NOT CALL] Recalculate CMD_HASHES.
 * 
 * @return int 
 */
static int __cmd_calc_hashes();

static int __cmd_hash_calculator = __cmd_calc_hashes();

static int __cmd_calc_hashes() {
    for (int cmd_id = 0; cmd_id < (char)(sizeof(CMD_SOURCE) / sizeof(*CMD_SOURCE)); ++cmd_id) {
        const char* command = CMD_SOURCE[cmd_id];
        CMD_HASHES[cmd_id] = get_hash(command, command + strlen(command) - 1);
    }
    return 0;
}

#endif