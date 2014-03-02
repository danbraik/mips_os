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

	root->data = mem_alloc(allocator, sizeof(fs_file_data));
	if(root->data == NULL)
		return FS_ERROR;

	root->file_type = FS_TYPE_DIRECTORY;
	if (_fs_cpyname(allocator, root, "/") == FS_ERROR)
		return FS_ERROR;	

	root->data->directory.children = NULL;
	return FS_SUCCESS;
}


static void _fs_mem_free_file(mem_allocator *allocator, fs_file *to_rm, bool rm_data)
{
	if (rm_data)
		mem_free(allocator, to_rm->data, sizeof(fs_file_data));
	mem_free(allocator, to_rm->name, strlen(to_rm->name)+1);
	mem_free(allocator, to_rm, sizeof(fs_file));
}


static void _fs_remove_recursive_files(mem_allocator *allocator,
	fs_file *parent)
{
	fs_list_cell *iterator = parent->data->directory.children;
	fs_list_cell *prev_iterator = NULL;
	while(iterator != NULL) {
		fs_file *to_rm = iterator->file;

		if (to_rm->file_type == FS_TYPE_DIRECTORY)
			_fs_remove_recursive_files(allocator, to_rm);

		_fs_mem_free_file(allocator, to_rm, true);
		
		prev_iterator = iterator;
		iterator = iterator->next;
		mem_free(allocator, prev_iterator, sizeof(fs_list_cell));
	}
}

int8_t fs_delete_root(mem_allocator *allocator, fs_file *root)
{
	if (root == NULL)
		return FS_ERROR;

	_fs_remove_recursive_files(allocator, root);

	mem_free(allocator, root->name, strlen(root->name)+1);
	mem_free(allocator, root->data, sizeof(fs_file_data));

	return FS_SUCCESS;
}



bool fs_is_directory(fs_file* file)
{
	return file != NULL 
		&& file->file_type == FS_TYPE_DIRECTORY;
}

bool fs_is_regular(fs_file* file)
{
	return file != NULL 
		&& file->file_type == FS_TYPE_REGULAR;	
}

static int8_t _fs_push_front_cell(mem_allocator *allocator,
		fs_file *parent, fs_file *file)
{
	fs_list_cell *new_cell = mem_alloc(allocator, sizeof(fs_list_cell));
	if (new_cell == NULL)
		return FS_ERROR;
	new_cell->file = file;
	new_cell->next = parent->data->directory.children;
	parent->data->directory.children = new_cell;
	return FS_SUCCESS;
}

static int8_t _fs_new_child(mem_allocator *allocator,
	uint8_t file_type, const char *name, fs_file **newchild)
{
	fs_file *new_child = mem_alloc(allocator, sizeof(fs_file));
	if (new_child == NULL)
		return FS_ERROR;

	new_child->data = mem_alloc(allocator, sizeof(fs_file_data));
	if(new_child == NULL) {
		mem_free(allocator, new_child, sizeof(fs_file));
		return FS_ERROR;
	}
	
	new_child->file_type = file_type;
	
	// cpy name
	if (_fs_cpyname(allocator, new_child, name) == FS_ERROR) {
		mem_free(allocator, new_child->data, sizeof(fs_file_data));
		mem_free(allocator, new_child, sizeof(fs_file));
		return FS_ERROR;
	}

	// init file data
	if (file_type == FS_TYPE_REGULAR) {
		new_child->data->regular.start = NULL;
		new_child->data->regular.size = 0;
	} else if (file_type == FS_TYPE_DIRECTORY) {
		new_child->data->directory.children = NULL;
	}

	if (newchild != NULL)
		*newchild = new_child;
}

