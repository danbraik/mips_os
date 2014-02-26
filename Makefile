# --------
# Makefile
# --------

# Project to compile (use a preprocessor definition)
PROJECT_TO_COMPILE = TEST_FILESYSTEM

# Executable name
EXE = prog.exe

# Directories
SRC_DIR = .
OBJ_DIR = obj

# Compiler
CC = gcc
# Compiler options
CFLAGS = -D $(PROJECT_TO_COMPILE) -std=c99 -W -Wall -Wextra -g # -Werror
# Linker options
LDFLAGS = 

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
