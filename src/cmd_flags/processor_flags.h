/**
 * @file processor_flags.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief List of command line arguments for processor.
 * @version 0.1
 * @date 2022-10-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// TODO: read assembler_flags.h

#include "common_flags.h"

{ {'R', ""}, { bundle(1, &reg.size), 1, edit_int},
    "set max. number of cells register can hold.\n"
    "\tDoes not check if integer was specified." },

{ {'R', ""}, { bundle(1, &ram.size), 1, edit_int},
    "set RAM size in cells.\n"
    "\tDoes not check if integer was specified." },

{ {'W', ""}, { bundle(1, &vmd.width), 1, edit_int},
    "set text screen width.\n"
    "\tDoes not check if integer was specified." },

{ {'H', ""}, { bundle(1, &vmd.height), 1, edit_int },
    "set text screen height.\n"
    "\tDoes not check if integer was specified."},