static int8_t _fs_add_child(mem_allocator *allocator, 
						fs_file *parent, uint8_t file_type, 
						const char *name, fs_file **newchild)
{
	if (!fs_is_directory(parent))
		return FS_ERROR;

	// search if child with the same name already exists
	fs_iterator iterator = fs_get_first_child(parent);
	while (iterator != NULL) {
		if (strcmp(
					fs_get_name(fs_get_file_by_iter(iterator)),
					name) == 0)
			return FS_ERROR;
		iterator = fs_get_next_child(iterator);
	}

	fs_file *new_child = NULL;
	if (_fs_new_child(allocator, file_type, name, &new_child) == FS_ERROR)
		return FS_ERROR;

	if (_fs_push_front_cell(allocator, parent, new_child) == FS_ERROR) {
		_fs_mem_free_file(allocator, new_child, true);
		return FS_ERROR;
	}

	if (newchild != NULL)
		*newchild = new_child;

	return FS_SUCCESS;
}


int8_t fs_add_regular(mem_allocator *allocator, fs_file *dir, const char *filename, fs_file **newfile)
{
	return _fs_add_child(allocator, dir, FS_TYPE_REGULAR, filename, newfile);
}

static int8_t _fs_add_phys_link(mem_allocator *allocator,
						fs_file *dir, fs_file *file_to_like,
						const char *linkname)
{

	fs_file *new_child = NULL;
	if (_fs_new_child(allocator, 
						file_to_like->file_type, 
						linkname, &new_child) == FS_ERROR)
		return FS_ERROR;
	
	mem_free(allocator, new_child->data, sizeof(fs_file_data));
	new_child->data = file_to_like->data;

	if (_fs_push_front_cell(allocator, dir, new_child) == FS_ERROR) {
		_fs_mem_free_file(allocator, new_child, false);
		return FS_ERROR;
	}

	return FS_SUCCESS;
}


int8_t fs_add_dir(mem_allocator *allocator, 
		fs_file *dir, 
		const char *dirname, 
		fs_file **newdir)
{
	if (!fs_is_directory(dir))
		return FS_ERROR;

	fs_file *new_dir;

	if (_fs_add_child(allocator, dir, FS_TYPE_DIRECTORY, dirname, &new_dir) == FS_ERROR)
		return FS_ERROR;

	// if (_fs_add_phys_link(allocator,
	// 					new_dir, new_dir,
	// 					".") == FS_ERROR) {
	// 	fs_remove_file(allocator, dir, dirname);
	// 	return FS_ERROR;
	// }

	// if (_fs_add_phys_link(allocator,
	// 					new_dir, dir,
	// 					"..") == FS_ERROR) {
	// 	fs_remove_file(allocator, dir, dirname);
	// 	return FS_ERROR;
	// }

	if (newdir != NULL)
		*newdir = new_dir;

	return FS_SUCCESS;
}




int8_t fs_remove_file(mem_allocator *allocator, 
				fs_file *parent, 
				const char *name)
{
	if (!fs_is_directory(parent))
		return FS_ERROR;

	fs_list_cell *iterator = parent->data->directory.children;
	fs_list_cell **prev_iterator = &(parent->data->directory.children);

	while (iterator != NULL) {
		fs_file *to_rm = iterator->file;

		if (strcmp(name, to_rm->name) == 0) {
			if (to_rm->file_type == FS_TYPE_DIRECTORY)
				_fs_remove_recursive_files(allocator, to_rm);

			_fs_mem_free_file(allocator, to_rm, true);

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
	return dir->data->directory.children;
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


int8_t fs_write_regular(mem_allocator *allocator, 
						fs_file *file, 
						const uint8_t *data, 
						uint32_t size)
{
	if (!fs_is_regular(file))
		return FS_ERROR;

	uint8_t *start = (uint8_t*) mem_alloc(allocator, size);

	if (start == NULL)
		return FS_ERROR;

	memcpy((void*)start, (void*)data, size);

	file->data->regular.start = start;
	file->data->regular.size = size;

	return FS_SUCCESS;
}


int8_t fs_get_memdata(fs_file *file, uint8_t **data, uint32_t *size)
{
	if (!fs_is_regular(file))
		return FS_ERROR;

	*data = file->data->regular.start;
	*size = file->data->regular.size;

	return FS_SUCCESS;
}


static fs_file* _fs_get_file_by_path_rec()
{

}


fs_file* fs_get_file_by_path(fs_file *root, 
	fs_file *working, 
	const char *filepath)
{
	if(!fs_is_directory(root) || !fs_is_directory(working))
		return NULL;



}