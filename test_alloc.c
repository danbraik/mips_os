#ifdef TEST_ALLOC
#include "alloc.h"

int main(int argc, char const *argv[])
{
	mem_allocator allocator;
	uint8_t *memory = malloc(512);
	mem_init(&allocator, memory, 512);

	void *ptr = mem_alloc(&allocator, 50);
	mem_free(&allocator, ptr, 50);

	ptr = mem_alloc(&allocator, 800);
	mem_free(&allocator, ptr, 800);

	for(int i=0; i < 50; ++i) {
		ptr = mem_alloc(&allocator, i+1);
		mem_free(&allocator, ptr, i+1);
	}

	return 0;
}
#endif