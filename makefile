CC = g++

CFLAGS = -c -Wall

BLD_FOLDER = build
TEST_FOLDER = test

PROC_BLD_NAME = processor
PROC_BLD_VERSION = 0.1
PROC_BLD_PLATFORM = linux
PROC_BLD_TYPE = dev
PROC_BLD_FORMAT = .out

ASM_BLD_NAME = assembler
ASM_BLD_VERSION = 0.1
ASM_BLD_PLATFORM = linux
ASM_BLD_TYPE = dev
ASM_BLD_FORMAT = .out

DASM_BLD_NAME = disassembler
DASM_BLD_VERSION = 0.1
DASM_BLD_PLATFORM = linux
DASM_BLD_TYPE = dev
DASM_BLD_FORMAT = .out

PROC_BLD_FULL_NAME = $(PROC_BLD_NAME)_v$(PROC_BLD_VERSION)_$(PROC_BLD_TYPE)_$(PROC_BLD_PLATFORM)$(PROC_BLD_FORMAT)
ASM_BLD_FULL_NAME = $(ASM_BLD_NAME)_v$(ASM_BLD_VERSION)_$(ASM_BLD_TYPE)_$(ASM_BLD_PLATFORM)$(ASM_BLD_FORMAT)
DASM_BLD_FULL_NAME = $(DASM_BLD_NAME)_v$(DASM_BLD_VERSION)_$(DASM_BLD_TYPE)_$(DASM_BLD_PLATFORM)$(DASM_BLD_FORMAT)

all: assembler processor

ASSEMBLER_OBJECTS = assembler.o argparser.o logger.o debug.o file_proc.o
assembler: $(ASSEMBLER_OBJECTS)
	mkdir -p $(BLD_FOLDER)
	$(CC) $(ASSEMBLER_OBJECTS) -o $(BLD_FOLDER)/$(ASM_BLD_FULL_NAME)

PROCESSOR_OBJECTS = processor.o argparser.o logger.o debug.o file_proc.o
processor: $(PROCESSOR_OBJECTS)
	mkdir -p $(BLD_FOLDER)
	$(CC) $(PROCESSOR_OBJECTS) -o $(BLD_FOLDER)/$(PROC_BLD_FULL_NAME)

asm:
	cd $(BLD_FOLDER) && exec ./$(ASM_BLD_FULL_NAME) $(ARGS)

run:
	cd $(BLD_FOLDER) && exec ./$(PROC_BLD_FULL_NAME) $(ARGS)

assembler.o:
	$(CC) $(CFLAGS) -c assembler.cpp

processor.o:
	$(CC) $(CFLAGS) -c processor.cpp

file_proc.o:
	$(CC) $(CFLAGS) -c lib/file_proc.cpp

argparser.o:
	$(CC) $(CFLAGS) -c lib/util/argparser.cpp

logger.o:
	$(CC) $(CFLAGS) -c lib/util/dbg/logger.cpp

debug.o:
	$(CC) $(CFLAGS) -c lib/util/dbg/debug.cpp

clean:
	rm -rf *.o

rmbld:
	rm -rf $(BLD_FOLDER)
	rm -rf $(TEST_FOLDER)

rm: clean rmbld