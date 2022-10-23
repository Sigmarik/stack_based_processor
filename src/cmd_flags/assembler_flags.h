/**
 * @file assembler_flags.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief List of command line arguments for assembler.
 * @version 0.1
 * @date 2022-10-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "common_flags.h"

{ {'L', ""},    { bundle(1, &gen_listing),      1, edit_int },
    "set if program should generate listing or not.\n"
    "\tDoes not check if integer was specified." },

{ {'S', ""},    { bundle(1, &labels.max_size),  1, edit_int },
    "set maximum number of labels.\n"
    "\tDoes not check if integer was specified." },

{ {'F', ""},    { bundle(1, &out_size),         1, edit_int },
    "set maximum output file size.\n"
    "\tDoes not check if integer was specified." },