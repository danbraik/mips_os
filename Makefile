# --------
# Makefile
# --------

# Project to compile (use a preprocessor definition)
#PROJECT_TO_COMPILE = TEST_ALLOC
#PROJECT_TO_COMPILE = TEST_FILESYSTEM
#PROJECT_TO_COMPILE = TEST_COMMANDS
PROJECT_TO_COMPILE = SHELL

TRACES = # -D TRACE_ALLOC -D PRINT_STACK

LIBC = -D USE_REAL_LIBC


# Executable name
EXE = prog.exe

# Directories
SRC_DIR = .
OBJ_DIR = obj

# Compiler
CC = gcc
# Compiler options
CDEBUG = -g
COPTIMIZE = # -Os
CFLAGS = -D $(PROJECT_TO_COMPILE) $(TRACES) $(LIBC) -std=c99 $(CDEBUG) $(COPTIMIZE) -W -Wall -Wextra # -Werror 
# Linker options
LDFLAGS = -rdynamic

# ----

SRC = $(wildcard $(SRC_DIR)/[^N]*.c)
OBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


all : $(EXE)

.PHONY: tests
tests :
	cd tests ; make

$(EXE) : $(OBJS) 
	$(CC) $(LDFLAGS) -o $@ $^

# Create obj directory if needed
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean :
	@rm $(OBJS)

.PHONY: cleaner
cleaner : clean
	# The || avoid errors if OBJ_DIR does not exist
	@rmdir "$(OBJ_DIR)" || echo ; \
	rm "$(EXE)"
	
$(PPM_DIR)/tests/%.ppm : $(CONF_DIR)/%.txt
	./$(EXE) $< $@
