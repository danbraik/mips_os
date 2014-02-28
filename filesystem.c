#include <stdlib.h>
#include <string.h>
#include "filesystem.h"



static int8_t _fs_cpyname(mem_allocator *allocator, fs_file *file, const char *name)
{
	uint32_t len = strlen(name) + 1;
	if ((file->name = mem_alloc(allocator, len)) == NULL)
		return FS_ERROR;
	strncpy(file->name, name, len);
	return FS_SUCCESS;
}


int8_t fs_new_root(mem_allocator *allocator, 
					fs_file *root)
{
	if (root == NULL)
		return FS_ERROR;

	root->file_type = FS_TYPE_DIRECTORY;
	if (_fs_cpyname(allocator, root, "/") == FS_ERROR)
		return FS_ERROR;	

	root->data.directory.children = NULL;
	return FS_SUCCESS;
}

static void _fs_mem_free_file(mem_allocator *allocator, fs_file *to_rm)
{
	mem_free(allocator, to_rm->name, strlen(to_rm->name)+1);
	mem_free(allocator, to_rm, sizeof(fs_file));
}


static void _fs_remove_recursive_files(mem_allocator *allocator,
	fs_file *parent)
{
	fs_list_cell *iterator = parent->data.directory.children;
	fs_list_cell *prev_iterator = NULL;
	while(iterator != NULL) {
		fs_file *to_rm = iterator->file;

		if (to_rm->file_type == FS_TYPE_DIRECTORY)
			_fs_remove_recursive_files(allocator, to_rm);

		_fs_mem_free_file(allocator, to_rm);
		
		prev_iterator = iterator;
		iterator = iterator->next;
		mem_free(allocator, prev_iterator, sizeof(fs_list_cell));
	}
}

int8_t fs_delete_root(mem_allocator *allocator, fs_file *root)
{
	if (root == NULL)
		return FS_ERROR;

	mem_free(allocator, root->name, strlen(root->name)+1);

	_fs_remove_recursive_files(allocator, root);

	return FS_SUCCESS;
}



static int8_t _fs_add_child(mem_allocator *allocator, 
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


int8_t fs_add_regular(mem_allocator *allocator, fs_file *dir, const char *filename, fs_file **newfile)
{
	return _fs_add_child(allocator, dir, FS_TYPE_REGULAR, filename, newfile);
}

int8_t fs_add_dir(mem_allocator *allocator, fs_file *dir, const char *dirname, fs_file **newdir)
{
	return _fs_add_child(allocator, dir, FS_TYPE_DIRECTORY, dirname, newdir);
}




int8_t fs_remove_file(mem_allocator *allocator, 
				fs_file *parent, 
				const char *name)
{
	if (parent == NULL || parent->file_type != FS_TYPE_DIRECTORY)
		return FS_ERROR;

	fs_list_cell *iterator = parent->data.directory.children;
	fs_list_cell **prev_iterator = &(parent->data.directory.children);

	while (iterator != NULL) {
		fs_file *to_rm = iterator->file;

		if (strcmp(name, to_rm->name) == 0) {
			if (to_rm->file_type == FS_TYPE_DIRECTORY)
				_fs_remove_recursive_files(allocator, to_rm);

			_fs_mem_free_file(allocator, to_rm);

			// re-link previous cell
			*prev_iterator = iterator->next;

			mem_free(allocator, iterator, sizeof(fs_list_cell));

			return FS_SUCCESS;
		}

		prev_iterator = &(iterator->next);
		iterator = iterator->next;
	}

	// no file found
	return FS_ERROR;
}



int8_t fs_get_file(fs_file *root, fs_file *working, 
	const char *filepath)
{
	return FS_SUCCESS;
}



bool fs_is_directory(fs_file* file)
{
	return file != NULL 
		&& file->file_type == FS_TYPE_DIRECTORY;
}

const char * fs_get_name(fs_file *file)
{
	if (file == NULL)
		return NULL;
	return file->name;
}


fs_iterator fs_get_first_child(fs_file *dir)
{
	if (!fs_is_directory(dir))
		return NULL;
	return dir->data.directory.children;
}


fs_iterator fs_get_next_child(fs_iterator iterator)
{
	if (iterator == NULL)
		return NULL;
	return iterator->next;
}


fs_file* fs_get_file_by_iter(fs_iterator iterator)
{
	if (iterator == NULL)
		return NULL;
	return iterator->file;
}