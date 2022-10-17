/**
 * @file cmddef.h
 * @author Ilya Kudryashov (kudriashov.it@phystech.edu)
 * @brief List of commads.
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef ASSEMBLER
    #ifndef ARG_PTR
        #error ARG_PTR was not defined while trying to access assembly code.
    #endif
    #ifndef GET_LABEL
        #error GET_LABEL was not defined while trying to access assembly code.
    #endif
    #ifndef CUR_ID
        #error CUR_ID was not defined while trying to access assembly code.
    #endif
    #ifndef BUF_WRITE
        #error BUF_WRITE was not defined while trying to access assembly code.
    #endif
    #ifndef ERRNO
        #error ERRNO was not defined while trying to access assembly code.
    #endif
#endif
#ifdef PROCESSOR
    #ifndef STACK
        #error STACK was not defined while trying to access execution code.
    #endif
    #ifndef ADDR_STACK
        #error ADDR_STACK was not defined while trying to access execution code.
    #endif
    #ifndef SHIFT
        #error SHIFT was not defined while trying to access execution code.
    #endif
    #ifndef EXEC_POINT
        #error EXEC_POINT was not defined while trying to access execution code.
    #endif
    #ifndef ARG_PTR
        #error ARG_PTR was not defined while trying to access execution code.
    #endif
    #ifndef ERRNO
        #error ERRNO was not defined while trying to access execution code.
    #endif
    #ifndef REG_SIZE
        #error REG_SIZE was not defined while trying to access execution code.
    #endif
    #ifndef RAM_SIZE
        #error RAM_SIZE was not defined while trying to access execution code.
    #endif
    #ifndef RAM
        #error RAM was not defined while trying to access execution code.
    #endif
    #ifndef VMD_SIZE
        #error VMD_SIZE was not defined while trying to access execution code.
    #endif
    #ifndef VMD
        #error VMD was not defined while trying to access execution code.
    #endif
#endif

#include "cmds/flow.h"
#include "cmds/memops.h"
#include "cmds/arifm.h"
#include "cmds/interaction.h"