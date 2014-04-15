# ********* global

# Project to compile (use a preprocessor definition)
#PROJECT_TO_COMPILE = TEST_ALLOC
#PROJECT_TO_COMPILE = TEST_FILESYSTEM
#PROJECT_TO_COMPILE = TEST_COMMANDS
PROJECT_TO_COMPILE = SHELL

TRACES = # -D TRACE_ALLOC -D PRINT_STACK


MIPS_TOOLS_BIN=/opt/mips-tools-cep/bin


# ******** MIPS

# MIPS | QEMU
ENV = QEMU
QEMU_GRAPHIC = -nographic

AS = $(MIPS_TOOLS_BIN)/mips-elf-gcc
ASFLAGS = -c -Os
CC = $(MIPS_TOOLS_BIN)/mips-elf-gcc
CFLAGS = -Wall -Wextra -std=c99 -D $(PROJECT_TO_COMPILE) -D $(ENV)
LD = $(MIPS_TOOLS_BIN)/mips-elf-gcc
LDFLAGS = -T cep.ld

aaOBJS=shell.o alloc.o commands.o filesystem.o simu_mips.o

.PHONY: all
all: mips pc

.PHONY: mips
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
	"$(MIPS_TOOLS_BIN)/qemu-system-mips" -M mipscep $(QEMU_GRAPHIC) -show-cursor --kernel os_mips



# ************ PC 




# Executable name
EXE = os_pc

# Directories
SRC_DIR = .
OBJ_DIR = obj

# Compiler
mmCC = gcc
# Compiler options
mmCDEBUG = -g
mmCOPTIMIZE = # -Os
mmCFLAGS = -D $(PROJECT_TO_COMPILE) $(TRACES) $(LIBC) -D PC -std=c99 $(CDEBUG) $(COPTIMIZE) -W -Wall -Wextra # -Werror 
# Linker options
mmLDFLAGS = -rdynamic

# ----

SRC = $(wildcard $(SRC_DIR)/[^N]*.c)
mmOBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: debug
debug:
	echo $(SRC) ; \
	echo "$$PATH"

.PHONY: pc
pc: $(EXE)

$(EXE): $(mmOBJS) 
	$(mmCC) $(mmLDFLAGS) -o $@ $^

# Create obj directory if needed
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(OBJ_DIR)
	$(mmCC) $(mmCFLAGS) -o $@ -c $<

.PHONY: clean_pc
clean_pc:
	@rm $(mmOBJS)


.PHONY: run_pc
run_pc: $(EXE)
	./$(EXE)


# ****

.PHONY:clean
clean: clean_pc clean_mips

