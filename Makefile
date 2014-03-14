# ********* global

# Project to compile (use a preprocessor definition)
#PROJECT_TO_COMPILE = TEST_ALLOC
#PROJECT_TO_COMPILE = TEST_FILESYSTEM
#PROJECT_TO_COMPILE = TEST_COMMANDS
PROJECT_TO_COMPILE = SHELL

TRACES = # -D TRACE_ALLOC -D PRINT_STACK


# ******** MIPS

AS = mips-elf-gcc
ASFLAGS = -c -Os
CC = mips-elf-gcc
CFLAGS = -Wall -Wextra -g -std=c99 -D $(PROJECT_TO_COMPILE)
LD = mips-elf-gcc
LDFLAGS = -T cep.ld

aaOBJS=shell.o alloc.o commands.o filesystem.o simu_mips.o

.PHONY: all

mips: os_mips

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

.PHONY: clean
clean_mips:
	$(RM) os_mips $(aaOBJS)

os_mips : $(aaOBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.PHONY: run_mips
run_mips: os_mips
	qemu-system-mips -M mipscep -nographic --kernel os_mips




# ************ PC 




# Executable name
EXE = os_pc

# Directories
SRC_DIR = .
OBJ_DIR = obj

# Compiler
mmCC = gcc
# Compiler options
mmCFLAGS = -D $(PROJECT_TO_COMPILE) $(TRACES) -std=c99 -g -W -Wall -Wextra # -Os # -Werror 
# Linker options
mmLDFLAGS = -rdynamic

# ----

SRC = $(wildcard $(SRC_DIR)/[^N]*.c)
mmOBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

debug:
	echo $(SRC)

pc: $(EXE)

$(EXE): $(mmOBJS) 
	$(mmCC) $(mmLDFLAGS) -o $@ $^

# Create obj directory if needed
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(OBJ_DIR)
	$(mmCC) $(mmCFLAGS) -o $@ -c $<

.PHONY: clean
clean_pc:
	@rm $(mmOBJS)

.PHONY: cleaner
cleaner: clean
	# The || avoid errors if OBJ_DIR does not exist
	@rmdir "$(OBJ_DIR)" || echo ; \
	rm "$(EXE)"

.PHONY: run_pc
run_pc: pc
	./pc


# ****

.PHONY:clean
clean: clean_pc clean_mips

