#include "simu_mips.h"


#include <stdio.h>

char mips_get_char()
{
	return getchar();
}

void mips_get_line(char buffer[], 
					uint8_t buf_size)
{
	fgets(buffer, buf_size, stdin);
}

void mips_putc(char c)
{
	fprintf(stdout, "%c", c);
}

void mips_puts(const char *string)
{
	fprintf(stdout, "%s", string);
}

// display string and go to new line
void mips_puts_nl(const char *string)
{
	mips_puts(string);
	mips_putc('\n');
}