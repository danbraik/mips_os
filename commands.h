#ifndef _COMMANDS_H_
#define _COMMANDS_H_


#include "filesystem.h"

#define CMD_SUCCESS 1
#define CMD_ERROR 0

uint8_t tree(fs_file *file);

uint8_t ls(fs_file *file);




#endif