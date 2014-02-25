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
	mem_allocator *iterator = allocator;

	while(true) {
		if (iterator->size >= size) {
			void *ptr = iterator->start;

			mem_allocator *next = (mem_allocator*) iterator->start;
			mem_allocator *newnext = (mem_allocator*) ((uint8_t*)(iterator->start) + size);
			
			// move cell
			*newnext = *next;

			iterator->start = (uint8_t*) newnext;
			iterator->size -= size;

			return ptr;

		} else {
			// move iterator to next cell
			if (iterator->start == NULL) // end of list
				return NULL;
			else
				iterator = (mem_allocator*) iterator->start; // go to next list cell
		}
	}
	return NULL;
}

void mem_free(mem_allocator *allocator, void *ptr, uint32_t size)
{
	if (ptr == NULL || size == 0)
		return;

	const uint8_t *start = (uint8_t*) ptr;
	mem_allocator *iterator = allocator;

	while(true) {

		if (start < iterator->start) {
			mem_allocator *cell = (mem_allocator*) start;

			// test if we can merge
			if (start + size == iterator->start) {
				mem_allocator *next = (mem_allocator*) iterator->start;
				*cell = *next;
				iterator->size += size;
			} else {
				*cell = *iterator;
				iterator->size = size;
			}

			iterator->start = start;			

			return;
		} else {
			// move iterator to next cell
			if (iterator->start == NULL)
				return;
			else
				iterator = (mem_allocator*) iterator->start;
		}

	}
}
