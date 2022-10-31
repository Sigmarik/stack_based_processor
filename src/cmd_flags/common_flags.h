/**
 * @file common_flags.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief Flags present in all programs.
 * @version 0.1
 * @date 2022-10-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// TODO: read assembler_flags.h

// TODO: also, I would change extension of this special "header"
//       files, so it's clear they can't be used as usual.

// E.g. ".inc" is a common one.

{ {'O', "owl"}, { {}, 0, print_owl },
    "print 10 owls to the screen." },

{ {'I', ""}, { bundle(1, &log_threshold), 1, edit_int },
    "set log threshold to the specified number.\n"
    "\tDoes not check if integer was specified." },
