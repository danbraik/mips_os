#ifdef TEST_COMMANDS
#include <stdio.h>

#include "tests.h"

#include "commands.h"

void treat(uint8_t return_code)
{
	if (return_code == CMD_ERROR)
		puts("/!\\ Error /!\\");
}

int main(int argc, char const *argv[])
{
	mem_allocator allocator;
	uint8_t *memory = malloc(MEM_SIZE);
	mem_init(&allocator, memory, MEM_SIZE);

	fs_file *root;
	if(fs_new_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_new_root");

	if (fs_add_regular(&allocator, root, "monFichierapappapapapapapapapapapapaas", NULL) == FS_ERROR)
		puts("Error pour fs_add_file");
	fs_file *home, *user, *file1;
	fs_add_dir(&allocator, root, "home", &home);
	fs_add_dir(&allocator, home, "user", &user);
	fs_add_regular(&allocator, user, "file1", &file1);
	fs_add_regular(&allocator, user, "file2", NULL);

	cmd_filesystem myfilesystem;
	myfilesystem.root = myfilesystem.working = root;
	// begin tests

	puts("* Test LS");
	treat(ls(root));
	puts("* End");

	puts("* Test TREE");
	treat(tree(root));
	puts("* End");

	puts("* Test MKDIR");
	treat(mkdir(&allocator, root, "dossier"));
	tree(root);
	puts("* End");

	puts("* Test TOUCH");
	treat(touch(&allocator, root, "fileTouched"));
	tree(root);
	puts("* create twice fileTouched. Assert error.");
	treat(touch(&allocator, root, "fileTouched"));
	puts("* End");

	puts("* Test WRITE");
	treat(write(&allocator, file1, (const uint8_t*)"content_data_content_data_content_data_content_data"));
	puts("* End");

	puts("* Test CAT");
	treat(cat(file1));
	puts("* End");

	puts("* Test RM");
	tree(root);
	puts("* rm /home/user/file1");
	treat(rm(&allocator, file1));
	tree(root);
	puts("* End");

	puts("* Test PWD");
	myfilesystem.working = user;
	treat(pwd(&myfilesystem));
	puts("* End");

	puts("* Test CD");
	char filepath[] = "/home/.././home/./././user/";
	printf("in : %s\n", filepath);
	treat(cd(&myfilesystem, filepath));
	pwd(&myfilesystem);
	puts("* End");

	if (fs_delete_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_delete_root");

	// clean test
	mem_debug(&allocator);
	free(memory);

	return 0;
}


#endif