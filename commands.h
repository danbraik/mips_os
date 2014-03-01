#ifndef _COMMANDS_H_
#define _COMMANDS_H_


#include "filesystem.h"

#define CMD_SUCCESS 1
#define CMD_ERROR 0


uint8_t tree(fs_file *file);
uint8_t ls(fs_file *file);
uint8_t mkdir(mem_allocator *allocator, fs_file *parent, const char *dirname);
uint8_t touch(mem_allocator *allocator, fs_file *parent, const char *filename);
uint8_t write(mem_allocator *allocator, fs_file *file, const uint8_t *data, uint32_t size);
uint8_t cat(fs_file *file);
uint8_t rm(mem_allocator *allocator, fs_file *parent, const char *name);
uint8_t exec(mem_allocator *allocator, fs_file *file);


#endif