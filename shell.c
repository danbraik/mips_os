#ifdef SHELL

#include <stdbool.h>

#include "tests.h"
#include "simu_mips.h"
#include "commands.h"

#define BUFFER_SIZE 64

/*

COMMANDS :




*/

int main(int argc, char const *argv[])
{
	mem_allocator allocator;
	uint8_t *memory = malloc(MEM_SIZE);
	mem_init(&allocator, memory, MEM_SIZE);

	fs_file *root;
	if(fs_new_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_new_root");

	cmd_filesystem myfilesystem;
	myfilesystem.root = myfilesystem.working = root;

	//mem_debug(&allocator);

	// *******************

	char buffer[BUFFER_SIZE];
	bool running = true;
	uint32_t length = 0;

	char *cmd, *arg1, *rest;


	while (running) {

		mips_get_line(buffer, BUFFER_SIZE);
		
		length = strnlen(buffer, BUFFER_SIZE);
		buffer[length-1] = '\0';

		mips_puts_nl(buffer);

		// extract CMD
		// [
		uint8_t index = 0;

		while(buffer[index] == ' ' && index < length)
			index++;

		cmd = buffer + index;
		
		while(buffer[index] != ' ' && index < length)
			index++;

		buffer[index] = '\0';
		// ]

		// extract arg1
		// [
		index++;
		while(buffer[index] == ' ' && index < length)
			index++;

		arg1 = buffer + index;

		while(buffer[index] != ' ' && index < length)
			index++;

		buffer[index] = '\0';
		// ]

		// extract rest
		// [
		rest = buffer + index + 1;
		// ]

		mips_puts("CMD = ");
		mips_puts_nl(cmd);
		
		uint8_t ret_code = 0;

		if (strcmp(cmd, "tree") == 0) {
			// tree [filename]
			fs_file *file = (strlen(arg1) == 0) ? myfilesystem.working :
							fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1);
			ret_code = tree(file);
		} else if (strcmp(cmd, "ls") == 0) {
			// ls [filename]
			fs_file *file = (strlen(arg1) == 0) ? myfilesystem.working :
							fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1);
			ret_code = ls(file);
		} else if (strcmp(cmd, "mkdir") == 0) {
			// mkdir dirname
			ret_code = mkdir(&allocator, myfilesystem.working, arg1);
		} else if (strcmp(cmd, "touch") == 0) {
			// touch regularname
			ret_code = touch(&allocator, myfilesystem.working, arg1);
		} else if (strcmp(cmd, "write") == 0) {
			// write regularname content
			ret_code = write(&allocator, fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1), rest);
		} else if (strcmp(cmd, "cat") == 0) {
			// cat regularname
			ret_code = cat(fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1));
		} else if (strcmp(cmd, "rm") == 0) {
			// rm filename
			ret_code = rm(&allocator, myfilesystem.working, arg1);
		} else if (strcmp(cmd, "pwd") == 0) {
			// pwd
			ret_code = pwd(&myfilesystem);
		} else if (strcmp(cmd, "cd") == 0) {
			// cd dirname
			ret_code = cd(&myfilesystem, arg1);
		} else if (strcmp(cmd, "exec") == 0) {
			// exec regularname
			ret_code = exec(&allocator, fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1));
		} else if (strcmp(cmd, "exit") == 0) {
			// exit
			running = false;
			ret_code = CMD_SUCCESS;
		} else {
			mips_puts_nl("> ukwn cmd");
			ret_code = CMD_ERROR;
		}

		if (ret_code == CMD_ERROR)
			mips_puts_nl("> error");
	}

	//******
	//mem_debug(&allocator);
	// clean
	if (fs_delete_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_delete_root");
	mem_debug(&allocator);
	// clean test
	free(memory);

	return 0;
}


#endif
