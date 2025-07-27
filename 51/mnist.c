/*
* 8051 CNN MNIST Handwritten Digit Recognition
* STC8H8K64U-45I-PDIP40 @40MHz
* LCD16080 ST7735
* sdcc mnist.c --model-large
* https://liuliqiang.com/51/mnist.c
* https://liuliqiang.com/51/model_mnist.h
* https://liuliqiang.com/51/train_mnist.py
* by liuliqiang 2025-07-23
*/

#include <8051.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "model_mnist.h"

#define SPI 1

#define LCD_SCL P1_5
#define LCD_SDA P1_3
#define LCD_RES P1_4
#define LCD_DC	P1_6
#define LCD_CS	P1_7
#define LCD_BLK	P1_0 // PWM1P

#define LCD_SCLK_CLR() LCD_SCL = 0
#define LCD_SCLK_SET() LCD_SCL = 1

#define LCD_MOSI_CLR() LCD_SDA = 0
#define LCD_MOSI_SET() LCD_SDA = 1

#define LCD_RES_CLR() LCD_RES = 0
#define LCD_RES_SET() LCD_RES = 1

#define LCD_DC_CLR() LCD_DC = 0
#define LCD_DC_SET() LCD_DC = 1

#define LCD_CS_CLR() LCD_CS = 0
#define LCD_CS_SET() LCD_CS = 1

__sfr __at(0x93) P0M1;
__sfr __at(0x94) P0M0;
__sfr __at(0x91) P1M1;
__sfr __at(0x92) P1M0;
__sfr __at(0x95) P2M1;
__sfr __at(0x96) P2M0;
__sfr __at(0xB1) P3M1;
__sfr __at(0xB2) P3M0;
__sfr __at(0xB3) P4M1;
__sfr __at(0xB4) P4M0;
__sfr __at(0xC9) P5M1;
__sfr __at(0xCA) P5M0;
__sfr __at(0xCB) P6M1;
__sfr __at(0xCC) P6M0;
__sfr __at(0xE1) P7M1;
__sfr __at(0xE2) P7M0;

__sfr __at(0xCE) SPCTL;
__sfr __at(0xCD) SPSTAT;
__sfr __at(0xCF) SPDAT;

__sfr __at(0xA2) P_SW1;
__sfr __at(0xBA) P_SW2;

#define PWMA_CCER1      (*(uint8_t volatile __xdata *)0xfecc)
#define PWMA_CCMR1      (*(uint8_t volatile __xdata *)0xfec8)
#define PWMA_ENO        (*(uint8_t volatile __xdata *)0xfeb1)
#define PWMA_BKR        (*(uint8_t volatile __xdata *)0xfedd)
#define PWMA_CCR1       (*(uint16_t volatile __xdata *)0xfed5)
#define PWMA_ARR        (*(uint16_t volatile __xdata *)0xfed2)
#define PWMA_CR1        (*(uint8_t volatile __xdata *)0xfec0)

#define P1SR        (*(uint8_t volatile __xdata *)0xFE21)
#define P3SR        (*(uint8_t volatile __xdata *)0xFE23)

#define FOSC        40000000UL
#define BRT         (65536 - FOSC / 115200 / 4)

__sfr __at(0x8e) AUXR;
__sfr __at(0xd6) T2H;
__sfr __at(0xd7) T2L;

static uint8_t __data x, y, i, j, k, n;

static void UartInit(void)
{
	SCON = 0x50;
	T2L = (uint8_t)BRT;
	T2H = (uint8_t)(BRT >> 8);
	AUXR = 0x15;
}

int putchar(int c)
{
	SBUF = c;
	while (!TI);
	TI = 0;
	return c;
}

static inline void SPI_Send(uint8_t dat)
{
#if SPI
	SPDAT = dat;
	while (!(SPSTAT & 0x80));
	SPSTAT = 0xC0;
#else
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (dat & 0x80)
		{
			LCD_MOSI_SET();
		}
		else
		{
			LCD_MOSI_CLR();
		}
		LCD_SCLK_CLR();
		LCD_SCLK_SET();
		dat <<= 1;
	}
