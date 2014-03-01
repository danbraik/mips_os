#ifdef TEST_COMMANDS

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

	fs_file root;
	if(fs_new_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_init_directory");
	if (fs_add_regular(&allocator, &root, "monFichierapappapapapapapapapapapapaas", NULL) == FS_ERROR)
		puts("Error pour fs_add_file");
	fs_file *home, *user, *file1;
	fs_add_dir(&allocator, &root, "home", &home);
	fs_add_dir(&allocator, home, "user", &user);
	fs_add_regular(&allocator, user, "file1", &file1);
	fs_add_regular(&allocator, user, "file2", NULL);
	// begin tests

	puts("* Test LS");
	treat(ls(&root));
	puts("* End");

	puts("* Test TREE");
	treat(tree(&root));
	puts("* End");

	puts("* Test MKDIR");
	treat(mkdir(&allocator, &root, "dossier"));
	tree(&root);
	puts("* End");

	puts("* Test TOUCH");
	treat(touch(&allocator, &root, "fileTouched"));
	tree(&root);
	puts("* create twice fileTouched. Assert error.");
	treat(touch(&allocator, &root, "fileTouched"));
	puts("* End");

	puts("* Test WRITE");
	treat(write(&allocator, file1, "content_data_content_data_content_data_content_data", 52));
	puts("* End");

	puts("* Test CAT");
	treat(cat(file1));
	puts("* End");

	puts("* Test RM");
	puts("* rm /dossier /home/user/file2");
	treat(rm(&allocator, user, "file2"));
	treat(rm(&allocator, &root, "dossier"));
	tree(&root);
	puts("* End");




	if (fs_delete_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_delete_root");

	// clean test
	free(memory);

	return 0;
}


#endif