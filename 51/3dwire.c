/*
* STC8H8K64U-45I-PDIP40 @24MHz
* OLED12864 SSD1306/SH1106
* sdcc 3dwire.c --model-large
* https://liuliqiang.com/51/3dwire.c
* by liuliqiang 2025-02-13
*/

#include <8051.h>
#include <stdint.h>
#include <math.h>

#define SPI 1
#define DMA 1

#define XOFFSET 2 // SSD1306:0 SH1106:2

#define OLED_CLK	P1_5
#define OLED_MOSI	P1_3
#define OLED_RES	P1_4
#define OLED_DC		P1_6
#define OLED_CS		P1_7

#define OLED_CS_CLR()	OLED_CS = 0
#define OLED_CS_SET()	OLED_CS = 1

#define OLED_RES_CLR()	OLED_RES = 0
#define OLED_RES_SET()	OLED_RES = 1

#define OLED_DC_CLR()	OLED_DC = 0
#define OLED_DC_SET()	OLED_DC = 1

#define OLED_CLK_CLR()	OLED_CLK = 0
#define OLED_CLK_SET()	OLED_CLK = 1

#define OLED_MOSI_CLR()	OLED_MOSI = 0
#define OLED_MOSI_SET()	OLED_MOSI = 1

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

#define DMA_M2M_CFG		(*(uint8_t volatile __xdata *)0xFA00)
#define DMA_M2M_CR		(*(uint8_t volatile __xdata *)0xFA01)
#define DMA_M2M_STA		(*(uint8_t volatile __xdata *)0xFA02)
#define DMA_M2M_AMT		(*(uint8_t volatile __xdata *)0xFA03)
#define DMA_M2M_DONE	(*(uint8_t volatile __xdata *)0xFA04)
#define DMA_M2M_TXAH	(*(uint8_t volatile __xdata *)0xFA05)
#define DMA_M2M_TXAL	(*(uint8_t volatile __xdata *)0xFA06)
#define DMA_M2M_RXAH	(*(uint8_t volatile __xdata *)0xFA07)
#define DMA_M2M_RXAL	(*(uint8_t volatile __xdata *)0xFA08)

#define DMA_SPI_CFG		(*(uint8_t volatile __xdata *)0xFA20)
#define DMA_SPI_CR		(*(uint8_t volatile __xdata *)0xFA21)
#define DMA_SPI_STA		(*(uint8_t volatile __xdata *)0xFA22)
#define DMA_SPI_AMT		(*(uint8_t volatile __xdata *)0xFA23)
#define DMA_SPI_DONE	(*(uint8_t volatile __xdata *)0xFA24)
#define DMA_SPI_TXAH	(*(uint8_t volatile __xdata *)0xFA25)
#define DMA_SPI_TXAL	(*(uint8_t volatile __xdata *)0xFA26)
#define DMA_SPI_RXAH	(*(uint8_t volatile __xdata *)0xFA27)
#define DMA_SPI_RXAL	(*(uint8_t volatile __xdata *)0xFA28)
#define DMA_SPI_CFG2	(*(uint8_t volatile __xdata *)0xFA29)

#define W 128
#define H 64

#define OLED_CMD	0
#define OLED_DATA	1

static void Delay100ms(void)	//@24.000MHz
{
	unsigned char __data i, j, k;

	__asm__("nop");
	__asm__("nop");
	i = 13;
	j = 45;
	k = 214;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

static void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
		OLED_DC_SET();
	else
		OLED_DC_CLR();
	OLED_CS_CLR();
#if SPI
	SPDAT = dat;
	while (!(SPSTAT & 0x80));
	SPSTAT = 0xC0;
#else
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (dat & 0x80)
		{
			OLED_MOSI_SET();
		}
		else
		{
			OLED_MOSI_CLR();
		}
		OLED_CLK_CLR();
		OLED_CLK_SET();
		dat <<= 1;
	}
