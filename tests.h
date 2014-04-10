#ifndef TESTS_H
#define TESTS_H

#ifdef PC
	#define MEM_SIZE 1024
#elif defined(QEMU)
	#define MEM_SIZE 1024
#elif defined(MIPS)
	#define MEM_SIZE 512
#endif

#endif