#endif
}
static void Delay100ms(void)	//@40MHz
{
	unsigned char __data i, j, k;

	__asm__("nop");
	i = 21;
	j = 75;
	k = 189;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
static void LCD_WR_Bus(uint8_t dat)
{
	LCD_CS_CLR();
	SPI_Send(dat);
	LCD_CS_SET();
}
static void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_CLR();
	LCD_CS_CLR();
	SPI_Send(dat);
	LCD_CS_SET();
	LCD_DC_SET();
}
static void LCD_WR_DATA8(uint8_t dat)
{
	LCD_CS_CLR();
	SPI_Send(dat);
	LCD_CS_SET();
}
static void LCD_Init(void)
{
	LCD_RES_CLR();
	Delay100ms();
	LCD_RES_SET();
	Delay100ms();

	LCD_WR_REG(0x11);
	Delay100ms();
	LCD_WR_REG(0xB1);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xB3);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xB4);
	LCD_WR_DATA8(0x03);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x04);

	LCD_WR_REG(0xC1);
	LCD_WR_DATA8(0xC5);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0x2A);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x8D);
	LCD_WR_DATA8(0xEE);

	LCD_WR_REG(0xC5);
	LCD_WR_DATA8(0x06);

	LCD_WR_REG(0x36);
	LCD_WR_DATA8(0x78);
	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x55);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0x0b);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x0a);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x1a);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x1d);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x37);
	LCD_WR_DATA8(0x3c);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x10);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0x0c);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x0d);
	LCD_WR_DATA8(0x1b);
	LCD_WR_DATA8(0x19);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x1d);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x26);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x3E);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x10);
}

static void LCD_Set_Address(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	LCD_CS_CLR();
	LCD_DC_CLR();
	SPI_Send(0x2a);
	LCD_DC_SET();
	SPI_Send(0);
	SPI_Send(x1);
	SPI_Send(0);
	SPI_Send(x2);
	LCD_DC_CLR();
	SPI_Send(0x2b);
	LCD_DC_SET();
	SPI_Send(0);
	SPI_Send(y1 + 24);
	SPI_Send(0);
	SPI_Send(y2 + 24);
	LCD_DC_CLR();
	SPI_Send(0x2c);
	LCD_DC_SET();
	LCD_CS_SET();
}
static void LCD_Clear(uint16_t c)
{
	LCD_Set_Address(0, 0, (uint8_t)(160 - 1), (uint8_t)(80 - 1));
	LCD_CS_CLR();
	for (i = 0; i < 160; i++)
	{
		for (j = 0; j < 80; j++)
		{
			SPI_Send(c >> 8);
			SPI_Send(c & 0xff);
		}
	}
	LCD_CS_SET();
}

#define WHITE       0xFFFF
#define BLACK       0x0000	  
#define BLUE        0x001F  
#define BRED        0XF81F
#define GRED 		0XFFE0
#define GBLUE		0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 		0XBC40
#define BRRED 		0XFC07
#define GRAY  		0X8410
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define LIGHTGREEN  0X841F
#define LGRAY 		0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12
#define DARKGRAY 	0XAD55
#define DODERBLUE	0x1C9F

static void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t c)
{
	LCD_Set_Address(x, y, x + w - 1, y + h - 1);
	LCD_CS_CLR();
	for (uint8_t __data i = 0; i < w; i++)
	{
		for (uint8_t __data j = 0; j < h; j++)
		{
			SPI_Send(c >> 8);
			SPI_Send(c & 0xff);
		}
	}
	LCD_CS_SET();
}
static const uint8_t ASC16_0_9[] =
{
	0x00, 0x00, 0x38, 0x6c, 0xc6, 0xc6, 0xd6, 0xd6, 0xc6, 0xc6, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00, // 0
	0x00, 0x00, 0x18, 0x38, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00, // 1
	0x00, 0x00, 0x7c, 0xc6, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0xc6, 0xfe, 0x00, 0x00, 0x00, 0x00, // 2
	0x00, 0x00, 0x7c, 0xc6, 0x06, 0x06, 0x3c, 0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, // 3
	0x00, 0x00, 0x0c, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x0c, 0x1e, 0x00, 0x00, 0x00, 0x00, // 4
	0x00, 0x00, 0xfe, 0xc0, 0xc0, 0xc0, 0xfc, 0x06, 0x06, 0x06, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, // 5
	0x00, 0x00, 0x38, 0x60, 0xc0, 0xc0, 0xfc, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, // 6
	0x00, 0x00, 0xfe, 0xc6, 0x06, 0x06, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, // 7
	0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00, // 8
	0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7e, 0x06, 0x06, 0x06, 0x0c, 0x78, 0x00, 0x00, 0x00, 0x00, // 9
};
static void drawPixel(uint8_t x, uint8_t y, uint16_t c)
{
	LCD_Set_Address(x, y, x, y);
	LCD_CS_CLR();
	SPI_Send(c >> 8);
	SPI_Send(c & 0xff);
	LCD_CS_SET();
}
static void drawASC16(uint8_t x, uint8_t y, uint8_t w, uint16_t c)
{
	w -= '0';
	for (k = 0; k < 16; k++)
	{
		for (n = 0; n < 8; n++)
		{
			drawPixel(x + n, y + k, ASC16_0_9[w * 16 + k] & (0x80 >> n) ? c : BLACK);
		}
	}
}

