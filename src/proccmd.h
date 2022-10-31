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

#include "lib/util/dbg/debug.h"

static const char CMD_COMMENT_CHAR = '#';

static const char CMD_LABEL[] = "HERE";
static hash_t CMD_LABEL_HASH = get_hash(CMD_LABEL, CMD_LABEL + strlen(CMD_LABEL));
// TODO: #include <string.h>                                   ^~~~~~~~~~~~~~~~~!

static const size_t LABEL_MAX_NAME_LENGTH = 128;

#define DEF_CMD(name, parse_script, exec_script, disasm_script) CMD_##name,

enum CMD_LIST {
    #include "cmddef.h"
};

#undef DEF_CMD
#define DEF_CMD(name, parse_script, exec_script, disasm_script) #name,

//* Command as they should be written in a source file.
static const char* CMD_SOURCE[] = {
    #include "cmddef.h"
};

#undef DEF_CMD

//* Command hashes.
static hash_t CMD_HASHES[sizeof(CMD_SOURCE) / sizeof(*CMD_SOURCE)];
//                       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TODO: imagine you mistype this in one of 1000 places you copied this to...
//       What a funny bug to hunt down. Like really, bell in your head should
//       have already told you to extract it long time ago.

// Fine tune that compas in your brain, so it sees it.

/**
 * @brief [DO NOT CALL] Recalculate CMD_HASHES.
 * 
 * @return int 
 */
static int __cmd_calc_hashes();

static int __cmd_hash_calculator = __cmd_calc_hashes();

static int __cmd_calc_hashes() { // TODO: you can use __attribute__((constructor)) if you don't
                                 //       care about portability
    for (int cmd_id = 0; cmd_id < (char)(sizeof(CMD_SOURCE) / sizeof(*CMD_SOURCE)); ++cmd_id) {
        const char* command = CMD_SOURCE[cmd_id];
        CMD_HASHES[cmd_id] = get_hash(command, command + strlen(command));
    }
    return 0;
}

#endif
