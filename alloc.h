#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <inttypes.h>

#ifdef PRINT_STACK
#define STATIC 
#else
#define STATIC static
#endif

typedef struct mem_allocator {
	uint8_t *start; // == mem_allocator* (next cell)
	uint32_t size;
} mem_allocator;



/**
 * size must be greater or equal than sizeof(struct mem_allocator)
*/
void mem_init(mem_allocator *allocator, uint8_t *start, uint32_t size);

void * mem_alloc(mem_allocator *allocator, 
	uint32_t size);

void mem_free(mem_allocator *allocator, void *ptr, uint32_t size);

void mem_debug(mem_allocator *allocator);

#endif