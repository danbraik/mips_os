#include <stdbool.h>
#include "alloc.h"


void mem_init(mem_allocator *allocator, uint8_t *start, uint32_t size)
{
	allocator->start = start;
	allocator->size = size;

	// init the next cell
	mem_allocator *next = (mem_allocator*) allocator->start;
	next->start = NULL;
	next->size = 0;
}

void * mem_alloc(mem_allocator *allocator, uint32_t size)
{
	bool go = true;
	mem_allocator *iterator = allocator;

	while(go) {
		if (iterator->size >= size) {
			void *ptr = iterator->start;

			mem_allocator *next = (mem_allocator*) iterator->start;
			mem_allocator *newnext = (mem_allocator*) ((uint8_t*)(iterator->start) + size);
			
			// move cell
			newnext->start = next->start;
			newnext->size = next->size;

			iterator->start = (uint8_t*) newnext;
			iterator->size -= size;

			return ptr;

		} else {
			if (iterator->start == NULL)
				return NULL;
			else
				iterator = (mem_allocator*) iterator->start; // go to next cell
		}
	}
	return NULL;
}

void mem_free(mem_allocator *allocator, void *ptr, uint32_t size)
{
	
}
