#include <stdio.h>
#include "commands.h"


static void _tree_rec(fs_file *file, uint8_t deep)
{
	for(uint8_t i = 0; i < deep ; ++i)
		for(uint8_t j = 0; j < 4; j++)
			putchar(' ');
	puts(fs_get_name(file));

	if (fs_is_directory(file)) {
		fs_iterator iterator = fs_get_first_child(file);
		while (iterator != NULL) {
			_tree_rec(fs_get_file_by_iter(iterator), deep+1);
			iterator = fs_get_next_child(iterator);
		}
	}
}

uint8_t tree(fs_file *file)
{
	if (file == NULL)
		return CMD_ERROR;

	_tree_rec(file, 0);
	return CMD_SUCCESS;
}


uint8_t ls(fs_file *file)
{
	if (file == NULL)
		return CMD_ERROR;

	if (fs_is_directory(file)) {
		fs_iterator iterator = fs_get_first_child(file);
		while (iterator != NULL) {
			puts(fs_get_name(fs_get_file_by_iter(iterator)));
			iterator = fs_get_next_child(iterator);
		}
	} else {
		puts(fs_get_name(file));
	}

	return CMD_SUCCESS;
}

uint8_t mkdir(mem_allocator *allocator, 
	fs_file *parent, const char *dirname)
{
	return
		(fs_add_dir(allocator, 
			parent, 
			dirname, 
			NULL) == FS_SUCCESS) ?
		CMD_SUCCESS : CMD_ERROR;
}

uint8_t touch(mem_allocator *allocator, 
	fs_file *parent, const char *filename)
{
	return 
		(fs_add_regular(allocator, 
			parent, 
			filename, 
			NULL) == FS_SUCCESS) ? 
		CMD_SUCCESS : CMD_ERROR;
}

uint8_t write(mem_allocator *allocator, 
	fs_file *file, 
	const uint8_t *data, 
	uint32_t size)
{
	return 
		(fs_write_regular(allocator, 
			file, 
			data, 
			size) == FS_SUCCESS) ? 
		CMD_SUCCESS : CMD_ERROR;
}

uint8_t cat(fs_file *file)
{
	uint8_t *data;
	uint32_t size;

	if (fs_get_memdata(file, &data, &size) == FS_ERROR)
		return CMD_ERROR;

	fwrite((void*)data, size, 1, stdout);
	puts("");

	return CMD_SUCCESS;
}

uint8_t rm(mem_allocator *allocator, fs_file *parent, const char *name)
{
	return 
		(fs_remove_file(allocator, 
			parent, 
			name) == FS_SUCCESS) ? 
		CMD_SUCCESS : CMD_ERROR;
}

uint8_t exec(mem_allocator *allocator, fs_file *file);