#define BAR_X 8
#define BAR_Y 72
#define BAR_W 64
#define BAR_H 8
static inline void drawProg(uint8_t step)
{
	drawBox(BAR_X + step / 2, BAR_Y, 1, BAR_H, DODERBLUE);
}

static float __data sum;

#define conv2d_s1_p0(input, size_input, kernel, size_kernel, bias, output, size_output) \
{ \
    for (x = 0; x < size_output; ++x) \
    { \
        for (y = 0; y < size_output; ++y) \
        { \
            sum = 0; \
            for (i = 0; i < size_kernel; ++i) \
            { \
                for (j = 0; j < size_kernel; ++j) \
                { \
                    sum += input[(x + i) * size_input + y + j] * kernel[i * size_kernel + j]; \
                } \
            } \
            output[x * size_output + y] = sum + bias; \
        } \
    } \
}

static uint8_t bitmap[4][28];
#define bitmap_set(x, y) bitmap[x / 8][y] |= (uint8_t)1 << (x % 8)
#define bitmap_clr(x, y) bitmap[x / 8][y] &= ~((uint8_t)1 << (x % 8))
#define bitmap_get(x, y) (bitmap[x / 8][y] & ((uint8_t)1 << (x % 8)))
static int8_t __data center_dx, center_dy;
static void calcCenter(void)
{
	uint8_t __data maxx = 0;
	uint8_t __data maxy = 0;
	uint8_t __data minx = 27;
	uint8_t __data miny = 27;
	for (x = 0; x < 28; ++x)
	{
		for (y = 0; y < 28; ++y)
		{
			if (bitmap_get(x, y) > 0)
			{
				if (x > maxx)
					maxx = x;
				if (y > maxy)
					maxy = y;
				if (x < minx)
					minx = x;
				if (y < miny)
					miny = y;
			}
		}
	}
	center_dx = (maxx + minx) / 2 - 14;
	center_dy = (maxy + miny) / 2 - 14;
}
#define conv2d_s1_p0_bitmap(size_input, kernel, size_kernel, bias, output, size_output) \
{ \
    for (x = 0; x < size_output; ++x) \
    { \
        for (y = 0; y < size_output; ++y) \
        { \
            sum = 0; \
            for (i = 0; i < size_kernel; ++i) \
            { \
                for (j = 0; j < size_kernel; ++j) \
                { \
                    const int8_t __data _x = x + center_dx + i; \
                    const int8_t __data _y = y + center_dy + j; \
                    const float __data input = (0 <= _x && x < 28 && 0 <= _y && _y < 28) ? \
                    (bitmap_get((uint8_t)_x, (uint8_t)_y) > 0 ? 1.0f : 0.0f) : 0.0f; \
                    sum += input * kernel[i * size_kernel + j]; \
                } \
            } \
            output[x * size_output + y] = sum + bias; \
        } \
    } \
}

#define relu(input, size_input) \
{ \
    for (x = 0; x < size_input; ++x) \
    { \
        for (y = 0; y < size_input; ++y) \
        { \
            if (input[x * size_input + y] < 0) \
                input[x * size_input + y] = 0; \
        } \
    } \
}

static inline float max(float a, float b) { return a > b ? a : b; }
#define max_pool2d_2x2(input, size_input, output, size_output) \
{ \
    for (x = 0; x < size_output; ++x) \
    { \
        for (y = 0; y < size_output; ++y) \
        { \
            const float __data _00 = input[(x * 2 + 0) * size_input + (y * 2 + 0)]; \
            const float __data _10 = input[(x * 2 + 1) * size_input + (y * 2 + 0)]; \
            const float __data _01 = input[(x * 2 + 0) * size_input + (y * 2 + 1)]; \
            const float __data _11 = input[(x * 2 + 1) * size_input + (y * 2 + 1)]; \
            output[x * size_output + y] = max(_00, max(_10, max(_01, _11))); \
        } \
    } \
}

#define softmax(input, total_input, output) \
{ \
    sum = 0; \
    for (i = 0; i < total_input; ++i) \
    { \
        const float __data x = expf(input[i]); \
        input[i] = x; \
        sum += x; \
    } \
    for (i = 0; i < total_input; ++i) \
    { \
        output[i] = (uint8_t)(input[i] / sum * 100); \
    } \
}

