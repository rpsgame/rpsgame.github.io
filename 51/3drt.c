/*
* STC8H8K64U-45I-PDIP40 @40MHz
* LCD16080 ST7735
* sdcc 3drt.c --model-large
* https://liuliqiang.com/51/3drt.c
* by liuliqiang 2025-03-23
*/

#include <8051.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

#define SPI 1

#define spp 1
#define max_depth 2

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

	Delay100ms();
	LCD_WR_REG(0x29);
}

static uint8_t i, j, k, l, n;

struct vec3_t
{
    float x, y, z;
};

struct mat4_t
{
    float m[4][4];
};

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif			 

#ifndef min		 
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define vec3_assign(r, v) \
{ \
    r.x = v.x; \
    r.y = v.y; \
    r.z = v.z; \
}
#define vec3_assign_s(v, a) \
{ \
    v.x = a; \
    v.y = a; \
    v.z = a; \
}
#define vec3_assign_s3(v, a, b, c) \
{ \
    v.x = a; \
    v.y = b; \
    v.z = c; \
}
#define vec3_div_assign_s(v, a) \
{ \
    v.x /= a; \
    v.y /= a; \
    v.z /= a; \
}
#define vec3_mul_assign_s(v, a) \
{ \
    v.x *= a; \
    v.y *= a; \
    v.z *= a; \
}
#define vec3_div_assign(v1, v2) \
{ \
    v1.x /= v2.x; \
    v1.y /= v2.y; \
    v1.z /= v2.z; \
}
#define vec3_mul_assign(v1, v2) \
{ \
    v1.x *= v2.x; \
    v1.y *= v2.y; \
    v1.z *= v2.z; \
}
#define vec3_add_assign(v1, v2) \
{ \
    v1.x += v2.x; \
    v1.y += v2.y; \
    v1.z += v2.z; \
}
#define vec3_mad_assign(v1, v2, v3) \
{ \
    v1.x += v2.x * v3.x; \
    v1.y += v2.y * v3.y; \
    v1.z += v2.z * v3.z; \
}
#define vec3_mad(r, v1, v2, v3) \
{ \
    r.x = v1.x * v2.x + v3.x; \
    r.y = v1.y * v2.y + v3.y; \
    r.z = v1.z * v2.z + v3.z; \
}
#define vec3_mad_s(r, v1, v2, a) \
{ \
    r.x = v1.x + v2.x * a; \
    r.y = v1.y + v2.y * a; \
    r.z = v1.z + v2.z * a; \
}
#define vec3_add(r, v1, v2) \
{ \
    r.x = v1.x + v2.x; \
    r.y = v1.y + v2.y; \
    r.z = v1.z + v2.z; \
}
#define vec3_mul(r, v1, v2) \
{ \
    r.x = v1.x * v2.x; \
    r.y = v1.y * v2.y; \
    r.z = v1.z * v2.z; \
}
#define vec3_mul_s(r, v, a) \
{ \
    r.x = v.x * a; \
    r.y = v.y * a; \
    r.z = v.z * a; \
}
#define vec3_mul_assign_s(v, a) \
{ \
    v.x *= a; \
    v.y *= a; \
    v.z *= a; \
}
#define vec3_rcp(r, v) \
{ \
    r.x = 1.0f / v.x; \
    r.y = 1.0f / v.y; \
    r.z = 1.0f / v.z; \
}
#define vec3_sub(r, v1, v2) \
{ \
	r.x = v1.x - v2.x; \
	r.y = v1.y - v2.y; \
	r.z = v1.z - v2.z; \
}
#define vec3_min(r, v1, v2) \
{ \
    r.x = min(v1.x, v2.x); \
    r.y = min(v1.y, v2.y); \
    r.z = min(v1.z, v2.z); \
}
#define vec3_max(r, v1, v2) \
{ \
    r.x = max(v1.x, v2.x); \
    r.y = max(v1.y, v2.y); \
    r.z = max(v1.z, v2.z); \
}

#define vec3_compMax(v) (max(v.x, max(v.y, v.z)))
#define vec3_compMin(v) (min(v.x, min(v.y, v.z)))

#define dot(v1, v2) (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z)
#define dot_m(v1, v2) (-v1.x * v2.x + -v1.y * v2.y + -v1.z * v2.z)

