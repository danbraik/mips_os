#ifdef TEST_ALLOC
#include "alloc.h"

int main(int argc, char const *argv[])
{
	void * allptr[320];

	mem_allocator allocator;
	uint8_t *memory = malloc(512);
	mem_init(&allocator, memory, 512);

	//mem_debug(&allocator);

	void *ptr = mem_alloc(&allocator, 50);
	//mem_debug(&allocator);

	mem_free(&allocator, ptr, 50);
	//mem_debug(&allocator);

	ptr = mem_alloc(&allocator, 800);
	//mem_debug(&allocator);

	mem_free(&allocator, ptr, 800);
	mem_debug(&allocator);

	puts("*** ALLOC TIME ***");
	int max = 3;

	for(int i=0; i < max; ++i) {
		allptr[i] = mem_alloc(&allocator, i+1);
		mem_debug(&allocator);
	}

	puts("*** FREE TIME ***");

	
	for(int i=0; i < max; ++i) {
		mem_free(&allocator, allptr[i], i+1);
		mem_debug(&allocator);
	}

	

	return 0;
}
#endif