#define argmax(input, total_input, idx) \
{ \
    uint8_t __data maxx = 0; \
    for (i = 0; i < total_input; ++i) \
    { \
        if (input[i] > maxx) \
        { \
            maxx = input[i]; \
            idx = i; \
        } \
    } \
}

struct Layer_1
{
	float weight[8][5][5];
	float bias[8];
};
struct Layer_2
{
	float weight[16][8][5][5];
	float bias[16];
};
struct Layer_3
{
	float weight[10][256];
	float bias[10];
};
static const struct Layer_1* __data l1;
static const struct Layer_2* __data l2;
static const struct Layer_3* __data l3;

static const float* __data kernel;
static const float* __data input;
static float* __data output;
static float __data bias;
static uint8_t num;
static uint8_t prob[10];

static void mnist(void)
{
	float tensor1[16][8 * 8];
	for (i = 0; i < 16; ++i)
	{
		for (j = 0; j < 8 * 8; ++j)
		{
			tensor1[i][j] = l2->bias[i];
		}
	}

	for (k = 0; k < 8; ++k)
	{
		float tensor[12 * 12];
		{
			float t[24 * 24];
			kernel = &l1->weight[k][0][0];
			bias = l1->bias[k];
			output = &t[0];
			conv2d_s1_p0_bitmap(28, kernel, 5, bias, output, 24);
			relu(t, 24);
			output = &tensor[0];
			max_pool2d_2x2(t, 24, output, 12);
		}
		for (n = 0; n < 16; ++n)
		{
			float t[8 * 8];
			input = &tensor[0];
			kernel = &l2->weight[n][k][0][0];
			output = &t[0];
			conv2d_s1_p0(input, 12, kernel, 5, 0, output, 8);
			for (i = 0; i < 8 * 8; ++i)
			{
				tensor1[n][i] += t[i];
			}

			drawProg(k * 16 + n);
		}
	}

	float tensor2[10];
	for (i = 0; i < 10; ++i)
	{
		tensor2[i] = l3->bias[i];
	}

	for (i = 0; i < 16; ++i)
	{
		float t[4 * 4];
		relu(tensor1[i], 8);
		output = &t[0];
		max_pool2d_2x2(tensor1[i], 8, output, 4);
		for (j = 0; j < 4 * 4; ++j)
		{
			for (k = 0; k < 10; ++k)
			{
				tensor2[k] += t[j] * l3->weight[k][i * 16 + j];
			}
		}
	}

	softmax(tensor2, 10, prob);
	argmax(prob, 10, num);
}

static inline void drawBar(void)
{
	drawBox(BAR_X, BAR_Y, BAR_W, BAR_H, GRAY);
}
static void drawProb(void)
{
	y = 0;
	for (i = 0; i <= 4; ++i)
	{
		const uint8_t _1 = prob[i] / 10;
		const uint8_t _0 = prob[i] - _1 * 10;
		const uint16_t c = i == num ? GREEN : DARKGRAY;
		drawASC16(80 + 8 + 8 + 0, y, _1 + '0', c);
		drawASC16(80 + 8 + 8 + 8, y, _0 + '0', c);
		y += 16;
	}
	y = 0;
	for (i = 5; i <= 9; ++i)
	{
		const uint8_t _1 = prob[i] / 10;
		const uint8_t _0 = prob[i] - _1 * 10;
		const uint16_t c = i == num ? GREEN : DARKGRAY;
		drawASC16(120u + 8u + 8u + 0u, y, _1 + '0', c);
		drawASC16(120u + 8u + 8u + 8u, y, _0 + '0', c);
		y += 16;
	}
}

#define BOUND_X 12
#define BOUND_Y 8

static int8_t pen_x;
static int8_t pen_y;
static int8_t pen_w;
static uint8_t draw;
#define MOVE_PEN(dx, dy, a, b) \
	pen_x += dx; if (pen_x < a) pen_x = a; else if (pen_x > b) pen_x = b; \
	pen_y += dy; if (pen_y < a) pen_y = a; else if (pen_y > b) pen_y = b;

