
#include <string.h>
#include <stdio.h>

#include "simu_mips.h"

#include "commands.h"


static void _tree_rec(fs_file *file, uint8_t deep)
{
	for(uint8_t i = 0; i < deep ; ++i)
		for(uint8_t j = 0; j < 4; j++)
			mips_putc(' ');
	mips_puts_nl(fs_get_name(file));

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
	if (!fs_is_directory(file))
		return CMD_ERROR;

	_tree_rec(file, 0);
	return CMD_SUCCESS;
}


static void _ls_regular(fs_file* file)
{
	// print size
	uint32_t size = fs_get_regular_size(file);
	// uint32_t tmp = size;
	// int8_t nbc = 0;
	// while(tmp > 0) {
	// 	tmp /= 10;
	// 	nbc++;
	// }
	// if (nbc == 0)
	// 	mips_putc('0');
	// for(--nbc;nbc > 0; --nbc) {
	// 	mips_putc('0' + (size / (10*nbc)));
	// }
	// mips_putc(' ');
	char buf[5];
	sprintf(buf, "%4u", size);
	mips_puts(buf);
	mips_putc(' ');

	// print name
	mips_puts_nl(fs_get_name(file));
}

uint8_t ls(fs_file *file)
{
	if (file == NULL)
		return CMD_ERROR;

	if (fs_is_directory(file)) {
		fs_iterator iterator = fs_get_first_child(file);
		while (iterator != NULL) {
			fs_file *file = fs_get_file_by_iter(iterator);
			if (fs_is_regular(file)) {
				_ls_regular(file);
			} else if (fs_is_directory(file)) {
				mips_puts("     ");
				mips_puts_nl(fs_get_name(file));
			}
			iterator = fs_get_next_child(iterator);
		}
	} else {
		_ls_regular(file);
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
	const char *data_hex)
{
	const uint8_t *data = (uint8_t*)data_hex;
	uint32_t size = strlen(data_hex);

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

	for(uint32_t i=0; i < size; ++i)
		mips_putc(data[i]);
	mips_putc('\n');

	return CMD_SUCCESS;
}

uint8_t rm(mem_allocator *allocator, fs_file *file)
{
	if (file == NULL)
		return CMD_ERROR;

	return 
		(fs_remove_file(allocator, 
			file) == FS_SUCCESS) ? 
		CMD_SUCCESS : CMD_ERROR;
}



typedef struct _pwd_struct
{
	char *name;
	struct _pwd_struct *next;
} _pwd_struct;


uint8_t pwd(mem_allocator *allocator, 
			cmd_filesystem *filesystem)
{
	if (filesystem == NULL)
		return CMD_ERROR;

	_pwd_struct *first = NULL, *old_f;

	// compute the path from root to the working dir
	fs_file *iterator = filesystem->working;

	// iterate over parent except root node
	while(iterator != NULL && fs_get_parent(iterator) != NULL) {
		_pwd_struct *cell = mem_alloc(allocator, sizeof(_pwd_struct));
		cell->next = first;
		cell->name = fs_get_name(iterator);
		first = cell;
		iterator = fs_get_parent(iterator);
	}

	// display root
	mips_putc('/');

	// display the path
	while (first != NULL) {
		mips_puts(first->name);
		mips_putc('/');
		old_f = first;
		first = first->next;
		mem_free(allocator, old_f, sizeof(_pwd_struct));
	}
	mips_putc('\n');

	return CMD_SUCCESS;
}


uint8_t cd(cmd_filesystem *filesystem, char *filepath)
{
	fs_file *new_working = fs_get_file_by_path(
			filesystem->root,
			filesystem->working, 
			filepath);
	if (!fs_is_directory(new_working))
		return CMD_ERROR;
	filesystem->working = new_working;
	return CMD_SUCCESS;
}


uint8_t exec(mem_allocator *allocator, fs_file *file)
{

	return CMD_SUCCESS;
}
