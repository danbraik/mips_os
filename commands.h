#ifndef _COMMANDS_H_
#define _COMMANDS_H_


#include "filesystem.h"

#define CMD_SUCCESS 1
#define CMD_ERROR 0

typedef struct {
	// have to be both directories
	fs_file *root;
	fs_file *working;
} cmd_filesystem;

uint8_t tree(fs_file *file);
uint8_t ls(fs_file *file);
uint8_t mkdir(mem_allocator *allocator, cmd_filesystem *filesystem, char *dirpath);
uint8_t touch(mem_allocator *allocator, cmd_filesystem *filesystem, char *filepath);
uint8_t wtxt(mem_allocator *allocator, cmd_filesystem *filesystem, char *filepath, const char *data_txt);
uint8_t whex(mem_allocator *allocator, cmd_filesystem *filesystem, char *filepath, const char *data_hex);
uint8_t cat(fs_file *file);
uint8_t rm(mem_allocator *allocator, cmd_filesystem *filesystem, char *path);
uint8_t pwd(mem_allocator *allocator, cmd_filesystem *filesystem);
uint8_t cd(cmd_filesystem *filesystem, char *filepath);


uint8_t exec(mem_allocator *allocator, fs_file *file);


#endif
