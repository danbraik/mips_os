#ifndef SIMU_MIPS
#define SIMU_MIPS

#include <inttypes.h>

char mips_get_char();

void mips_get_line(char buffer[], uint8_t buf_size);

void mips_putc(char c);

void mips_puts(const char *string);

// display string and go to new line
void mips_puts_nl(const char *string);


#endif