#include <stdlib.h>
#include <string.h>
#include "filesystem.h"




STATIC int8_t _fs_cpyname(mem_allocator *allocator, fs_file *file, const char *name)
{
	uint32_t len = strlen(name) + 1;
	if ((file->name = mem_alloc(allocator, len)) == NULL)
		return FS_ERROR;
	strncpy(file->name, name, len);
	return FS_SUCCESS;
}


int8_t fs_new_root(mem_allocator *allocator, 
					fs_file **root)
{
	if (root == NULL)
		return FS_ERROR;

	*root = mem_alloc(allocator, sizeof(fs_file));
	if (*root == NULL)
		return FS_ERROR;

	(*root)->file_type = FS_TYPE_DIRECTORY;
	if (_fs_cpyname(allocator, *root, "/") == FS_ERROR)
		return FS_ERROR;
	(*root)->parent = NULL;

	(*root)->data.directory.children = NULL;
	return FS_SUCCESS;
}


STATIC void _fs_delete_file(mem_allocator *allocator, fs_file *to_rm)
{
	if (fs_is_regular(to_rm) && to_rm->data.regular.size > 0) {
		mem_free(allocator, to_rm->data.regular.start, to_rm->data.regular.size);
	}
	mem_free(allocator, to_rm->name, strlen(to_rm->name)+1);
	mem_free(allocator, to_rm, sizeof(fs_file));
}


STATIC void _fs_remove_recursive_files(mem_allocator *allocator,
	fs_file *parent)
{
	fs_iterator iterator = parent->data.directory.children;
	fs_iterator prev_iterator = NULL;
	while(iterator != NULL) {
		fs_file *to_rm = iterator->file;

		if (to_rm->file_type == FS_TYPE_DIRECTORY)
			_fs_remove_recursive_files(allocator, to_rm);

		_fs_delete_file(allocator, to_rm);
		
		prev_iterator = iterator;
		iterator = iterator->next;
		mem_free(allocator, prev_iterator, sizeof(fs_list_cell));
	}
}

