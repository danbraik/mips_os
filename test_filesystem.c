#ifdef TEST_FILESYSTEM
#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

#define MEM_SIZE 1024


void tree(fs_file *file, int deep)
{
	if (file == NULL)
		return;

	for(int i=0;i<deep;++i)
		printf("\t");
	if (file->file_type == FS_TYPE_REGULAR) {
		puts(file->name);
	} else if (file->file_type == FS_TYPE_DIRECTORY) {
		printf("%s/\n", file->name);

		fs_list_cell *it = file->data.directory.children;
		while (it != NULL) {
			tree(it->file, deep+1);
			it = it->next;
		}
	}	
}


int main(int argc, char const *argv[])
{
	mem_allocator allocator;
	uint8_t *memory = malloc(MEM_SIZE);
	mem_init(&allocator, memory, MEM_SIZE);

	mem_debug(&allocator);

	fs_file root;
	if(fs_new_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_init_directory");

	mem_debug(&allocator);

	if (fs_add_regular(&allocator, &root, "monFichier", NULL) == FS_ERROR)
		puts("Error pour fs_add_file");

	fs_file *home, *user;
	fs_add_dir(&allocator, &root, "home", &home);
	fs_add_dir(&allocator, &root, "usr", NULL);
	fs_add_dir(&allocator, &root, "tmp", NULL);


	fs_add_dir(&allocator, home, "emmett", &user);
	fs_add_regular(&allocator, user, "Lego Movie.avi", NULL);
	fs_add_regular(&allocator, user, "algo.c", NULL);
	
	tree(&root, 0);
	mem_debug(&allocator);

	// if (fs_remove_file(&allocator, user, "algo.c") == FS_ERROR)
	// 	puts("Error pour fs_remove_file");

	// tree(&root, 0);
	// mem_debug(&allocator);

	// if (fs_remove_file(&allocator, &root, "home") == FS_ERROR)
	// 	puts("Error pour fs_remove_file");
	// tree(&root, 0);

	// if (fs_remove_file(&allocator, &root, "usr") == FS_ERROR)
	// 	puts("Error pour fs_remove_file");
	// if (fs_remove_file(&allocator, &root, "tmp") == FS_ERROR)
	// 	puts("Error pour fs_remove_file");
	// if (fs_remove_file(&allocator, &root, "monFichier") == FS_ERROR)
	// 	puts("Error pour fs_remove_file");
	// tree(&root, 0);

	// mem_debug(&allocator);

	if (fs_delete_root(&allocator, &root) == FS_ERROR)
		puts("Error pour fs_delete_root");

	mem_debug(&allocator);

	return 0;
}
#endif