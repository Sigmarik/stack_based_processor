/**
 * @file config.h
 * @author Kudryashov Ilya (kudriashov.it@phystech.edu)
 * @brief List of constants used in main() files.
 * @version 0.1
 * @date 2022-10-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//* ASSEMBLER program
#ifdef ASSEMBLER

    // Max byte length of single command. 
    const size_t MAX_CMD_BITE_LENGTH = 128;

    // Default name of the output binary file
    #define DEFAULT_BINARY_OUT_NAME "a.bin"
    // Default name of the listing file
    #define DEFAULT_LISTING_NAME "listing.txt"

#endif

//* DISASSEMBLER program
#ifdef DISASM

    // Default name of the disassembler output file.
    const char* DEFAULT_OUTPUT_NAME = "program.txt";

#endif

//* Processor program
#ifdef PROCESSOR

    // Type of BOTH stacks used in processor
    typedef long long stack_content_t;
    stack_content_t STACK_CONTENT_POISON = (stack_content_t) 0xDEADBABEC0FEBEEF;

    static const size_t STACK_START_SIZE = 1024;
    static const size_t ADDR_STACK_START_SIZE = 16;

    // Characters sorted by their brightness
    static const char PIX_STATES[] = R"( .'`^",:;Il!i><~+_-?][}{1)(|\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$)";

#endif

#endif