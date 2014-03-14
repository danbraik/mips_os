# ******** MIPS

BINS = os

# Project to compile (use a preprocessor definition)
#PROJECT_TO_COMPILE = TEST_ALLOC
#PROJECT_TO_COMPILE = TEST_FILESYSTEM
#PROJECT_TO_COMPILE = TEST_COMMANDS
PROJECT_TO_COMPILE = SHELL

TRACES = # -D TRACE_ALLOC -D PRINT_STACK


AS = mips-elf-gcc
ASFLAGS =  -c -Os
CC = mips-elf-gcc
CFLAGS = -Wall -Wextra -g -std=c99 -D $(PROJECT_TO_COMPILE)
LD = mips-elf-gcc
LDFLAGS = -T cep.ld


OBJS=shell.o alloc.o commands.o filesystem.o simu_mips.o

.PHONY: all

all: $(BINS)

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<



.PHONY: clean
clean:
	$(RM) $(BINS) $(OBJS)



os : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run
run:
	qemu-system-mips -M mipscep -nographic --kernel os