#endif
	OLED_CS_SET();
}
static inline void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xB0 + y, OLED_CMD);
	OLED_WR_Byte((((x + XOFFSET) & 0xF0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte(((x + XOFFSET) & 0x0F), OLED_CMD);
}
static inline void OLED_Clear(void)
{
	for (uint8_t y = 0; y < 8; y++)
	{
		OLED_Set_Pos(0, y);
		for (uint8_t x = 0; x < 128; x++)
			OLED_WR_Byte(0, OLED_DATA);
	}
}
static inline void OLED_Init(void)
{
	OLED_RES_SET();
	Delay100ms();
	OLED_RES_CLR();
	Delay100ms();
	OLED_RES_SET();

	OLED_WR_Byte(0xD5, OLED_CMD);
	OLED_WR_Byte(0xF0, OLED_CMD);
	OLED_WR_Byte(0x8D, OLED_CMD);
	OLED_WR_Byte(0x14, OLED_CMD);
	OLED_WR_Byte(0xA1, OLED_CMD);
	OLED_WR_Byte(0xC8, OLED_CMD);
	OLED_WR_Byte(0x81, OLED_CMD);
	OLED_WR_Byte(0xCF, OLED_CMD);
	OLED_WR_Byte(0xD9, OLED_CMD);
	OLED_WR_Byte(0xF1, OLED_CMD);
	OLED_WR_Byte(0xAF, OLED_CMD);
	OLED_WR_Byte(0x20, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD);

	OLED_Clear();
}

static uint8_t buffer[1024 + DMA];

static inline void buffer_clear(void)
{
#if DMA
	for (uint8_t i = 0; i < 4; ++i)
	{
		uint16_t idx = 1023 - i * 256;
		uint8_t* src = &buffer[idx + 1];
		uint8_t* dst = &buffer[idx];
		DMA_M2M_TXAH = ((uint16_t)src) >> 8;
		DMA_M2M_TXAL = ((uint16_t)src) & 0xFF;
		DMA_M2M_RXAH = ((uint16_t)dst) >> 8;
		DMA_M2M_RXAL = ((uint16_t)dst) & 0xFF;
		DMA_M2M_CR = 0xC0;
		while (!(DMA_M2M_STA & 0x01));
		DMA_M2M_STA = 0;
	}
#else
	for (uint16_t i = 0; i < 1024; i++)
		buffer[i] = 0;
#endif
}
static inline void buffer_display(void)
{
	uint16_t j = 0;
	for (uint8_t y = 0; y < 8; y++)
	{
		OLED_Set_Pos(0, y);
#if SPI & DMA
		OLED_DC_SET();
		OLED_CS_CLR();
		uint8_t* src = &buffer[j];
		DMA_SPI_TXAH = ((uint16_t)src) >> 8;
		DMA_SPI_TXAL = ((uint16_t)src) & 0xFF;
		DMA_SPI_CR = 0xC0;
		while (!(DMA_SPI_STA & 0x01));
		DMA_SPI_STA = 0;
		SPSTAT = 0xC0;
		OLED_CS_SET();
		j += 128;
#else
		for (uint8_t x = 0; x < 128; x++)
			OLED_WR_Byte(buffer[j++], OLED_DATA);
#endif
	}
}
static void buffer_drawpixel(int16_t x, int16_t y)
{
	if (x < 0 || x > W - 1 || y < 0 || y > H - 1)
		return;
	const uint8_t row = (uint8_t)y / 8;
	buffer[(row * W) + (uint8_t)x] |= 1 << ((uint8_t)y % 8);
}

#define drawpixel buffer_drawpixel

static inline int16_t abs(int16_t x)
{
	return x > 0 ? x : -x;
}
static void drawline(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	for (;;) {  /* loop */
		drawpixel(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}

struct vec2i_t
{
	int16_t x, y;
};

struct vec2_t
{
	float x, y;
};

struct vec3_t
{
	union
	{
		struct
		{
			float x, y, z;
		};
		float v[3];
	};
};

struct vec4_t
{
	union
	{
		struct
		{
			float x, y, z, w;
		};
		float v[4];
	};
};

struct mat4_t
{
	float m[4][4];
};

#define RADIANS(x) ((3.1415926f / 180) * (x))

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif			 

#ifndef min		 
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

static uint8_t k, l, n;
#define mat4_mul_mat4(r, a, b) \
{ \
	for (k = 0; k < 4; ++k) \
	{ \
		for (l = 0; l < 4; ++l) \
		{ \
			r.m[k][l] = 0; \
			for (n = 0; n < 4; ++n) \
			{ \
				r.m[k][l] += a.m[n][l] * b.m[k][n]; \
			} \
		} \
	} \
}

#define mat4_mul_vec4(r, a, b) \
{ \
	for (k = 0; k < 4; ++k) \
	{ \
		r.v[k] = 0; \
		for (l = 0; l < 4; ++l) \
		{ \
			r.v[k] += a.m[l][k] * b.v[l]; \
		} \
	} \
}

#define dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)

#define cross(r, a, b) \
{ \
	r.x = a.y * b.z - a.z * b.y; \
	r.y = a.z * b.x - a.x * b.z; \
	r.z = a.x * b.y - a.y * b.x; \
}

#define normalize(v) \
{ \
	const float r = sqrtf(dot(v, v)); \
	if (r > 0.0f) \
	{ \
		v.x /= r; \
		v.y /= r; \
		v.z /= r; \
	} \
}

#define vec3_sub(r, a, b) \
{ \
	r.x = a.x - b.x; \
	r.y = a.y - b.y; \
	r.z = a.z - b.z; \
}

#define perspective(proj, fovy, aspect, znear, zfar) \
{ \
	const float cot = 1.0f / tanf(fovy / 2.0f); \
	const float sum = (znear + zfar); \
	const float sub = (znear - zfar); \
 \
	proj.m[0][0] = cot / aspect; \
	proj.m[0][1] = 0.0f; \
	proj.m[0][2] = 0.0f; \
	proj.m[0][3] = 0.0f; \
 \
	proj.m[1][0] = 0.0f; \
	proj.m[1][1] = cot; \
	proj.m[1][2] = 0.0f; \
	proj.m[1][3] = 0.0f; \
 \
	proj.m[2][0] = 0.0f;\
	proj.m[2][1] = 0.0f;\
	proj.m[2][2] = sum / sub; \
	proj.m[2][3] = -1.0f; \
 \
	proj.m[3][0] = 0.0f; \
	proj.m[3][1] = 0.0f; \
	proj.m[3][2] = 2.0f * znear * zfar / sub; \
	proj.m[3][3] = 0.0f; \
}

#define lookat(view, eye, center, up) \
{ \
	struct vec3_t zaxis; \
	vec3_sub(zaxis, eye, center); \
	normalize(zaxis); \
 \
	struct vec3_t xaxis; \
	cross(xaxis, up, zaxis); \
	normalize(xaxis); \
 \
	struct vec3_t yaxis; \
	cross(yaxis, zaxis, xaxis); \
 \
	view.m[0][0] = xaxis.x; \
	view.m[0][1] = yaxis.x; \
	view.m[0][2] = zaxis.x; \
	view.m[0][3] = 0.0f; \
 \
	view.m[1][0] = xaxis.y; \
	view.m[1][1] = yaxis.y; \
	view.m[1][2] = zaxis.y; \
	view.m[1][3] = 0.0f; \
 \
	view.m[2][0] = xaxis.z; \
	view.m[2][1] = yaxis.z; \
	view.m[2][2] = zaxis.z; \
	view.m[2][3] = 0.0f; \
 \
	view.m[3][0] = -dot(xaxis, eye); \
	view.m[3][1] = -dot(yaxis, eye); \
	view.m[3][2] = -dot(zaxis, eye); \
	view.m[3][3] = 1.0f; \
}

#define rotate(r, a, x, y, z) \
{ \
	const float c = cosf(a); \
	const float s = sinf(a); \
	const float onec = 1.0f - c; \
 \
	r.m[0][0] = x * x * onec + c; \
	r.m[0][1] = y * x * onec + z * s; \
	r.m[0][2] = x * z * onec - y * s; \
	r.m[0][3] = 0.0f; \
 \
	r.m[1][0] = x * y * onec - z * s; \
	r.m[1][1] = y * y * onec + c; \
	r.m[1][2] = y * z * onec + x * s; \
	r.m[1][3] = 0.0f; \
 \
	r.m[2][0] = x * z * onec + y * s; \
	r.m[2][1] = y * z * onec - x * s; \
	r.m[2][2] = z * z * onec + c; \
	r.m[2][3] = 0.0f; \
 \
	r.m[3][0] = 0.0f; \
	r.m[3][1] = 0.0f; \
	r.m[3][2] = 0.0f; \
	r.m[3][3] = 1.0f; \
}

#define X 12
#define Y 12
static struct vec4_t vertex[X][Y];
static struct vec2i_t point[X][Y];
static struct mat4_t vp;

#define func(x, y) sinf(x*y)
//#define func(x, y) 2.0f*cosf(sqrtf((x*x + y*y)))
//#define func(x, y) 2.5f*expf(-0.5f*(x*x + y*y))

static uint8_t i, j;
static void precompute(void)
{
	{
		const float fromX = -2.5f;
		const float toX = 2.5f;
		const float stepX = (toX - fromX) / X;
		const float fromY = -2.5f;
		const float toY = 2.5f;
		const float stepY = (toY - fromY) / Y;
		for (i = 0; i < X; ++i)
		{
			for (j = 0; j < Y; ++j)
			{
				const float x = fromX + stepX * i;
				const float y = fromY + stepY * j;
				const float z = func(x, y);
				vertex[i][j].x = x;
				vertex[i][j].y = z;
				vertex[i][j].z = y;
				vertex[i][j].w = 1.0f;
			}
		}
	}
	{
		struct mat4_t proj;
		{
			const float fovy = RADIANS(45.0f);
			const float aspect = (float)W / H;
			const float znear = 0.1f;
			const float zfar = 10.0f;
			perspective(proj, fovy, aspect, znear, zfar);
		}

		struct mat4_t view;
		{
			struct vec3_t eye;
			eye.x = 5.0f;
			eye.y = 5.0f;
			eye.z = 5.0f;
			struct vec3_t center;
			center.x = 0.0f;
			center.y = 0.0f;
			center.z = 0.0f;
			struct vec3_t up;
			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;
			lookat(view, eye, center, up);
		}

		mat4_mul_mat4(vp, proj, view);
	}
}

static float angle;
static void draw3d(void)
{
	struct mat4_t mvp;
	{
		struct mat4_t r;
		const float a = RADIANS(angle);
		const float x = 0.0f;
		const float y = 1.0f;
		const float z = 0.0f;
		rotate(r, a, x, y, z);

		mat4_mul_mat4(mvp, vp, r);
	}

	for (i = 0; i < X; ++i)
	{
		for (j = 0; j < Y; ++j)
		{
			struct vec4_t position;
			mat4_mul_vec4(position, mvp, vertex[i][j]);
			const float rhw = 1.0f / (position.w + 0.00001f);
			point[i][j].x = (position.x * rhw + 1.0f) * W * 0.5f;
			point[i][j].y = (1.0f - position.y * rhw) * H * 0.5f;
		}
	}

	for (j = 0; j < Y; ++j)
	{
		for (i = 0; i < X - 1; ++i)
		{
			drawline(point[i][j].x, point[i][j].y, point[i + 1][j].x, point[i + 1][j].y);
		}
	}

	for (i = 0; i < X; ++i)
	{
		for (j = 0; j < Y - 1; ++j)
		{
			drawline(point[i][j].x, point[i][j].y, point[i][j + 1].x, point[i][j + 1].y);
		}
	}
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

#if SPI
	P_SW1 = 0;
	SPCTL = 0xD3;
	SPSTAT = 0xC0;
#endif
#if DMA
	P_SW2 |= 0x80;
	DMA_M2M_CFG = 0x30;
	DMA_M2M_AMT = 0xFF;
	buffer[1024] = 0;
#if SPI
	DMA_SPI_CFG = 0x40;
	DMA_SPI_AMT = 127;
#endif
#endif

	OLED_Init();

	angle = 0;
	precompute();

	while (1)
	{
		buffer_clear();
		draw3d();
		//drawline(0, 0, 127, 63);
		//drawline(0, 63, 127, 0);
		buffer_display();
		angle += 2;
	}
}
