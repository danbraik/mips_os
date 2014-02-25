#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <stdlib.h>
#include <inttypes.h>

typedef struct mem_allocator {
	uint8_t *start; // == next cell
	uint32_t size;
} mem_allocator;

/**
 * size must be greater or equal than sizeof(struct mem_allocator)
*/
void mem_init(mem_allocator *allocator, uint8_t *start, uint32_t size);

void * mem_alloc(mem_allocator *allocator, uint32_t size);

void mem_free(mem_allocator *allocator, void *ptr, uint32_t size);


#endif