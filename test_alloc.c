#include "alloc.h"

int main(int argc, char const *argv[])
{
	mem_allocator allocator;
	uint8_t *memory = malloc(512);
	mem_init(&allocator, memory, 512);

	void *ptr = mem_alloc(&allocator, 50);

	ptr = mem_alloc(&allocator, 500);

	return 0;
}