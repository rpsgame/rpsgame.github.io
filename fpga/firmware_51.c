/*
* sdcc firmware.c --model-large
* https://github.com/BrunoLevy/TinyPrograms/blob/main/mandelbrot.c
*/

#include <stdio.h>

__sfr __at(0xc1) uart;

int putchar(int c)
{
	while (uart);
	uart = c;
	return c;
}

#define W 32
#define H 32

#define mandel_shift 10
#define mandel_mul (1 << mandel_shift)
#define xmin -2*mandel_mul
#define ymax  2*mandel_mul
#define ymin -2*mandel_mul
#define xmax  2*mandel_mul
#define dx (xmax-xmin)/H
#define dy (ymax-ymin)/H
#define norm_max (4 << mandel_shift)

#define ANSIRGB(R,G,B) "\033[48;2;" #R ";"  #G ";" #B "m  "

const char* colormap[21] = {
   ANSIRGB(0, 0,  0),
   ANSIRGB(0, 0, 40),
   ANSIRGB(0, 0, 80),
   ANSIRGB(0, 0,120),
   ANSIRGB(0, 0,160),
   ANSIRGB(0, 0,200),
   ANSIRGB(0, 0,240),

   ANSIRGB(0,  0, 0),
   ANSIRGB(0, 40, 0),
   ANSIRGB(0, 80, 0),
   ANSIRGB(0,120, 0),
   ANSIRGB(0,160, 0),
   ANSIRGB(0,200, 0),
   ANSIRGB(0,240, 0),

   ANSIRGB(0, 0, 0),
   ANSIRGB(40, 0, 0),
   ANSIRGB(80, 0, 0),
   ANSIRGB(120, 0, 0),
   ANSIRGB(160, 0, 0),
   ANSIRGB(200, 0, 0),
   ANSIRGB(240, 0, 0)
};

void main(void) {
	long frame = 0;
	for (;;) {
		long last_color = -1;
		printf_small("\033[H");
		long Ci = ymin;
		for (unsigned char Y = 0; Y < H; ++Y) {
			long Cr = xmin;
			for (unsigned char X = 0; X < W; ++X) {
				long Zr = Cr;
				long Zi = Ci;
				long iter = 20;
				while (iter > 0) {
					long Zrr = (Zr * Zr) >> mandel_shift;
					long Zii = (Zi * Zi) >> mandel_shift;
					long Zri = (Zr * Zi) >> (mandel_shift - 1);
					Zr = Zrr - Zii + Cr;
					Zi = Zri + Ci;
					if (Zrr + Zii > norm_max) {
						break;
					}
					--iter;
				}
				long color = (iter + frame) % 21;
				printf_small("%s", color == last_color ? "  " : colormap[color]);
				last_color = color;
				Cr += dx;
			}
			Ci += dy;
			printf_small("\033[49m\n");
			last_color = -1;
		}
		++frame;
	}
}
