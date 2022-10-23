CC = g++

CFLAGS = -I./ -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++\
-Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations\
-Wcast-align -Wchar-subscripts -Wconditionally-supported\
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral\
-Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op\
-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith\
-Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo\
-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn\
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default\
-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast\
-Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers\
-Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector\
-fcheck-new\
-fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging\
-fno-omit-frame-pointer -fPIE -fsanitize=address,bool,${strip \
}bounds,enum,float-cast-overflow,float-divide-by-zero,${strip \
}integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,${strip \
}returns-nonnull-attribute,shift,signed-integer-overflow,undefined,${strip \
}unreachable,vla-bound,vptr\
-pie -Wlarger-than=65535 -Wstack-usage=8192

BLD_FOLDER = build
TEST_FOLDER = test
ASSET_FOLDER = assets

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

all: asset assembler processor disassembler

ASSEMBLER_OBJECTS = assembler.o alloc_tracker.o argworks.o common.o argparser.o logger.o debug.o file_proc.o
assembler: $(ASSEMBLER_OBJECTS)
	mkdir -p $(BLD_FOLDER)
	$(CC) $(ASSEMBLER_OBJECTS) $(CFLAGS) -o $(BLD_FOLDER)/$(ASM_BLD_FULL_NAME)

PROCESSOR_OBJECTS = processor.o alloc_tracker.o argworks.o common.o argparser.o logger.o debug.o file_proc.o
processor: $(PROCESSOR_OBJECTS)
	mkdir -p $(BLD_FOLDER)
	$(CC) $(PROCESSOR_OBJECTS) $(CFLAGS) -o $(BLD_FOLDER)/$(PROC_BLD_FULL_NAME)

DISASSEMBLER_OBJECTS = disasm.o alloc_tracker.o argworks.o common.o argparser.o logger.o debug.o file_proc.o
disassembler: $(DISASSEMBLER_OBJECTS)
	mkdir -p $(BLD_FOLDER)
	$(CC) $(DISASSEMBLER_OBJECTS) $(CFLAGS) -o $(BLD_FOLDER)/$(DASM_BLD_FULL_NAME)

asset:
	mkdir -p $(BLD_FOLDER)
	cp -r $(ASSET_FOLDER)/. $(BLD_FOLDER)

asm:
	cd $(BLD_FOLDER) && exec ./$(ASM_BLD_FULL_NAME) $(ARGS)

run:
	cd $(BLD_FOLDER) && exec ./$(PROC_BLD_FULL_NAME) $(ARGS)

disasm:
	cd $(BLD_FOLDER) && exec ./$(DASM_BLD_FULL_NAME) $(ARGS)

assembler.o:
	$(CC) $(CFLAGS) -c src/assembler.cpp

processor.o:
	$(CC) $(CFLAGS) -c src/processor.cpp

disasm.o:
	$(CC) $(CFLAGS) -c src/disasm.cpp

argworks.o:
	$(CC) $(CFLAGS) -c src/utils/argworks.cpp

common.o:
	$(CC) $(CFLAGS) -c src/utils/common.cpp

alloc_tracker.o:
	$(CC) $(CFLAGS) -c lib/alloc_tracker/alloc_tracker.cpp

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