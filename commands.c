
#include <string.h>
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


static void _ls(fs_file* file)
{
	// print size
	uint32_t size = fs_get_regular_size(file);
	uint32_t tmp = size;
	int8_t nbc = 0;
	while(tmp > 0) {
		tmp /= 10;
		nbc++;
	}
	if (nbc == 0)
		mips_putc('0');
	for(--nbc;nbc > 0; --nbc) {
		mips_putc('0' + (size / (10*nbc)));
	}
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
			_ls(fs_get_file_by_iter(iterator));
			iterator = fs_get_next_child(iterator);
		}
	} else {
		_ls(file);
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

uint8_t rm(mem_allocator *allocator, fs_file *parent, const char *name)
{
	return 
		(fs_remove_file(allocator, 
			parent, 
			name) == FS_SUCCESS) ? 
		CMD_SUCCESS : CMD_ERROR;
}



uint8_t pwd(cmd_filesystem *filesystem)
{
	if (filesystem == NULL)
		return CMD_ERROR;

	fs_file *iterator = filesystem->working;
	while(iterator != NULL) {
		mips_putc('.');
		mips_puts(fs_get_name(iterator));
		iterator = fs_get_parent(iterator);
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
