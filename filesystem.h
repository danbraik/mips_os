#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <inttypes.h>
#include <stdbool.h>
#include "alloc.h"

#define FS_ERROR 0
#define FS_SUCCESS 1


/*
 * FS_REGULAR
 */
typedef struct {
	uint8_t *start;
	uint32_t size;
} fs_regular;

/*
 * FS_DIRECTORY
 */
typedef struct fs_list_cell* fs_iterator;

typedef struct {
	fs_iterator children;
} fs_directory;

/*
 * FS_FILE
 */
enum {FS_TYPE_REGULAR, FS_TYPE_DIRECTORY};

typedef struct {
	union {
		fs_regular regular;
		fs_directory directory;
	} data;
	uint8_t file_type; // REGULAR, DIRECTORY
	char * name;
} fs_file;


typedef struct fs_list_cell {
	fs_file *file;
	fs_iterator next;
} fs_list_cell;


int8_t fs_new_root(mem_allocator *allocator, fs_file *root);

int8_t fs_delete_root(mem_allocator *allocator, fs_file *root);

int8_t fs_add_regular(mem_allocator *allocator, 
				fs_file *dir, 
				const char *filename, 
				fs_file **newfile);

int8_t fs_add_dir(mem_allocator *allocator, 
				fs_file *dir, 
				const char *dirname,
				fs_file **newdir);

int8_t fs_remove_file(mem_allocator *allocator, 
				fs_file *parent, 
				const char *name);

bool fs_is_directory(fs_file* file);

const char * fs_get_name(fs_file *file);

fs_iterator fs_get_first_child(fs_file *dir);

fs_iterator fs_get_next_child(fs_iterator iterator);

fs_file* fs_get_file_by_iter(fs_iterator iterator);


//void fs_get_root_cursor(fs_file *dir);

int8_t fs_get_file(fs_file *root, fs_file *working, const char *filepath);

//int fs_write_file(mem_allocator *allocator, fs_file *file, const uint8_t *data, uint32_t size);


#endif