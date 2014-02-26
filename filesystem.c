#include <stdlib.h>
#include <string.h>
#include "filesystem.h"



static int _fs_cpyname(mem_allocator *allocator, fs_file *file, const char *name)
{
	uint32_t len = strlen(name) + 1;
	if ((file->name = mem_alloc(allocator, len)) == NULL)
		return FS_ERROR;
	strncpy(file->name, name, len);
	return FS_SUCCESS;
}


int fs_init_directory(mem_allocator *allocator, 
					fs_file *dir, 
					const char *dirname)
{
	if (dir == NULL)
		return FS_ERROR;

	dir->file_type = FS_TYPE_DIRECTORY;
	if (_fs_cpyname(allocator, dir, dirname) == FS_ERROR)
		return FS_ERROR;	

	dir->data.directory.children = NULL;
	return FS_SUCCESS;
}



static int _fs_add_child(mem_allocator *allocator, 
						fs_file *parent, uint8_t file_type, const char *name, fs_file **newchild)
{
	if (parent == NULL || parent->file_type != FS_TYPE_DIRECTORY)
		return FS_ERROR;

	fs_file *new_child = mem_alloc(allocator, sizeof(fs_file));
	if (new_child == NULL)
		return FS_ERROR;
	// init
	new_child->file_type = file_type;
	if (_fs_cpyname(allocator, new_child, name) == FS_ERROR)
		return FS_ERROR;
	if (file_type == FS_TYPE_REGULAR) {
		new_child->data.regular.start = NULL;
		new_child->data.regular.size = 0;
	} else if (file_type == FS_TYPE_DIRECTORY) {
		new_child->data.directory.children = NULL;
	}

	// push front the new file
	fs_list_cell *new_cell = mem_alloc(allocator, sizeof(fs_list_cell));
	new_cell->file = new_child;
	new_cell->next = parent->data.directory.children;
	parent->data.directory.children = new_cell;

	if (newchild != NULL)
		*newchild = new_child;

	return FS_SUCCESS;
}


int fs_add_file(mem_allocator *allocator, fs_file *dir, const char *filename, fs_file **newfile)
{
	return _fs_add_child(allocator, dir, FS_TYPE_REGULAR, filename, newfile);
}

int fs_add_dir(mem_allocator *allocator, fs_file *dir, const char *dirname, fs_file **newdir)
{
	return _fs_add_child(allocator, dir, FS_TYPE_DIRECTORY, dirname, newdir);
}