static void movePen(int8_t dx, int8_t dy)
{
	if (draw)
	{
		if (pen_w == 3)
		{
			MOVE_PEN(dx, dy, 1, 26);
			for (i = 0; i < 3; ++i)
			{
				const int8_t x = pen_x - 1 + i;
				for (j = 0; j < 3; ++j)
				{
					const int8_t y = pen_y - 1 + j;
					drawBox(BOUND_X + x * 2, BOUND_Y + y * 2, 2, 2, YELLOW);
					bitmap_set(y, x);
				}
			}
		}
		else if (pen_w == 2)
		{
			MOVE_PEN(dx, dy, 0, 26);
			for (i = 0; i < 2; ++i)
			{
				const int8_t x = pen_x + i;
				for (j = 0; j < 2; ++j)
				{
					const int8_t y = pen_y + j;
					drawBox(BOUND_X + x * 2, BOUND_Y + y * 2, 2, 2, YELLOW);
					bitmap_set(y, x);
				}
			}
		}
		else
		{
			MOVE_PEN(dx, dy, 0, 27);
			const int8_t x = pen_x;
			const int8_t y = pen_y;
			drawBox(BOUND_X + x * 2, BOUND_Y + y * 2, 2, 2, YELLOW);
			bitmap_set(y, x);
		}
	}
	else
	{
		drawBox(BOUND_X + pen_x * 2, BOUND_Y + pen_y * 2, 2, 2, bitmap_get(pen_y, pen_x) > 0 ? YELLOW : GRAY);
		MOVE_PEN(dx, dy, 0, 27);
		drawBox(BOUND_X + pen_x * 2, BOUND_Y + pen_y * 2, 2, 2, RED);
	}
}
static void initPen(void)
{
	pen_x = 28 / 2;
	pen_y = 28 / 2;
	pen_w = 2;
	draw = 0;
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 28; ++j)
		{
			bitmap[i][j] = 0;
		}
	}
	drawBox(BAR_X, BAR_Y, BAR_W, BAR_H, GRAY);
	drawBox(BOUND_X, BOUND_Y, 28 * 2, 28 * 2, GRAY);
	movePen(0, 0);
}

void main(void)
{
	P0M0 = 0x00;
	P0M1 = 0x00;
	P1M0 = 0x00;
	P1M1 = 0x00;
	P2M0 = 0x00;
	P2M1 = 0x00;
	P3M0 = 0x00;
	P3M1 = 0x00;
	P4M0 = 0x00;
	P4M1 = 0x00;
	P5M0 = 0x00;
	P5M1 = 0x00;
	P6M0 = 0x00;
	P6M1 = 0x00;
	P7M0 = 0x00;
	P7M1 = 0x00;

	P_SW2 = 0x80;
	PWMA_CCER1 = 0x00;
	PWMA_CCMR1 = 0x60;
	PWMA_CCER1 = 0x01;
	PWMA_CCR1 = 10;
	PWMA_ARR = 100;
	PWMA_ENO = 0x01;
	PWMA_BKR = 0x80;
	PWMA_CR1 = 0x01;

#if SPI
	P_SW1 = 0x00;
	SPCTL = 0xD0;
	SPSTAT = 0xC0;
#endif

	LCD_Init();
	Delay100ms();
	LCD_Clear(BLACK);
	Delay100ms();
	LCD_WR_REG(0x29);

	UartInit();

	//printf_small("8051 MNIST\r\n");

#if SPI
	P1SR &= ~0x28;
	P3SR &= ~0xe0;
	SPCTL = 0xD3;
#endif

	l1 = (const struct Layer_1*)((uint8_t*)model_mnist + 0);
	l2 = (const struct Layer_2*)((uint8_t*)model_mnist + sizeof(struct Layer_1));
	l3 = (const struct Layer_3*)((uint8_t*)model_mnist + sizeof(struct Layer_1) + sizeof(struct Layer_2));

	y = 0;
	for (i = '0'; i <= '4'; ++i)
	{
		drawASC16(80, y, i, BRED);
		y += 16;
	}
	y = 0;
	for (i = '5'; i <= '9'; ++i)
	{
		drawASC16(120, y, i, BRED);
		y += 16;
	}

	drawProb();

	initPen();

	while (1)
	{
		if (RI)
		{
			const uint8_t c = SBUF;
			if (c == 'a') movePen(-1, 0); // left
			else if (c == 'd') movePen(1, 0); // right
			else if (c == 'w') movePen(0, -1); // up
			else if (c == 's') movePen(0, 1); // down
			else if (c == ' ') { draw = ~draw; movePen(0, 0); } // move/draw
			else if (c == 27) initPen(); // reset
			else if (c == '\r') { calcCenter(); drawBar(); mnist(); drawProb(); } // run
			else if (c == '1') { pen_w = 1; } // line width 1
			else if (c == '2') { pen_w = 2; } // line width 2
			else if (c == '3') { pen_w = 3; } // line width 3
			RI = 0;
		}
	}
}
