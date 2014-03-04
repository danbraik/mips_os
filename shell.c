#ifdef SHELL

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#include "tests.h"
#include "simu_mips.h"
#include "commands.h"

#define BUFFER_SIZE 64

/*

COMMANDS :

			man
						display a short help

			tree [path]
						display the filesystem like a tree

			ls [path]
						display children of the directory or the 
						information of a file

			mkdir dirpath
			
			touch filepath
						create empty file
			
			wtxt filepath text
						write the content into the file
						create file if it doesnt exist

			whex filepath hex
			
			cat filepath
			
			rm path
			
			pwd
			
			cd dirpath
			
			exec filepath
						#todo
			
			dbg
						display memory debug

			exit


*/

int main(void)
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

		mips_puts("$ ");

		mips_get_line(buffer, BUFFER_SIZE);
		
		length = strlen(buffer);
		buffer[length-1] = '\0';

		if (strlen(buffer) == 0)
			continue;

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
			// mkdir dirpath
			ret_code = mkdir(&allocator, &myfilesystem, arg1);
		} else if (strcmp(cmd, "touch") == 0) {
			// touch filepath
			ret_code = touch(&allocator, 
							 &myfilesystem, 
							 arg1);

		} else if (strcmp(cmd, "wtxt") == 0) {
			// wtxt filepath text
			ret_code = wtxt(&allocator, 
							&myfilesystem, 
							arg1, 
							rest);

		} else if (strcmp(cmd, "whex") == 0) {
			// whex filepath hex
			ret_code = whex(&allocator, 
							&myfilesystem, 
							arg1, 
							rest);

		}else if (strcmp(cmd, "cat") == 0) {
			// cat regularname
			ret_code = cat(fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1));
		} else if (strcmp(cmd, "rm") == 0) {
			// rm path
			ret_code = rm(&allocator, &myfilesystem, arg1);
		} else if (strcmp(cmd, "pwd") == 0) {
			// pwd
			ret_code = pwd(&allocator, &myfilesystem);
		} else if (strcmp(cmd, "cd") == 0) {
			// cd dirname
			ret_code = cd(&myfilesystem, arg1);
		} else if (strcmp(cmd, "exec") == 0) {
			// exec regularname
			ret_code = exec(&allocator, fs_get_file_by_path(myfilesystem.root, 
												myfilesystem.working, 
												arg1));
		} else if (strcmp(cmd, "man") == 0) {
			// man
			mips_puts_nl(" man                ");
			mips_puts_nl(" tree [path]        ");
			mips_puts_nl(" ls [path]          ");
			mips_puts_nl(" mkdir dirpath      ");
			mips_puts_nl(" touch filepath     ");
			mips_puts_nl(" rm path            ");
			mips_puts_nl(" wtxt filepath txt  ");
			mips_puts_nl(" whex filepath hex  ");
			mips_puts_nl(" cat filepath       ");
			mips_puts_nl(" exec filepath      ");
			mips_puts_nl(" pwd                ");
			mips_puts_nl(" cd dirpath         ");
			mips_puts_nl(" dbg                ");
			mips_puts_nl(" exit               ");

			ret_code = CMD_SUCCESS;
		} else if (strcmp(cmd, "exit") == 0) {
			// exit
			running = false;
			ret_code = CMD_SUCCESS;
		} else if (strcmp(cmd, "dbg") == 0) {
			// dbg
			mem_debug(&allocator);
			ret_code = CMD_SUCCESS;
		}else {
			mips_puts_nl("ukwn cmd");
			ret_code = CMD_ERROR;
		}

		if (ret_code == CMD_ERROR)
			mips_puts_nl("error");
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
