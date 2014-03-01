#include <stdbool.h>
#include <stdio.h>
#include "alloc.h"



void mem_init(mem_allocator *allocator, 
	uint8_t *start, 
	uint32_t size)
{
	allocator->start = start;
	allocator->size = size;

	// init the next cell
	mem_allocator *next = (mem_allocator*) allocator->start;
	next->start = NULL;
	next->size = 0;
}



static void _mem_correct_size(uint32_t *size)
{
	if (*size < sizeof(mem_allocator))
		*size = sizeof(mem_allocator);
}

void * mem_alloc(mem_allocator *allocator, 
	uint32_t size)
{
	_mem_correct_size(&size);

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

#ifdef TRACE_ALLOC
			printf("ALLOC %p\t%u\n", ptr, size);
#endif
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

	_mem_correct_size(&size);

	uint8_t *start = (uint8_t*) ptr;
	mem_allocator *iterator = allocator;
	mem_allocator *prev_iterator = NULL;

	while(true) {

		if (start < iterator->start) {
			mem_allocator *cell = (mem_allocator*) start;

			bool can_merge_left = prev_iterator != NULL && 
					   (prev_iterator->start // == (uint8*)iterator
					   	+ prev_iterator->size) == start;

			bool can_merge_right = start + size == iterator->start;

			if (can_merge_left && can_merge_right) {
				mem_allocator *next_cell = (mem_allocator*)(iterator->start);

				uint32_t next_size = iterator->size;
				*iterator = *next_cell;

				prev_iterator->size += size + next_size;
			
			} else if (can_merge_right) {
				mem_allocator *next = (mem_allocator*) iterator->start;
				*cell = *next;
				iterator->size += size;
				iterator->start = start;

			} else if (can_merge_left) {
				prev_iterator->size += size;

			// else, create intermediate cell	
			} else {
				*cell = *iterator;
				iterator->size = size;
				iterator->start = start;
			}
#ifdef TRACE_ALLOC
			printf("FREE %p\t%u\n", ptr, size);
#endif
			return;
		} else {
			// move iterator to next cell
			if (iterator->start == NULL)
				return;
			prev_iterator = iterator;
			iterator = (mem_allocator*) iterator->start;
		}

	}
}



void mem_debug(mem_allocator *allocator)
{

	puts("* * * * *");
	puts("* DBG allocator");

	
	uint32_t free_mem = 0;
	uint32_t free_chunks = 0;

	mem_allocator *iterator = allocator;
	
	while (iterator != NULL) {
		printf("Chunk[%d] @%p\n", free_chunks, iterator);
		printf("      nxt @%p\n", iterator->start);
		printf("      siz  %d\n", iterator->size);

		free_mem += iterator->size;

		iterator = (mem_allocator*)(iterator->start);
		free_chunks++;

	}
	free_chunks--; // balance last chunk (guard cell)

	printf("* Free mem :  %u\n", free_mem);
	printf("* Nb chunks : %u\n", free_chunks);

	puts("* * * * *");
}