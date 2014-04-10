#ifndef TESTS_H
#define TESTS_H

#ifdef PC
	#define MEM_SIZE 1024+512+256+64+32+16 // == 1904
#elif defined(QEMU)
	#define MEM_SIZE 1024+512+256+64+32+16 // == 1904
#elif defined(MIPS)
	#define MEM_SIZE 512 // == 1904
#endif



#endif