int8_t fs_delete_root(mem_allocator *allocator, fs_file **root)
{
	if (root == NULL)
		return FS_ERROR;

	_fs_remove_recursive_files(allocator, *root);

	mem_free(allocator, (*root)->name, strlen((*root)->name)+1);
	mem_free(allocator, *root, sizeof(fs_file));

	*root = NULL;

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

STATIC int8_t _fs_push_front_cell(mem_allocator *allocator,
		fs_file *parent, fs_file *file)
{
	fs_list_cell *new_cell = mem_alloc(allocator, sizeof(fs_list_cell));
	if (new_cell == NULL)
		return FS_ERROR;
	new_cell->file = file;
	new_cell->next = parent->data.directory.children;
	parent->data.directory.children = new_cell;
	return FS_SUCCESS;
}

STATIC int8_t _fs_new_file(mem_allocator *allocator,
	uint8_t file_type, 
	const char *name, 
	fs_file *parent,
	fs_file **out_new_file)
{
	fs_file *new_child = mem_alloc(allocator, sizeof(fs_file));
	if (new_child == NULL)
		return FS_ERROR;
	
	// init
	new_child->file_type = file_type;
	if (_fs_cpyname(allocator, new_child, name) == FS_ERROR) {
		mem_free(allocator, new_child, sizeof(fs_file));
		return FS_ERROR;
	}
	new_child->parent = parent;

	// init file data
	if (file_type == FS_TYPE_REGULAR) {
		new_child->data.regular.start = NULL;
		new_child->data.regular.size = 0;
	} else if (file_type == FS_TYPE_DIRECTORY) {
		new_child->data.directory.children = NULL;
	}

	if (out_new_file != NULL)
		*out_new_file = new_child;

	return FS_SUCCESS;
}


STATIC int8_t _fs_add_file(mem_allocator *allocator, 
						fs_file *parent, uint8_t file_type, 
						const char *name, fs_file **newchild)
{
	if (!fs_is_directory(parent) || strlen(name) == 0)
		return FS_ERROR;

	// search if child with the same name already exists
	fs_iterator iterator = fs_get_first_child(parent);
	while (iterator != NULL) {
		if (strcmp(fs_get_name(fs_get_file_by_iter(iterator)), name) == 0)
			return FS_ERROR;
		iterator = fs_get_next_child(iterator);
	}

	fs_file *new_child = NULL;
	if (_fs_new_file(allocator, file_type, name, parent, &new_child) == FS_ERROR)
		return FS_ERROR;

	if (_fs_push_front_cell(allocator, parent, new_child) == FS_ERROR) {
		_fs_delete_file(allocator, new_child);
		return FS_ERROR;
	}

	if (newchild != NULL)
		*newchild = new_child;

	return FS_SUCCESS;
}


int8_t fs_add_regular(mem_allocator *allocator, fs_file *dir, const char *filename, fs_file **out_new_file)
{
	return _fs_add_file(allocator, dir, FS_TYPE_REGULAR, filename, out_new_file);
}

int8_t fs_add_dir(mem_allocator *allocator, 
		fs_file *dir, 
		const char *dirname, 
		fs_file **out_new_dir)
{
	return _fs_add_file(allocator, dir, FS_TYPE_DIRECTORY, dirname, out_new_dir);
}




int8_t fs_remove_file_by_name(mem_allocator *allocator, 
				fs_file *parent, 
				const char *name)
{
	if (!fs_is_directory(parent))
		return FS_ERROR;

	fs_list_cell *iterator = parent->data.directory.children;
	fs_list_cell **prev_iterator = &(parent->data.directory.children);

	while (iterator != NULL) {
		fs_file *to_rm = iterator->file;

		if (strcmp(name, to_rm->name) == 0) {
			if (to_rm->file_type == FS_TYPE_DIRECTORY)
				_fs_remove_recursive_files(allocator, to_rm);

			_fs_delete_file(allocator, to_rm);

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



int8_t fs_remove_file(mem_allocator *allocator, 
				fs_file *file_to_rm)
{
	if (file_to_rm == NULL)
		return FS_ERROR;

	if (file_to_rm->parent != NULL) {
		fs_list_cell *iterator = file_to_rm->parent->data.directory.children;
		fs_list_cell **prev_iterator = &(file_to_rm->parent->data.directory.children);

		while (iterator != NULL) {
			fs_file *it_file = iterator->file;

			if (it_file == file_to_rm) {
				if (file_to_rm->file_type == FS_TYPE_DIRECTORY)
					_fs_remove_recursive_files(allocator, file_to_rm);

				_fs_delete_file(allocator, file_to_rm);

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

	} else if (fs_is_directory(file_to_rm)) {
		_fs_remove_recursive_files(allocator, file_to_rm);
		return FS_SUCCESS;
	}
	
	return FS_ERROR;
}



const char * fs_get_name(fs_file *file)
{
	if (file == NULL)
		return "!nil_file!";
	return file->name;
}

uint32_t fs_get_regular_size(fs_file *file)
{
	if(!fs_is_regular(file))
		return 0;
	return file->data.regular.size;
}

fs_iterator fs_get_first_child(fs_file *dir)
{
	if (!fs_is_directory(dir))
		return NULL;
	return dir->data.directory.children;
}


// fs_iterator fs_get_next_child(fs_iterator iterator)
// {
// 	if (iterator == NULL)
// 		return NULL;
// 	return iterator->next;
// }


// fs_file* fs_get_file_by_iter(fs_iterator iterator)
// {
// 	if (iterator == NULL)
// 		return NULL;
// 	return iterator->file;
// }

fs_file* fs_get_file_by_name(fs_file *dir, const char *name)
{
	if (fs_is_directory(dir)) {
		fs_iterator iterator = fs_get_first_child(dir);
		while (iterator != NULL) {
			fs_file *file = fs_get_file_by_iter(iterator);
			if (strcmp(name, fs_get_name(file)) == 0)
				return file;
			iterator = fs_get_next_child(iterator);
		}
	}
	return NULL;
}


fs_file* fs_get_parent(fs_file* file)
{
	return (file == NULL) ? NULL : file->parent;
}

int8_t fs_write_regular(mem_allocator *allocator, 
						fs_file *file, 
						const uint8_t *data, 
						uint32_t size)
{
	if (!fs_is_regular(file))
		return FS_ERROR;

	uint8_t *start = (uint8_t*) mem_alloc(allocator, size);

	if (start == NULL && size > 0)
		return FS_ERROR;

	// free old content
	if (file->data.regular.size > 0) {
		mem_free(allocator, 
				 file->data.regular.start,
				 file->data.regular.size);
		file->data.regular.size = 0;
	}

	memcpy((void*)start, (void*)data, size);

	file->data.regular.start = start;
	file->data.regular.size = size;

	return FS_SUCCESS;
}


int8_t fs_get_memdata(fs_file *file, uint8_t **data, uint32_t *size)
{
	if (!fs_is_regular(file))
		return FS_ERROR;

	*data = file->data.regular.start;
	*size = file->data.regular.size;

	return FS_SUCCESS;
}


STATIC fs_file* _fs_get_file_by_path_rec(
	fs_file *working, 
	char *filepath
	)
{
	// "/tmp/../lol"

	if (filepath[0] == '\0') {
		return working;
	} else if (filepath[0] == '/' && filepath[1] == '\0') {
		return (fs_is_directory(working)) ? working : NULL;
	} else if (filepath[0] == '/') {
		filepath++;
	}

	if (filepath[0] == '.') { // "."
		if (filepath[1] == '.') { // ".."
			return (working->parent == NULL) ? NULL : 
				_fs_get_file_by_path_rec(
					working->parent,
					&filepath[2]
					);
		} else {
			// rm the '.' alias
			// continue with recursivity
			// because the filepath can be '././././.'
			return _fs_get_file_by_path_rec(
				working, filepath+1
				);
		}
	}

	uint8_t index = 0;
	bool was_end;
	while(filepath[index] != '\0' && filepath[index] != '/')
		index++;

	if (filepath[index] == '\0')
		was_end = true;
	else
		filepath[index] = '\0'; // erase '/'

	fs_file* child = fs_get_file_by_name(working, filepath);

	if (!was_end) {
		filepath[index] = '/';
	}

	if (child == NULL)
		return NULL;

	return _fs_get_file_by_path_rec(child,
		filepath + index);
}



fs_file* fs_get_file_by_path(fs_file *root, 
	fs_file *working, 
	char *filepath)
{
	if(!fs_is_directory(root) || !fs_is_directory(working))
		return NULL;

	if(filepath[0] == '\0') { // strlen == 0
		return NULL;
	}

	if (filepath[0] == '/') { // absolute path
		return _fs_get_file_by_path_rec( 
			root, filepath+1); // shift the '/'
	} else {
		return _fs_get_file_by_path_rec(
			working, filepath);
	}

}