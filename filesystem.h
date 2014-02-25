#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <inttypes.h>

typedef struct io_file_regular {
	uint8_t *start;
	uint32_t size;
} io_file_regular;

struct io_file_list_cell;
typedef struct io_file_list_cell* io_file_list;

typedef struct io_directory {
	io_file_list children;
} io_directory;

enum {IO_FILE_TYPE_REGULAR, IO_FILE_TYPE_DIRECTORY};

typedef struct io_file_list_cell {
	union {
		io_file_regular regular;
		io_directory directory;
	} file;
	uint8_t file_type; // REGULAR, DIRECTORY
	struct io_file_list_cell *next;
} io_file_list_cell;


void io_init_directory(io_directory* dir);

void io_add_file(io_directory* dir);





#endif