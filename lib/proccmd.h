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

static const char CMD_LABEL[] = "HERE";
static hash_t CMD_LABEL_HASH = get_hash(CMD_LABEL, CMD_LABEL + strlen(CMD_LABEL));

static const size_t LABEL_MAX_NAME_LENGTH = 128;

#define DEF_CMD(name, parse_script, exec_script) CMD_##name,

enum CMD_LIST {
    #include "cmddef.h"
};

#undef DEF_CMD
#define DEF_CMD(name, parse_script, exec_script) #name,

//* Command as they should be written in a source file.
static const char* CMD_SOURCE[] = {
    #include "cmddef.h"
};

#undef DEF_CMD

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
        CMD_HASHES[cmd_id] = get_hash(command, command + strlen(command));
    }
    return 0;
}

#endif