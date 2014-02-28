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