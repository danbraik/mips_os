#ifdef TEST_COMMANDS

#include "tests.h"

#include "commands.h"

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
	fs_file *home, *user;
	fs_add_dir(&allocator, &root, "home", &home);
	fs_add_dir(&allocator, home, "user", &user);
	fs_add_regular(&allocator, user, "file1", NULL);
	fs_add_regular(&allocator, user, "file2", NULL);

	puts("* Test LS");
	ls(&root);
	puts("* End");

	puts("* Test TREE");
	tree(&root);
	puts("* End");


	if (fs_delete_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_delete_root");

	// clean test
	free(memory);

	return 0;
}


#endif