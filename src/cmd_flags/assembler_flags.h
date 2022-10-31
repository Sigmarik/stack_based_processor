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

// TODO: including headers in the middle of files isn't
//       that common or "pretty", avoid if you can

// It's hard to process for language servers, completely
// losses context (e.g. what is gen_listing and where to find it)

// You'll quickly end up greping over your source code and mixing
// and matching this things to finally get them to work
{ {'L', ""},    { bundle(1, &gen_listing),      1, edit_int },
    "set if program should generate listing or not.\n"
    "\tDoes not check if integer was specified." },

{ {'S', ""},    { bundle(1, &labels.max_size),  1, edit_int },
    "set maximum number of labels.\n"
    "\tDoes not check if integer was specified." },

{ {'F', ""},    { bundle(1, &out_size),         1, edit_int },
    "set maximum output file size.\n"
    "\tDoes not check if integer was specified." },