#define cross(r, v1, v2) \
{ \
	r.x = v1.y * v2.z - v1.z * v2.y; \
	r.y = v1.z * v2.x - v1.x * v2.z; \
	r.z = v1.x * v2.y - v1.y * v2.x; \
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

#define vec3_minus(r, v) \
{ \
    r.x = -v.x; \
    r.y = -v.y; \
    r.z = -v.z; \
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

static struct mat4_t T;
#define orthonormalBasis(N) \
{ \
    const float sign = N.z > 0.0f ? 1.0f : -1.0f; \
    const float a = -1.0f / (sign + N.z); \
    const float b = N.x * N.y * a; \
 \
    T.m[0][0] = 1.0f + sign * N.x * N.x * a; \
    T.m[0][1] = sign * b; \
    T.m[0][2] = -sign * N.x; \
    T.m[0][3] = 0.0f; \
 \
    T.m[1][0] = b; \
    T.m[1][1] = sign + N.y * N.y * a; \
    T.m[1][2] = -N.y; \
    T.m[1][3] = 0.0f; \
 \
    T.m[2][0] = N.x; \
    T.m[2][1] = N.y; \
    T.m[2][2] = N.z; \
    T.m[2][3] = 0.0f; \
}

#define radians(x) ((x) * 0.017453f)

static const float __code triangles[] =
{
    -0.240000f, 1.980000f, -0.220000f, 0.230000f, 1.980000f, 0.160000f,
    -0.240000f, 1.980000f, -0.220000f, 0.230000f, 1.980000f, 0.160000f, -0.240000f, 1.980000f, 0.160000f,
    0.470000f, 0.000000f, 0.380000f, 0.000000f, 0.000000f, 0.380000f, 0.000000f, -1.000000f, 0.000000f,
    -0.240000f, 1.980000f, -0.220000f, 0.230000f, 1.980000f, 0.160000f,
    -0.240000f, 1.980000f, -0.220000f, 0.230000f, 1.980000f, -0.220000f, 0.230000f, 1.980000f, 0.160000f,
    0.470000f, 0.000000f, 0.000000f, 0.470000f, 0.000000f, 0.380000f, 0.000000f, -1.000000f, 0.000000f,
    -1.010000f, 0.000000f, -1.040000f, 1.000000f, 0.000000f, 0.990000f,
    1.000000f, 0.000000f, 0.990000f, -0.990000f, -0.000000f, -1.040000f, -1.010000f, 0.000000f, 0.990000f,
    -1.990000f, -0.000000f, -2.030000f, -2.010000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f, -0.000000f,
    -1.020000f, 1.990000f, -1.040000f, 1.000000f, 1.990000f, 0.990000f,
    -1.020000f, 1.990000f, -1.040000f, 1.000000f, 1.990000f, 0.990000f, -1.020000f, 1.990000f, 0.990000f,
    2.020000f, 0.000000f, 2.030000f, 0.000000f, 0.000000f, 2.030000f, 0.000000f, -1.000000f, 0.000000f,
    -1.020000f, -0.000000f, -1.040000f, 1.000000f, 1.990000f, -1.040000f,
    -0.990000f, -0.000000f, -1.040000f, 1.000000f, 1.990000f, -1.040000f, -1.020000f, 1.990000f, -1.040000f,
    1.990000f, 1.990000f, 0.000000f, -0.030000f, 1.990000f, 0.000000f, 0.000000f, -0.000000f, 1.000000f,
    -0.990000f, 0.000000f, -1.040000f, 1.000000f, 0.000000f, 0.990000f,
    1.000000f, 0.000000f, 0.990000f, 1.000000f, -0.000000f, -1.040000f, -0.990000f, -0.000000f, -1.040000f,
    0.000000f, -0.000000f, -2.030000f, -1.990000f, -0.000000f, -2.030000f, 0.000000f, 1.000000f, -0.000000f,
    -1.020000f, 1.990000f, -1.040000f, 1.000000f, 1.990000f, 0.990000f,
    -1.020000f, 1.990000f, -1.040000f, 1.000000f, 1.990000f, -1.040000f, 1.000000f, 1.990000f, 0.990000f,
    2.020000f, 0.000000f, 0.000000f, 2.020000f, 0.000000f, 2.030000f, 0.000000f, -1.000000f, 0.000000f,
    -0.990000f, -0.000000f, -1.040000f, 1.000000f, 1.990000f, -1.040000f,
    -0.990000f, -0.000000f, -1.040000f, 1.000000f, -0.000000f, -1.040000f, 1.000000f, 1.990000f, -1.040000f,
    1.990000f, 0.000000f, 0.000000f, 1.990000f, 1.990000f, 0.000000f, 0.000000f, 0.000000f, 1.000000f,
    -1.020000f, 0.000000f, -1.040000f, -1.010000f, 1.990000f, 0.990000f,
    -1.010000f, 0.000000f, 0.990000f, -1.020000f, 1.990000f, -1.040000f, -1.020000f, 1.990000f, 0.990000f,
    -0.010000f, 1.990000f, -2.030000f, -0.010000f, 1.990000f, 0.000000f, 0.999987f, 0.005025f, 0.000000f,
    -1.020000f, 0.000000f, -1.040000f, -0.990000f, 1.990000f, 0.990000f,
    -1.010000f, 0.000000f, 0.990000f, -0.990000f, -0.000000f, -1.040000f, -1.020000f, 1.990000f, -1.040000f,
    0.020000f, -0.000000f, -2.030000f, -0.010000f, 1.990000f, -2.030000f, 0.999838f, 0.015073f, 0.009851f,
    1.000000f, 0.000000f, -1.040000f, 1.000000f, 1.990000f, 0.990000f,
    1.000000f, 0.000000f, 0.990000f, 1.000000f, 1.990000f, -1.040000f, 1.000000f, -0.000000f, -1.040000f,
    0.000000f, 1.990000f, -2.030000f, 0.000000f, -0.000000f, -2.030000f, -1.000000f, 0.000000f, -0.000000f,
    1.000000f, 0.000000f, -1.040000f, 1.000000f, 1.990000f, 0.990000f,
    1.000000f, 0.000000f, 0.990000f, 1.000000f, 1.990000f, 0.990000f, 1.000000f, 1.990000f, -1.040000f,
    0.000000f, 1.990000f, 0.000000f, 0.000000f, 1.990000f, -2.030000f, -1.000000f, 0.000000f, 0.000000f,
    -0.710000f, 1.200000f, -0.490000f, 0.040000f, 1.200000f, 0.090000f,
    0.040000f, 1.200000f, -0.090000f, -0.710000f, 1.200000f, -0.490000f, -0.530000f, 1.200000f, 0.090000f,
    -0.750000f, 0.000000f, -0.400000f, -0.570000f, 0.000000f, 0.180000f, 0.000000f, 1.000000f, 0.000000f,
    -0.710000f, -0.000000f, -0.490000f, -0.530000f, 1.200000f, 0.090000f,
    -0.530000f, 1.200000f, 0.090000f, -0.710000f, -0.000000f, -0.490000f, -0.530000f, 0.000000f, 0.090000f,
    -0.180000f, -1.200000f, -0.580000f, 0.000000f, -1.200000f, 0.000000f, -0.955064f, 0.000000f, 0.296399f,
    -0.710000f, -0.000000f, -0.670000f, -0.140000f, 1.200000f, -0.490000f,
    -0.710000f, 1.200000f, -0.490000f, -0.140000f, -0.000000f, -0.670000f, -0.710000f, -0.000000f, -0.490000f,
    0.570000f, -1.200000f, -0.180000f, 0.000000f, -1.200000f, 0.000000f, -0.301131f, -0.000000f, -0.953583f,
    -0.140000f, -0.000000f, -0.670000f, 0.040000f, 1.200000f, -0.090000f,
    -0.140000f, 1.200000f, -0.670000f, 0.040000f, -0.000000f, -0.090000f, -0.140000f, -0.000000f, -0.670000f,
    0.180000f, -1.200000f, 0.580000f, 0.000000f, -1.200000f, 0.000000f, 0.955064f, 0.000000f, -0.296399f,
    -0.530000f, 0.000000f, -0.090000f, 0.040000f, 1.200000f, 0.090000f,
    0.040000f, 1.200000f, -0.090000f, -0.530000f, 0.000000f, 0.090000f, 0.040000f, -0.000000f, -0.090000f,
    -0.570000f, -1.200000f, 0.180000f, 0.000000f, -1.200000f, 0.000000f, 0.301131f, 0.000000f, 0.953583f,
    -0.710000f, 1.200000f, -0.670000f, 0.040000f, 1.200000f, -0.090000f,
    0.040000f, 1.200000f, -0.090000f, -0.140000f, 1.200000f, -0.670000f, -0.710000f, 1.200000f, -0.490000f,
    -0.180000f, 0.000000f, -0.580000f, -0.750000f, 0.000000f, -0.400000f, 0.000000f, 1.000000f, 0.000000f,
    -0.710000f, -0.000000f, -0.490000f, -0.530000f, 1.200000f, 0.090000f,
    -0.530000f, 1.200000f, 0.090000f, -0.710000f, 1.200000f, -0.490000f, -0.710000f, -0.000000f, -0.490000f,
    -0.180000f, 0.000000f, -0.580000f, -0.180000f, -1.200000f, -0.580000f, -0.955064f, 0.000000f, 0.296399f,
    -0.710000f, -0.000000f, -0.670000f, -0.140000f, 1.200000f, -0.490000f,
    -0.710000f, 1.200000f, -0.490000f, -0.140000f, 1.200000f, -0.670000f, -0.140000f, -0.000000f, -0.670000f,
    0.570000f, 0.000000f, -0.180000f, 0.570000f, -1.200000f, -0.180000f, -0.301131f, 0.000000f, -0.953583f,
    -0.140000f, -0.000000f, -0.670000f, 0.040000f, 1.200000f, -0.090000f,
    -0.140000f, 1.200000f, -0.670000f, 0.040000f, 1.200000f, -0.090000f, 0.040000f, -0.000000f, -0.090000f,
    0.180000f, 0.000000f, 0.580000f, 0.180000f, -1.200000f, 0.580000f, 0.955064f, 0.000000f, -0.296399f,
    -0.530000f, 0.000000f, -0.090000f, 0.040000f, 1.200000f, 0.090000f,
    0.040000f, 1.200000f, -0.090000f, -0.530000f, 1.200000f, 0.090000f, -0.530000f, 0.000000f, 0.090000f,
    -0.570000f, 0.000000f, 0.180000f, -0.570000f, -1.200000f, 0.180000f, 0.301131f, 0.000000f, 0.953583f,
    -0.050000f, 0.600000f, -0.000000f, 0.530000f, 0.600000f, 0.750000f,
    0.530000f, 0.600000f, 0.750000f, 0.130000f, 0.600000f, -0.000000f, -0.050000f, 0.600000f, 0.570000f,
    -0.400000f, 0.000000f, -0.750000f, -0.580000f, 0.000000f, -0.180000f, 0.000000f, 1.000000f, 0.000000f,
    -0.050000f, 0.000000f, 0.000000f, 0.130000f, 0.600000f, 0.570000f,
    -0.050000f, 0.600000f, 0.570000f, 0.130000f, 0.000000f, 0.000000f, -0.050000f, 0.000000f, 0.570000f,
    0.180000f, -0.600000f, -0.570000f, 0.000000f, -0.600000f, 0.000000f, -0.953583f, -0.000000f, -0.301131f,
    -0.050000f, 0.000000f, 0.570000f, 0.530000f, 0.600000f, 0.750000f,
    0.530000f, 0.600000f, 0.750000f, -0.050000f, 0.000000f, 0.570000f, 0.530000f, 0.000000f, 0.750000f,
    -0.580000f, -0.600000f, -0.180000f, 0.000000f, -0.600000f, 0.000000f, -0.296399f, 0.000000f, 0.955064f,
    0.530000f, 0.000000f, 0.170000f, 0.700000f, 0.600000f, 0.750000f,
    0.700000f, 0.600000f, 0.170000f, 0.530000f, 0.000000f, 0.750000f, 0.700000f, 0.000000f, 0.170000f,
    -0.170000f, -0.600000f, 0.580000f, 0.000000f, -0.600000f, 0.000000f, 0.959629f, 0.000000f, 0.281270f,
    0.130000f, 0.000000f, -0.000000f, 0.700000f, 0.600000f, 0.170000f,
    0.130000f, 0.600000f, -0.000000f, 0.700000f, 0.000000f, 0.170000f, 0.130000f, 0.000000f, 0.000000f,
    0.570000f, -0.600000f, 0.170000f, 0.000000f, -0.600000f, 0.000000f, 0.285805f, 0.000000f, -0.958288f,
    0.130000f, 0.600000f, -0.000000f, 0.700000f, 0.600000f, 0.750000f,
    0.530000f, 0.600000f, 0.750000f, 0.700000f, 0.600000f, 0.170000f, 0.130000f, 0.600000f, -0.000000f,
    0.170000f, 0.000000f, -0.580000f, -0.400000f, 0.000000f, -0.750000f, 0.000000f, 1.000000f, 0.000000f,
    -0.050000f, 0.000000f, -0.000000f, 0.130000f, 0.600000f, 0.570000f,
    -0.050000f, 0.600000f, 0.570000f, 0.130000f, 0.600000f, -0.000000f, 0.130000f, 0.000000f, 0.000000f,
    0.180000f, 0.000000f, -0.570000f, 0.180000f, -0.600000f, -0.570000f, -0.953583f, 0.000000f, -0.301131f,
    -0.050000f, 0.000000f, 0.570000f, 0.530000f, 0.600000f, 0.750000f,
    0.530000f, 0.600000f, 0.750000f, -0.050000f, 0.600000f, 0.570000f, -0.050000f, 0.000000f, 0.570000f,
    -0.580000f, 0.000000f, -0.180000f, -0.580000f, -0.600000f, -0.180000f, -0.296399f, 0.000000f, 0.955064f,
    0.530000f, 0.000000f, 0.170000f, 0.700000f, 0.600000f, 0.750000f,
    0.700000f, 0.600000f, 0.170000f, 0.530000f, 0.600000f, 0.750000f, 0.530000f, 0.000000f, 0.750000f,
    -0.170000f, 0.000000f, 0.580000f, -0.170000f, -0.600000f, 0.580000f, 0.959629f, 0.000000f, 0.281270f,
    0.130000f, 0.000000f, -0.000000f, 0.700000f, 0.600000f, 0.170000f,
    0.130000f, 0.600000f, -0.000000f, 0.700000f, 0.600000f, 0.170000f, 0.700000f, 0.000000f, 0.170000f,
    0.570000f, 0.000000f, 0.170000f, 0.570000f, -0.600000f, 0.170000f, 0.285805f, 0.000000f, -0.958288f,
};

struct triangle_t
{
    struct vec3_t bbmin, bbmax;
    struct vec3_t v0, v1, v2;
    struct vec3_t E1, E2, normal;
};

struct intersection_t
{
    int8_t i;
    float t;
};
static struct intersection_t intersection;

struct ray_t
{
    struct vec3_t start;
    struct vec3_t direction;
    struct vec3_t inv_direction;
};
static struct ray_t ray;

struct interaction_t
{
    int8_t i;
    float t;
    __code struct triangle_t* surface;
    struct vec3_t position;
    struct vec3_t normal;
};
static struct interaction_t interaction;

#undef TWO_PI
#define INV_PI 0.318310f
#define TWO_PI 6.283185f
#define EPSILON 0.000001f

#define rand01() ((float)rand() / RAND_MAX)

static void makeInteraction(void)
{
    interaction.i = intersection.i;
    interaction.t = intersection.t;
    interaction.surface = &((__code struct triangle_t*)triangles)[intersection.i];
    vec3_mad_s(interaction.position, ray.start, ray.direction, intersection.t);
    vec3_assign(interaction.normal, interaction.surface->normal);

    if (dot_m(ray.direction, interaction.normal) < 0.0f)
    {
        vec3_minus(interaction.normal, interaction.normal);
    }

    orthonormalBasis(interaction.normal);
}

static struct vec3_t bbmin;
static struct vec3_t bbmax;

static float t;
static struct vec3_t t0;
static struct vec3_t t1;
static struct vec3_t temp;
static uint8_t bb_intersect(void)
{
    vec3_sub(t0, bbmin, ray.start);
    vec3_mul_assign(t0, ray.inv_direction);

    vec3_sub(t1, bbmax, ray.start);
    vec3_mul_assign(t1, ray.inv_direction);

    vec3_min(temp, t0, t1);
    t = max(vec3_compMax(temp), 0.0f);

    vec3_max(temp, t0, t1);
    return vec3_compMin(temp) >= t ? 1 : 0;
}

static __code struct triangle_t* s;
static uint8_t tt_intersect(void)
{
    struct vec3_t P;
    cross(P, ray.direction, s->E2);
    const float determinant = dot(P, s->E1);
    if (determinant < EPSILON && determinant > -EPSILON)
    {
        return 0;
    }

    const float inv_determinant = 1.0f / determinant;

    struct vec3_t T;
    vec3_sub(T, ray.start, s->v0);
    const float u = dot(P, T) * inv_determinant;
    if (u > 1.0f || u < 0.0f)
    {
        return 0;
    }

    struct vec3_t Q;
    cross(Q, T, s->E1);
    const float v = dot(Q, ray.direction) * inv_determinant;
    if (v > 1.0f || v < 0.0f || u + v > 1.0f)
    {
        return 0;
    }

    t = dot(Q, s->E2) * inv_determinant;
    if (t <= 0.0f)
    {
        return 0;
    }

    return 1;
}

static uint8_t tb_intersect(void)
{
    vec3_sub(t0, s->bbmin, ray.start);
    vec3_mul_assign(t0, ray.inv_direction);

    vec3_sub(t1, s->bbmax, ray.start);
    vec3_mul_assign(t1, ray.inv_direction);

    vec3_min(temp, t0, t1);
    t = max(vec3_compMax(temp), 0.0f);

    vec3_max(temp, t0, t1);
    return vec3_compMin(temp) >= t ? 1 : 0;
}

static void intersect(void)
{
    intersection.t = FLT_MAX;
    intersection.i = -1;

    if (bb_intersect())
    {
        for (k = 0; k < 32; ++k)
        {
            s = &((__code struct triangle_t*)triangles)[k];
            if (!tb_intersect())
                continue;
            if (tt_intersect())
            {
                if (t < intersection.t)
                {
                    intersection.t = t;
                    intersection.i = k;
                }
            }
        }
    }
}

static struct vec3_t reflectance;
static void Reflectance(int8_t i)
{
    if (i == 8 || i == 9)
    {
        vec3_assign_s3(reflectance, 0.05f, 0.65f, 0.05f);
    }
    else if (i == 10 || i == 11)
    {
        vec3_assign_s3(reflectance, 0.65f, 0.05f, 0.05f);
    }
    else
    {
        vec3_assign_s(reflectance, 0.65f);
    }
}

#define abs(x) ((x) > 0 ? x : -x)
static struct vec3_t bsdf_absIdotN;
static float bsdf_pdf;
static uint8_t sampleBSDF(void)
{
    struct vec3_t wi;
    struct vec3_t z;
    vec3_assign_s3(z, T.m[2][0], T.m[2][1], T.m[2][2]);
    wi.z = dot(z, ray.direction);

    if (wi.z <= 0.0f)
    {
        return 0;
    }

    bsdf_pdf = wi.z * INV_PI;

    const float temp = INV_PI * abs(wi.z);
    Reflectance(interaction.i);
    vec3_mul_s(bsdf_absIdotN, reflectance, temp);

    return bsdf_pdf > 0.0f ? 1 : 0;
}

static float u0;
static float u1;
static float u2;

static struct vec3_t linear_r;
static struct vec3_t linear_x;
static struct vec3_t linear_y;
static struct vec3_t linear_z;
static struct vec3_t linear_t;
static void linearCombination(void)
{
    linear_r.x = dot(linear_x, linear_t);
    linear_r.y = dot(linear_y, linear_t);
    linear_r.z = dot(linear_z, linear_t);
}
static int8_t lightIdx;
static struct vec3_t light_pos;
static void lightPoint(void)
{
    const float su = sqrtf(u0);
    const float x = (1 - su);
    const float y = (1 - u1) * su;
    const float z = u1 * su;
    __code struct triangle_t* light = &((__code struct triangle_t*)triangles)[lightIdx];
    vec3_assign_s3(linear_t, x, y, z);
    vec3_assign_s3(linear_x, light->v0.x, light->v1.x, light->v2.x);
    vec3_assign_s3(linear_y, light->v0.y, light->v1.y, light->v2.y);
    vec3_assign_s3(linear_z, light->v0.z, light->v1.z, light->v2.z);
    linearCombination();
    vec3_assign(light_pos, linear_r);
}

static void cosWeightedHemi(void)
{
    u0 = rand01();
    u1 = rand01();

    const float r = sqrtf(u0);
    const float azimuth = u1 * TWO_PI;

    struct vec3_t v;
    vec3_assign_s3(v, r * cosf(azimuth), r * sinf(azimuth), sqrtf(1.0f - u0));

    vec3_mad_s(ray.start, interaction.position, interaction.normal, EPSILON);

    struct vec3_t x;
    struct vec3_t y;
    struct vec3_t z;
    vec3_assign_s3(x, T.m[0][0], T.m[1][0], T.m[2][0]);
    vec3_assign_s3(y, T.m[0][1], T.m[1][1], T.m[2][1]);
    vec3_assign_s3(z, T.m[0][2], T.m[1][2], T.m[2][2]);
    ray.direction.x = dot(x, v);
    ray.direction.y = dot(y, v);
    ray.direction.z = dot(z, v);

    vec3_rcp(ray.inv_direction, ray.direction);
}

#define balanceHeuristic(a, b) ((a) / ((a) + (b)))
static struct vec3_t radiance;
static struct vec3_t lightColor;
#define light_area 0.0893f
static void sampleLight(void)
{
    vec3_assign_s(radiance, 0.0f);

    u0 = rand01();
    u1 = rand01();
    u2 = rand01();

    lightIdx = u0 < 0.5f ? 0 : 1;

    lightPoint();
    vec3_mad_s(ray.start, interaction.position, interaction.normal, EPSILON);
    vec3_sub(ray.direction, light_pos, ray.start);
    normalize(ray.direction);
    vec3_rcp(ray.inv_direction, ray.direction);

    __code struct triangle_t* light = &((__code struct triangle_t*)triangles)[lightIdx];
    const float cos_light_theta = dot_m(ray.direction, light->normal);
    if (cos_light_theta <= 0.0f)
    {
        return;
    }

    const float cos_theta = dot(ray.direction, interaction.normal);
    if (cos_theta <= 0.0f)
    {
        return;
    }

    intersect();
    if (intersection.i == -1 || intersection.i != lightIdx)
    {
        return;
    }

    if (!sampleBSDF())
    {
        return;
    }

    const float light_pdf = (intersection.t * intersection.t) / (light_area * cos_light_theta);
    const float mis_weight = balanceHeuristic(light_pdf, bsdf_pdf);
    vec3_mul(radiance, bsdf_absIdotN, lightColor);
    vec3_mul_assign_s(radiance, mis_weight);
    vec3_div_assign_s(radiance, light_pdf);
    vec3_div_assign_s(radiance, 0.5f);
}
static uint16_t depth;
static struct vec3_t sample;
static void sampleRay(void)
{
    struct vec3_t throughput;
    vec3_assign_s(throughput, 1.0f);

    depth = 0;
    while (1)
    {
        intersect();
        if (intersection.i == -1)
        {
            vec3_add_assign(sample, throughput);
            return;
        }

        if (intersection.i < 2)
        {
            if (depth == 0)
            {
                vec3_add_assign(sample, lightColor);
            }
            return;
        }

        makeInteraction();

        sampleLight();
        vec3_mad_assign(sample, radiance, throughput);

        depth++;
        cosWeightedHemi();
        if (!sampleBSDF())
        {
            return;
        }

        vec3_mul_assign(throughput, bsdf_absIdotN);
        vec3_div_assign_s(throughput, bsdf_pdf);

        if (depth == max_depth)
        {
            return;
        }
    }
}

static uint8_t X;
static uint8_t Y;

static struct vec3_t eye;
static struct vec3_t center;
static struct vec3_t up;
static struct vec3_t view_x;
static struct vec3_t view_y;
static struct vec3_t view_z;
static struct mat4_t view;
static void precompute_rt(void)
{
    vec3_assign_s3(eye, 0.0f, 1.0f, 3.5f);
    vec3_assign_s3(center, 0.0f, 1.0f, 0.0f);
    vec3_assign_s3(up, 0.0f, 1.0f, 0.0f);

    lookat(view, eye, center, up);

    vec3_assign_s3(view_x, view.m[0][0], view.m[0][1], view.m[0][2]);
    vec3_assign_s3(view_y, view.m[1][0], view.m[1][1], view.m[1][2]);
    vec3_assign_s3(view_z, view.m[2][0], view.m[2][1], view.m[2][2]);

    vec3_assign_s3(lightColor, 200.0f, 200.0f, 200.0f);
    vec3_assign_s3(bbmin, -1.0f, 0.0f, -1.0f);
    vec3_assign_s3(bbmax, 1.0f, 2.0f, 1.0f);
    X = 0;
    Y = 0;
}

#define LCD_W 160
#define LCD_H 80

#define alpha 45
static void samplePixel(void)
{
    vec3_assign_s(sample, 0.0f);
    for (i = 0; i < spp; i++)
    {
        u0 = rand01();
        u1 = rand01();

        const float Xw = X + u0;
        const float Yw = LCD_H - 1 - Y + u1;
        const float Xc = Xw - LCD_W * 0.5f;
        const float Yc = Yw - LCD_H * 0.5f;
        const float Zc = -LCD_H * 0.5f / tanf(radians(alpha) * 0.5f);

        vec3_assign_s3(linear_t, Xc, Yc, Zc);
        vec3_assign_s3(linear_x, view_x.x, view_y.x, view_z.x);
        vec3_assign_s3(linear_y, view_x.y, view_y.y, view_z.y);
        vec3_assign_s3(linear_z, view_x.z, view_y.z, view_z.z);
        linearCombination();
        vec3_assign(ray.direction, linear_r);

        vec3_assign(ray.start, eye);
        normalize(ray.direction);
        vec3_rcp(ray.inv_direction, ray.direction);

        sampleRay();
    }

    vec3_div_assign_s(sample, spp);

    struct vec3_t one;
    vec3_assign_s(one, 1.0f);

    struct vec3_t sample_one;
    vec3_add(sample_one, sample, one);

    vec3_div_assign(sample, sample_one);
}

#define drawpixel(x, y, c) \
{ \
    SPI_Send(c >> 8); \
    SPI_Send(c & 0xff); \
}
static void draw3drt(void)
{
    samplePixel();
    const uint16_t r = sample.x * 0x1f;
    const uint16_t g = sample.y * 0x3f;
    const uint16_t b = sample.z * 0x1f;
    const uint16_t c16 = (r << 11) | (g << 5) | b;
    drawpixel(X, Y, c16);
    X++;
    if (X == LCD_W)
    {
        X = 0;
        Y++;
        if (Y == LCD_H)
        {
            Y = 0;
        }
    }
}

#define PWMA_CCER1      (*(uint8_t volatile __xdata *)0xfecc)
#define PWMA_CCMR1      (*(uint8_t volatile __xdata *)0xfec8)
#define PWMA_ENO        (*(uint8_t volatile __xdata *)0xfeb1)
#define PWMA_BKR        (*(uint8_t volatile __xdata *)0xfedd)
#define PWMA_CCR1       (*(uint16_t volatile __xdata *)0xfed5)
#define PWMA_ARR        (*(uint16_t volatile __xdata *)0xfed2)
#define PWMA_CR1        (*(uint8_t volatile __xdata *)0xfec0)

#define P1SR        (*(uint8_t volatile __xdata *)0xFE21)
#define P3SR        (*(uint8_t volatile __xdata *)0xFE23)

static inline void LCD_Set_Address(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
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
static void LCD_Clear(void)
{
	LCD_Set_Address(0, 0, (uint8_t)(160 - 1), (uint8_t)(80 - 1));
	LCD_CS_CLR();
	for (i = 0; i < 160; i++)
	{
		for (j = 0; j < 80; j++)
		{
			SPI_Send(0);
			SPI_Send(0);
		}
	}
	LCD_CS_SET();
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
	PWMA_CCR1 = 30;
	PWMA_ARR = 500;
	PWMA_ENO = 0x01;
	PWMA_BKR = 0x80;
	PWMA_CR1 = 0x01;

#if SPI
	P_SW1 = 0x00;
	SPCTL = 0xD0;
	SPSTAT = 0xC0;
#endif

	LCD_Init();

	LCD_Clear();

#if SPI
	P1SR &= ~0x28;
	P3SR &= ~0xe0;
	SPCTL = 0xD3;
#endif

	precompute_rt();

	while (1)
	{
		if (X == 0 && Y == 0)
		{
			LCD_Set_Address(0, 0, (uint8_t)(LCD_W - 1), (uint8_t)(LCD_H - 1));
		}
		LCD_CS_CLR();
		draw3drt();
		LCD_CS_SET();
	}
}
