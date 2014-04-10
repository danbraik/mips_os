#include "simu_mips.h"

#ifdef PC
#include <stdio.h>




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


























































#elif defined(QEMU)


#include <stdio.h>
//#include <string.h> 
#include <inttypes.h>

// espacement entre les caractères
#define E 2

// largeur caractères
#define CHAR_X 8
// hauteur caractères
#define CHAR_Y 8

#define PIX_ADDR_X 9
#define PIX_ADDR_Y 8
#define PIX_ADDR_X_SHIFT 5
#define PIX_ADDR_X_MASK ((1<<PIX_ADDR_X)-1)
#define PIX_ADDR_X_MASK_WORD ((1<<PIX_ADDR_X_SHIFT)-1)
#define PIX_ADDR_Y_SHIFT (PIX_ADDR_X-PIX_ADDR_X_SHIFT)
#define PIX_ADDR_Y_MASK ((1<<PIX_ADDR_Y)-1)


struct gfx
{
	int32_t X;
	int32_t Y;
};

void stepCursor(struct gfx* c) {
	c->X += CHAR_X+E;
	if(c->X > 311) {
		c->Y += CHAR_Y+E;
		c->X = E;
	}
}
void skipLine(struct gfx* c) {
	c->X = E;
	c->Y += CHAR_Y+E;
}

// affiche le pixel aux coordonnées données par gfx
void affiche_pixel(struct gfx* c, uint32_t pix)
{
	uint32_t *img = (uint32_t*)0x80051000; // 0x10000, emulator -> 0x80051000
	uint32_t addr;
	uint32_t d, mask;
	addr = ((c->Y & PIX_ADDR_Y_MASK) << PIX_ADDR_Y_SHIFT) | ((c->X & PIX_ADDR_X_MASK) >> PIX_ADDR_X_SHIFT);
	d = 31-(PIX_ADDR_X_MASK_WORD & c->X);
	mask = 1<<d;
	img[addr] = (img[addr] & ~mask) | ((pix&1) << d);
}

// tableau contenant les valeurs des pixels
// pour chaque lettres afin de les afficher
uint8_t char_vals[][8] = {
		{0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, // espace
		{0b11111111,0b11111001,0b11111001,0b11111001,0b11111001,0b11111111,0b11111111,0b11111001}, // A
		{0b11111100,0b11110110,0b11110110,0b11111100,0b11110010,0b11110011,0b11111111,0b11111110}, // B
		{0b11111111,0b11110011,0b11110000,0b11110000,0b11110000,0b11110011,0b11111111,0b11111111}, // C
		{0b11111110,0b11110011,0b11110011,0b11110011,0b11110011,0b11110011,0b11111111,0b11111110}, // D
		{0b11111111,0b11110011,0b11110000,0b11111000,0b11111000,0b11110011,0b11111111,0b11111111}, // E
		{0b11111111,0b11110011,0b11110000,0b11110000,0b11110000,0b11111100,0b11111100,0b11110000}, // F
		{0b11111111,0b11110011,0b11110000,0b11110111,0b11110111,0b11110011,0b11111111,0b11111111}, // G
		{0b11110011,0b11110011,0b11110011,0b11110011,0b11110011,0b11111111,0b11111111,0b11110011}, // H
		{0b11111111,0b00111100,0b00111100,0b00111100,0b00111100,0b00111100,0b11111111,0b11111111}, // I
		{0b11111111,0b00111100,0b00111100,0b00111100,0b00111100,0b10111100,0b11111100,0b01111000}, // J
		{0b11110011,0b11110011,0b11110011,0b11110110,0b11111100,0b11111110,0b11110111,0b11110011}, // K
		{0b11110000,0b11110000,0b11110000,0b11110000,0b11110000,0b11110011,0b11111111,0b11111111}, // L
		{0b11100011,0b11110111,0b11111111,0b11111111,0b11111111,0b11101011,0b11100011,0b11100011}, // M
		{0b11000011,0b11100011,0b11110011,0b11111011,0b11111111,0b11110111,0b11110011,0b11110001}, // N
		{0b11111111,0b11110011,0b11110011,0b11110011,0b11110011,0b11110011,0b11111111,0b11111111}, // O
		{0b11111111,0b11110011,0b11110011,0b11110011,0b11110011,0b11111111,0b11111111,0b11110000}, // P
		{0b11111111,0b11110011,0b11110011,0b11110011,0b11110011,0b11110011,0b01111110,0b00000011}, // Q
		{0b11111110,0b11110011,0b11110011,0b11110011,0b11110011,0b11111110,0b11111111,0b11110011}, // R
		{0b11111111,0b11110011,0b11110000,0b11111111,0b00000011,0b11110011,0b11111111,0b11111111}, // S
		{0b11111111,0b00111100,0b00111100,0b00111100,0b00111100,0b00111100,0b00111100,0b00111100}, // T
		{0b11110011,0b11110011,0b11110011,0b11110011,0b11110011,0b11110011,0b11111111,0b11111111}, // U
		{0b11110011,0b11110011,0b11110011,0b11110011,0b01111011,0b00111111,0b00011111,0b00001111}, // V
		{0b11100011,0b11100011,0b11101011,0b11111111,0b11111111,0b11111111,0b11110111,0b11100011}, // W
		{0b11110001,0b11111011,0b01111111,0b00111110,0b01111111,0b11111111,0b11111011,0b11110001}, // X
		{0b11110011,0b11110011,0b11110011,0b11110011,0b11111011,0b01111110,0b00111100,0b00111100}, // Y
		{0b11111111,0b11110011,0b00000011,0b11111111,0b11110000,0b11110011,0b11111111,0b11111111}, // Z
		{0b01111110,0b11000011,0b11000111,0b11001101,0b11011001,0b11110001,0b11100001,0b01111110}, // 0
		{0b00011000,0b01111000,0b00011000,0b00011000,0b00011000,0b00011000,0b00011000,0b11111111}, // 1
		{0b01111100,0b11111111,0b11000011,0b00000011,0b00111110,0b01100000,0b11000011,0b11111111}, // 2
		{0b01111110,0b11100111,0b11000011,0b00001110,0b00001110,0b11000011,0b11100111,0b01111110}, // 3
		{0b00011110,0b00111110,0b01110110,0b11100110,0b11000111,0b11111111,0b00000110,0b00011111}, // 4
		{0b11111111,0b11100000,0b11100000,0b11111110,0b00000111,0b11000011,0b11100111,0b01111110}, // 5
		{0b00111111,0b01111100,0b11110000,0b11100000,0b11111110,0b11100011,0b11100011,0b01111110}, // 6
		{0b11111111,0b11000011,0b11000011,0b00000111,0b00001110,0b00011100,0b00011000,0b00011000}, // 7
		{0b01111110,0b11000011,0b11000011,0b11000011,0b01111110,0b11000011,0b11000011,0b01111110}, // 8
		{0b01111110,0b11000011,0b11000011,0b01111111,0b00000011,0b00000110,0b00001110,0b00111100}, // 9
		{0b00011000,0b11111111,0b11011000,0b11111111,0b00011011,0b11011011,0b11111111,0b00011000}  // $
	};

// Print character h and move cursor
void print_char(struct gfx* c, char h) {
	uint8_t n;
	switch(h) {
		case ' ': n = 0; break;
		case 'A': n = 1; break;
		case 'B': n = 2; break;
		case 'C': n = 3; break;
		case 'D': n = 4; break;
		case 'E': n = 5; break;
		case 'F': n = 6; break;
		case 'G': n = 7; break;
		case 'H': n = 8; break;
		case 'I': n = 9; break;
		case 'J': n = 10; break;
		case 'K': n = 11; break;
		case 'L': n = 12; break;
		case 'M': n = 13; break;
		case 'N': n = 14; break;
		case 'O': n = 15; break;
		case 'P': n = 16; break;
		case 'Q': n = 17; break;
		case 'R': n = 18; break;
		case 'S': n = 19; break;
		case 'T': n = 20; break;
		case 'U': n = 21; break;
		case 'V': n = 22; break;
		case 'W': n = 23; break;
		case 'X': n = 24; break;
		case 'Y': n = 25; break;
		case 'Z': n = 26; break;
		case '0': n = 27; break;
		case '1': n = 28; break;
		case '2': n = 29; break;
		case '3': n = 30; break;
		case '4': n = 31; break;
		case '5': n = 32; break;
		case '6': n = 33; break;
		case '7': n = 34; break;
		case '8': n = 35; break;
		case '9': n = 36; break;
		case '$': n = 37; break;
		case '\n':
			skipLine(c);
			return;
		break;
		default: n = 0; break;
	}

	uint32_t x_s = c->X + 9;
	uint32_t y_s = c->Y;
	for (int i = 0; i < CHAR_Y; ++i) {
		uint8_t tmp = char_vals[n][i];
		c->X = x_s;
		for (int j = 0; j < CHAR_X; ++j) {
			affiche_pixel(c, tmp & 1);
			c->X--;
			tmp >>= 1;
		}
		c->Y++;
	}
	c->Y = y_s;

	stepCursor(c);
}

// Print string s
void print_string(struct gfx* c, const char *s) {
	int n = strlen(s);
	for(int i = 0; i < n; ++i) {
		print_char(c, s[i]);
	}
}



	



void mips_get_line(char buffer[], 
					uint8_t buf_size)
{
	fgets(buffer, buf_size, stdin);
}

void mips_putc(char c)
{
	struct gfx C = {2, 2};
	print_char(&C, c);
}

void mips_puts(const char *string)
{
	struct gfx C = {2, 2};
	print_string(&C, string);
}



#else











































void mips_get_line(char buffer[], 
					uint8_t buf_size)
{
}

void mips_putc(char c)
{
}

void mips_puts(const char *string)
{
}




#endif






















// display string and go to new line
void mips_puts_nl(const char *string)
{
	mips_puts(string);
	mips_putc('\n');
}
