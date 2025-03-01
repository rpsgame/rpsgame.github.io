/*
* STC8H8K64U-45I-PDIP40 @40MHz
* LCD16080 ST7735
* sdcc 3dcolor.c --model-large
* https://liuliqiang.com/51/3dcolor.c
* by liuliqiang 2025-03-01
*/

#include <8051.h>
#include <stdint.h>
#include <math.h>

#define SPI 1
#define DMA 1

#define WIRE 0
#define DEPTH 0

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

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430
#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
#define LIGHTGREEN     	 0X841F
#define LGRAY 			 0XC618
#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12

#define LCD_W 96
#define LCD_H 80

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

#define mat4_mul_vec3_h(r, a, b) \
{ \
	for (k = 0; k < 4; ++k) \
	{ \
		r.v[k] = a.m[3][k]; \
		for (l = 0; l < 3; ++l) \
		{ \
			r.v[k] += a.m[l][k] * b.v[l]; \
		} \
	} \
}

#define mat4_mul_vec3(r, a, b) \
{ \
	for (k = 0; k < 3; ++k) \
	{ \
		r.v[k] = 0; \
		for (l = 0; l < 3; ++l) \
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

static const float __code vertex[] =
{
	-1.0f, -1.0f, -1.0f, // 0
	 1.0f,  1.0f, -1.0f, // 1
	 1.0f, -1.0f, -1.0f, // 2
	-1.0f,  1.0f, -1.0f, // 3
	-1.0f, -1.0f,  1.0f, // 4
	 1.0f, -1.0f,  1.0f, // 5
	 1.0f,  1.0f,  1.0f, // 6
	-1.0f,  1.0f,  1.0f, // 7
};
static const float __code texcoord[] =
{
	// back face
	0.0f, 1.0f, // bottom-left
	1.0f, 0.0f, // top-right
	0.0f, 0.0f, // bottom-right         
	1.0f, 0.0f, // top-right
	0.0f, 1.0f, // bottom-left
	1.0f, 1.0f, // top-left
	// front face
	0.0f, 0.0f, // bottom-left
	0.0f, 1.0f, // bottom-right
	1.0f, 1.0f, // top-right
	1.0f, 1.0f, // top-right
	1.0f, 0.0f, // top-left
	0.0f, 0.0f, // bottom-left
	// left face
	1.0f, 1.0f, // top-right
	1.0f, 0.0f, // top-left
	0.0f, 0.0f, // bottom-left
	0.0f, 0.0f, // bottom-left
	0.0f, 1.0f, // bottom-right
	1.0f, 1.0f, // top-right
	// right face
	1.0f, 0.0f, // top-left
	0.0f, 1.0f, // bottom-right
	1.0f, 1.0f, // top-right         
	0.0f, 1.0f, // bottom-right
	1.0f, 0.0f, // top-left
	0.0f, 0.0f, // bottom-left     
	// bottom face
	1.0f, 0.0f, // top-right
	1.0f, 1.0f, // top-left
	0.0f, 1.0f, // bottom-left
	0.0f, 1.0f, // bottom-left
	0.0f, 0.0f, // bottom-right
	1.0f, 0.0f, // top-right
	// top face
	1.0f, 0.0f, // top-left
	0.0f, 1.0f, // bottom-right
	1.0f, 1.0f, // top-right     
	0.0f, 1.0f, // bottom-right
	1.0f, 0.0f, // top-left
	0.0f, 0.0f  // bottom-left     
};
static const uint8_t __code index[] =
{
	// back face
	0, // bottom-left
	1, // top-right
	2, // bottom-right         
	1, // top-right
	0, // bottom-left
	3, // top-left
	// front face
	4, // bottom-left
	5, // bottom-right
	6, // top-right
	6, // top-right
	7, // top-left
	4, // bottom-left
	// left face
	7, // top-right
	3, // top-left
	0, // bottom-left
	0, // bottom-left
	4, // bottom-right
	7, // top-right
	// right face
	6, // top-left
	2, // bottom-right
	1, // top-right         
	2, // bottom-right
	6, // top-left
	5, // bottom-left     
	// bottom face
	0, // top-right
	2, // top-left
	5, // bottom-left
	5, // bottom-left
	4, // bottom-right
	0, // top-right
	// top face
	3, // top-left
	6, // bottom-right
	1, // top-right     
	6, // bottom-right
	3, // top-left
	7, // bottom-left     
};
static const uint16_t __code doge[32][32] =
{
	{0x2dfc,0x2dfc,0xcdfd,0xcdfd,0x8dff,0x8dff,0xedcf,0xedcf,0xed97,0xed97,0xce67,0xce6f,0xd467,0xd467,0xba6f,0xba6f,0xdd6e,0xde6e,0x3e65,0x5e65,0x9f6b,0x9f6b,0x5f93,0x5e93,0x19a3,0x3bb3,0x5dfb,0x5dfb,0x57fb,0x57fb,0x50fb,0x50fb,},
	{0x2dfc,0x2dfc,0xcdfd,0xcdfd,0x8dff,0x8dff,0xedcf,0xedcf,0xcd97,0xed97,0xad67,0x2c77,0xd18e,0x2f9e,0xf29d,0xb19d,0x539d,0x749d,0x9694,0xd98c,0x7c73,0x9e63,0x5e93,0x7a8b,0x2f8b,0xdab2,0x1ceb,0x1ceb,0x15eb,0xf4e2,0x30f3,0x50fb,},
	{0xcdfd,0xcdf5,0x8df7,0x8df7,0xcdcf,0xedcf,0xed97,0xcd97,0xad67,0xec7e,0xeda5,0x49cd,0x69e5,0x28e5,0x49e5,0x6add,0xcced,0x0dee,0x0dee,0x6ae5,0x8ec4,0xf59b,0x76a3,0xce73,0xc992,0x4faa,0x2cba,0x2cba,0x09ba,0x09ba,0xeceb,0x2df4,},
	{0xcdf5,0xccf5,0x8cf7,0x8df7,0xcdcf,0xcdcf,0xcc97,0xac97,0x4a96,0x2cbd,0xb3dd,0x96ee,0x55e6,0x96e6,0xd6e6,0xf6e6,0x72e6,0xcfcd,0xb1ee,0x54ff,0x14f7,0xd1c5,0xe78a,0x858a,0x65d9,0x23d9,0x23d1,0x44c9,0x45c1,0xacd2,0x0cf4,0x2cf4,},
	{0x6cf7,0x6cf7,0xacc7,0xacc7,0xac97,0xcc97,0x6d6f,0xab95,0xccdd,0xf3f6,0x39f7,0x5af7,0x5af7,0x7af7,0x9af7,0x9af7,0x56f7,0x96f7,0xd3e6,0x14ef,0xb6f7,0x32ce,0x648a,0x049a,0xe3e0,0xe3e0,0x03d9,0xe3c8,0x64c9,0x09d3,0xacf5,0xccf5,},
	{0x6cf7,0x2ac6,0xc995,0xe995,0xe975,0xe975,0x6965,0x686c,0x4eb5,0xedb4,0x8bbc,0x4dbc,0x4ebc,0x4cbc,0x6cbc,0x6cb4,0xceac,0x30ad,0x51a5,0x7095,0x7095,0x6b7c,0xc4b1,0xe4d0,0xe3d8,0xe3d8,0xe3d0,0xe3c8,0x03c9,0x44c1,0xcae4,0xccf5,},
	{0xacc7,0x4aa6,0x496d,0x696d,0x4965,0xe483,0x8d9c,0xec7b,0xcc7b,0x36ce,0xf5bd,0x4a83,0xab8b,0x76ce,0x78ce,0x58ce,0x12b5,0x2973,0xadac,0xce8c,0x308d,0x6a64,0x2492,0x64b1,0xa3d8,0xa3d8,0xe3c8,0x03c9,0x44c9,0xa4b1,0xcad5,0x6cf7,},
	{0xcccf,0xacc7,0x8c97,0x8c97,0x4b7e,0x4acd,0x17f7,0x5aef,0x5aef,0xbcf7,0xdcf7,0x10bd,0x13de,0xfbf7,0xddf7,0xdff7,0x5cf7,0x53bd,0x73d6,0x9aef,0xbae7,0x529e,0x4382,0xe5a1,0x05e9,0x04d9,0x03c9,0xe3c0,0x63c1,0xa8bb,0x0cef,0x8cf7,},
	{0xcc97,0xcc97,0xee6f,0x6d6f,0x0cbd,0xd7de,0xdcf7,0xdcf7,0xdcf7,0x3af7,0xafc4,0x258a,0xc89a,0xf3dd,0x9cf7,0xbeff,0x9eff,0x3bef,0x5bef,0xfcf7,0xdcf7,0xd8de,0xea73,0xc653,0x677b,0x8592,0x84c1,0x03e1,0xa4e2,0x6bde,0xcccf,0xcccf,},
	{0xcd97,0xed97,0xee6f,0x2a96,0x8eed,0xbdf7,0xdcff,0xbaf7,0xbaf7,0xd3c5,0x4171,0x6281,0x4179,0x4482,0x97e6,0xdcff,0xbdff,0xbdff,0xbdff,0xbcf7,0xdcf7,0x9bff,0x96ce,0x8c7c,0x6873,0x4ca4,0x4dc4,0x06d3,0x43c2,0xc8bc,0x8cc7,0xcccf,},
	{0xce6f,0xce67,0xb467,0x2abd,0x91f6,0x9ff7,0xdbff,0xb7ff,0xb3ee,0xabbc,0xe191,0x8191,0x4189,0xe099,0xd0dd,0x35f7,0x9aff,0x9bff,0x7eff,0x9cff,0xbcff,0xdcf7,0xbbf7,0xf8de,0x2b84,0x53ce,0x97ef,0x2fee,0x87c4,0xcbbe,0xac97,0xcc97,},
	{0xce6f,0xce67,0x126f,0x6cb5,0x54f7,0x9af7,0x34f7,0xeef6,0xadf6,0x6cee,0x0ade,0x88d5,0x47cd,0x0be6,0xeef6,0xeef6,0x12ff,0x57ff,0x7bff,0xbcff,0xbcf7,0xdcef,0xdbef,0xdcef,0x55c6,0xd5d6,0xb7ff,0x51f7,0x68c5,0x2bbf,0xed97,0xcc97,},
	{0xf46f,0xf46f,0x9676,0xadc5,0x90ee,0xb5f6,0x6fee,0x6cee,0x8cf6,0xadf6,0xccf6,0xacf6,0xccf6,0xedf6,0xcdf6,0xcdf6,0xd0f6,0x14f7,0x5af7,0xbcff,0xbcf7,0xdcef,0xdbef,0xdbef,0x99ef,0x98ef,0x76ff,0x51f7,0x89cd,0x8b8e,0xee6f,0xce67,},
	{0xf46f,0xf46f,0x7596,0x69e5,0x4bf6,0x0cd6,0xeccc,0x2bcc,0x8ad4,0x6bdd,0x8cf6,0x6af6,0xcbfe,0xeafe,0xcefe,0xaff6,0x2cde,0xebd5,0x0dde,0x34d6,0x35d6,0xd6e6,0x78f7,0xb8ff,0x79f7,0x36f7,0x12f7,0xeff6,0x48d5,0x4b9e,0xee6f,0xce67,},
	{0xfb6f,0xfb6f,0xd78d,0x4bdd,0xcacc,0xd6cd,0x76ee,0x499b,0x2071,0x208a,0xa8cc,0xacf6,0xedfe,0xcdfe,0xcff6,0x2bc5,0x33cd,0x57de,0x90cd,0x8582,0xe479,0x499b,0x4ecd,0xd1f6,0xb1f6,0xd0f6,0xaef6,0xcdf6,0x88dd,0x4b8e,0xf46f,0xd467,},
	{0xfb6f,0xfb6f,0xd78d,0xaacc,0x8dcc,0x3ef7,0x57ee,0xa78a,0x6279,0x8179,0xa39a,0x8cee,0xcdfe,0xeefe,0xacd5,0x8ed5,0x1df7,0x9dff,0x51bd,0xc281,0x8089,0x6081,0x408a,0x0bee,0xccfe,0xcbfe,0xaaf6,0xcbf6,0x67e5,0x4b8e,0xf46f,0xd467,},
	{0x1f6f,0x1f6f,0xd884,0xebd4,0x8cc4,0xbbee,0x78f6,0x0993,0x50c4,0x477a,0xcba3,0x8eee,0xedf6,0x0df7,0xced5,0x93d5,0x3ef7,0x7eff,0x53b5,0xc481,0x669a,0xa289,0x808a,0x0cee,0xccfe,0xcbfe,0xaaf6,0xabf6,0x47d5,0x707e,0xfb6f,0xdb67,},
	{0x1f6f,0x1f6f,0xba64,0x2cd5,0xcaed,0xf0c4,0x73dd,0xcca3,0x51bc,0x70ac,0xb7e6,0x11f7,0x0ef7,0x0df7,0xf5f6,0x52c5,0x98cd,0x79de,0x73c5,0x6782,0x4dcc,0x07a3,0xc9bc,0x8dee,0xccf6,0xecf6,0xcbf6,0x6af6,0x27cd,0xf266,0xfb6f,0xdb67,},
	{0x5f6d,0x5f6d,0x7d63,0xacc4,0x08f6,0xaeee,0xd5e6,0x97de,0x7bde,0x1cef,0xbcff,0x73f7,0x0ff7,0x0cef,0x36f7,0x1af7,0xb7de,0x14d6,0xf4d5,0x72cd,0x92d5,0xb0d5,0x8fee,0xeeee,0xecf6,0xcbf6,0xaaf6,0xe8f5,0xebc4,0x7a67,0xfe6e,0xfe66,},
	{0x5f6d,0x5f6d,0x9e6b,0xf4a3,0x47dd,0xcaf6,0xeff6,0x11f7,0x76ff,0xb7ff,0xb7ff,0x52f7,0x30f7,0x0ef7,0x53ff,0x75ff,0x97ff,0x97f7,0x98f7,0x73f7,0x30f7,0xcef6,0xcef6,0xcdf6,0xecf6,0xccf6,0x8af6,0x46f5,0x91ad,0xdb67,0xfe66,0xfe66,},
	{0x9f6b,0x9f6b,0x5f93,0x3a93,0x07d5,0xc9f6,0xecf6,0x0ef7,0x32f7,0x53f7,0x54f7,0x51ff,0x30f7,0x2fff,0x11f7,0x12f7,0x14f7,0x14f7,0x14f7,0x0ff7,0xedee,0xcdf6,0xcdf6,0xcdf6,0xecf6,0xecf6,0x6af6,0x07e5,0xf985,0x1f6f,0x5e65,0x5e65,},
	{0x9f6b,0x9f6b,0x5f93,0x3b93,0x87cc,0xa6d5,0xccf6,0xeef6,0xcdf6,0xcdf6,0xeef6,0x10f7,0x30ff,0x10ff,0xeffe,0xeefe,0xedfe,0xedfe,0xedf6,0xedf6,0xeef6,0xeef6,0xedf6,0xedf6,0xecf6,0xccf6,0x88dd,0x66d4,0xb88d,0x1f6f,0x5e65,0x5e65,},
	{0x5f93,0x5f93,0x5fcb,0x39bb,0x85cc,0x04d5,0x09e6,0xacf6,0xcdf6,0xcdf6,0xeef6,0xeef6,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x0fff,0x2fff,0x2fff,0x0ff7,0x0ef7,0x0ef7,0x0ef7,0xccf6,0xe9e5,0x25dd,0xe8d4,0xb794,0x5f6d,0x9e63,0x9e63,},
	{0x5f93,0x5f93,0x5fcb,0x76bb,0x27dd,0x08ee,0x08e6,0x8af6,0xeef6,0x0ff7,0x0ff7,0xeef6,0xcdf6,0xedf6,0xeffe,0x0fff,0x0fff,0x0fff,0x10ff,0x2ff7,0x2ff7,0x2ef7,0x2ff7,0x2fff,0x8cf6,0x06cd,0x66e5,0x29dd,0x9884,0x5e6d,0x9e63,0x9e63,},
	{0x5fcb,0x5fcb,0x5df3,0xf2db,0x87ed,0x89fe,0x89f6,0xabf6,0xeff6,0x30ff,0x31f7,0xeff6,0xeef6,0xeff6,0x10ff,0x10f7,0x10ff,0x0ff7,0x0fff,0x2ff7,0x2ff7,0x2ff7,0x2fff,0xcef6,0x46ed,0x64cc,0xa6ed,0x09cd,0x7a6b,0x9e6b,0x5e93,0x5e93,},
	{0x5ecb,0x5ecb,0x3df3,0x6fdc,0xa8f5,0x48fe,0x48f6,0x8af6,0xadf6,0xadf6,0x6ef6,0xcefe,0x0fff,0x30ff,0x31ff,0x31ff,0x31f7,0x30f7,0x30f7,0x0ff7,0x0ff7,0x0ff7,0x0ff7,0x4cee,0x24cc,0x65d4,0xe6f5,0xccbc,0x7d6b,0x9e63,0x5e93,0x5e93,},
	{0x5df3,0x3df3,0x34db,0xa9dc,0xe9f5,0x48fe,0x48fe,0x48fe,0xa6f5,0xc5dc,0x05dd,0x69dd,0xabdd,0x2ee6,0x8fee,0xafee,0xcff6,0xafee,0x8eee,0x0ce6,0x8ad5,0xafee,0x0ff7,0x4cf6,0xa4dc,0x25dd,0xa6ed,0xf393,0x5e93,0x5e93,0x5ecb,0x5ecb,},
	{0x5dfb,0x5df3,0x53d3,0xc9dc,0x2af6,0x49fe,0xe9f5,0x48e5,0xc9cc,0x2ae5,0x4cf7,0x6bde,0xaaad,0xe9bc,0xe9c4,0x0acd,0x2ad5,0x2ac5,0x0bb5,0xce8d,0xb18d,0xa6d4,0x4cf6,0x4cf6,0xe6ec,0xa8f5,0x45dd,0x997b,0x5e93,0x5e93,0x5ecb,0x5fcb,},
	{0x56f3,0x57fb,0x0ee3,0x28d4,0x07e5,0x06e5,0xa6d4,0x88cc,0x0cef,0x8cf7,0xcccf,0xcccf,0xcc97,0xac97,0x6d6f,0x0c6f,0x126f,0x736f,0xba6f,0xdb6f,0xde6e,0x769d,0x08dd,0xeaf5,0xc8f5,0xc7ed,0x2fb4,0x5d93,0x5ecb,0x5ecb,0x5dfb,0x5dfb,},
	{0x57fb,0x57fb,0x50fb,0x4fe3,0x0bd4,0x0bd4,0x6ce5,0xccf5,0x8df7,0x6cf7,0xcccf,0xcdcf,0xed97,0xed97,0xee6f,0xee6f,0xf46f,0xf46f,0xfb6f,0xfb6f,0xfe6e,0xde66,0x95a4,0x07e5,0xc9ed,0xadc4,0xba93,0x3e93,0x5ecb,0x5ecb,0x5dfb,0x5dfb,},
	{0x50fb,0x50fb,0x2dfc,0x2dfc,0xcdfd,0xcdfd,0x8dff,0x8dff,0xedcf,0xedcf,0xed97,0xed97,0xee6f,0xee6f,0xf46f,0xf46f,0xfb6f,0xfb6f,0x1f6f,0x1f6f,0x5f6d,0x5f6d,0x9d73,0xd893,0xb5a3,0x9ba3,0x5ecb,0x5fcb,0x5df3,0x5dfb,0x57fb,0x57fb,},
	{0x50fb,0x50fb,0x2dfc,0x2dfc,0xcdfd,0xcdfd,0x8dff,0x8dff,0xedcf,0xedcf,0xed97,0xed97,0xee6f,0xee6f,0xf46f,0xf46f,0xfb6f,0xfb6f,0x1f6f,0x1f6f,0x5f6d,0x5f6d,0x9f6b,0x9e63,0x3e93,0x5f93,0x5fcb,0x5fcb,0x5dfb,0x5dfb,0x57fb,0x57fb,},
};

#define TILE_W 32
#define TILE_H 16
struct buffer_t
{
	uint16_t buffer[TILE_H][TILE_W];
	uint16_t clear;
};
static struct buffer_t colorbuffer[2];
static uint8_t bufferID;
#if DEPTH
static struct buffer_t depthbuffer;
#endif
static uint8_t clipX0;
static uint8_t clipY0;
static uint8_t clipW;
static uint8_t clipH;

struct vertex_t
{
	float rhw;
	struct vec2i_t point;
#if DEPTH
	float point_z;
#endif
};
struct triangle_t
{
	uint8_t v0;
	uint8_t v1;
	uint8_t v2;
	struct vec2_t texcoord0;
	struct vec2_t texcoord1;
	struct vec2_t texcoord2;
	int16_t W12;
	int16_t W20;
	int16_t W01;
	float area_r;
	int8_t I12;
	int8_t I20;
	int8_t I01;
	int8_t J12;
	int8_t J20;
	int8_t J01;
	uint8_t xmin;
	uint8_t xmax;
	uint8_t ymin;
	uint8_t ymax;
};
static struct vertex_t vVertex[sizeof(vertex) / sizeof(struct vec3_t)];
static struct triangle_t vTriangle[sizeof(index) / 3];
static uint8_t triangleIdx;
#define triangle vTriangle[triangleIdx]

static uint8_t i;
static uint8_t j;
static uint8_t x;
static uint8_t y;
static struct mat4_t mvp;
static float rhw;
static struct vec4_t position;
static float point_x;
static float point_y;
static uint8_t v0;
static uint8_t v1;
static uint8_t v2;
static int16_t area;
static uint8_t xmin;
static uint8_t xmax;
static uint8_t ymin;
static uint8_t ymax;
static void makeTriangle(void)
{
	for (i = 0; i < sizeof(vertex) / sizeof(struct vec3_t); i++)
	{
		mat4_mul_vec3_h(position, mvp, ((__code struct vec3_t*)vertex)[i]);
		rhw = 1.0f / position.w;
		point_x = (position.x * rhw + 1.0f) * LCD_W * 0.5f;
		point_y = (1.0f - position.y * rhw) * LCD_H * 0.5f;
#if DEPTH
		vVertex[i].point_z = (1.0f - position.z * rhw) * 0.5f;
#endif
		vVertex[i].point.x = (int16_t)(point_x + 0.5f);
		vVertex[i].point.y = (int16_t)(point_y + 0.5f);
		vVertex[i].rhw = rhw;
	}

	triangleIdx = 0;
	for (i = 0; i < sizeof(index); i += 3)
	{
		v0 = index[i + 0];
		v1 = index[i + 1];
		v2 = index[i + 2];
#define perpdot0(a, b) ((0 - a.x) * (b.y - a.y) - (0 - a.y) * (b.x - a.x))
		triangle.W12 = perpdot0(vVertex[v1].point, vVertex[v2].point);
		triangle.W20 = perpdot0(vVertex[v2].point, vVertex[v0].point);
		triangle.W01 = perpdot0(vVertex[v0].point, vVertex[v1].point);
		area = triangle.W12 + triangle.W20 + triangle.W01;
		if (area <= 0)
			continue;
		triangle.area_r = 1.0f / area;
		triangle.v0 = v0;
		triangle.v1 = v1;
		triangle.v2 = v2;
		triangle.I12 = vVertex[v2].point.y - vVertex[v1].point.y;
		triangle.I20 = vVertex[v0].point.y - vVertex[v2].point.y;
		triangle.I01 = vVertex[v1].point.y - vVertex[v0].point.y;
		triangle.J12 = -(vVertex[v2].point.x - vVertex[v1].point.x);
		triangle.J20 = -(vVertex[v0].point.x - vVertex[v2].point.x);
		triangle.J01 = -(vVertex[v1].point.x - vVertex[v0].point.x);
		xmin = (uint8_t)(LCD_W - 1);
		xmax = 0;
		ymin = (uint8_t)(LCD_H - 1);
		ymax = 0;
#define bbox(v) \
		{xmin = max(0, min(xmin, v.x));} \
		{xmax = min((uint8_t)(LCD_W - 1), max(xmax, v.x));} \
		{ymin = max(0, min(triangle.ymin, v.y));} \
		{ymax = min((uint8_t)(LCD_H - 1), max(ymax, v.y));}
		bbox(vVertex[v0].point);
		bbox(vVertex[v1].point);
		bbox(vVertex[v2].point);
		triangle.xmin = xmin;
		triangle.xmax = xmax;
		triangle.ymin = ymin;
		triangle.ymax = ymax;
		triangle.texcoord0.x = ((__code struct vec2_t*)texcoord)[i + 0].x * vVertex[v0].rhw;
		triangle.texcoord0.y = ((__code struct vec2_t*)texcoord)[i + 0].y * vVertex[v0].rhw;
		triangle.texcoord1.x = ((__code struct vec2_t*)texcoord)[i + 1].x * vVertex[v1].rhw;
		triangle.texcoord1.y = ((__code struct vec2_t*)texcoord)[i + 1].y * vVertex[v1].rhw;
		triangle.texcoord2.x = ((__code struct vec2_t*)texcoord)[i + 2].x * vVertex[v2].rhw;
		triangle.texcoord2.y = ((__code struct vec2_t*)texcoord)[i + 2].y * vVertex[v2].rhw;
		triangleIdx++;
	}
}
static struct vec3_t uv;
static float u, v;
static int16_t I12offset;
static int16_t I20offset;
static int16_t I01offset;
static int16_t J12offset;
static int16_t J20offset;
static int16_t J01offset;
static int16_t wy12;
static int16_t wy20;
static int16_t wy01;
static int16_t w12;
static int16_t w20;
static int16_t w01;
#if WIRE
static void setPixel(int16_t x, int16_t y)
{
	if (x < clipX0 || x >= clipW || y < clipY0 || y >= clipH)
		return;
	colorbuffer[bufferID].buffer[y - clipY0][x - clipX0] = 0xffff;
}
static inline int16_t abs(int16_t x) { return x > 0 ? x : -x; }
static void plotLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int16_t err = dx + dy, e2; /* error value e_xy */

	for (;;) {  /* loop */
		setPixel(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}
}
#endif
static void drawTriangle(void)
{
	for (k = 0; k < triangleIdx; k++)
	{
#undef triangle
#define triangle vTriangle[k]
#if WIRE
		v0 = triangle.v0;
		v1 = triangle.v1;
		v2 = triangle.v2;
		plotLine(vVertex[v0].point.x, vVertex[v0].point.y, vVertex[v1].point.x, vVertex[v1].point.y);
		plotLine(vVertex[v0].point.x, vVertex[v0].point.y, vVertex[v2].point.x, vVertex[v2].point.y);
		plotLine(vVertex[v1].point.x, vVertex[v1].point.y, vVertex[v2].point.x, vVertex[v2].point.y);
#else
		xmin = triangle.xmin;
		xmax = triangle.xmax;
		ymin = triangle.ymin;
		ymax = triangle.ymax;
		if (xmin < clipX0) xmin = clipX0;
		if (xmin >= clipW) continue;
		if (xmax < clipX0) continue;
		if (xmax >= clipW) xmax = clipW - 1;
		if (ymin < clipY0) ymin = clipY0;
		if (ymin >= clipH) continue;
		if (ymax < clipY0) continue;
		if (ymax >= clipH) ymax = clipH - 1;

		v0 = triangle.v0;
		v1 = triangle.v1;
		v2 = triangle.v2;

		I12offset = triangle.I12 * xmin;
		I20offset = triangle.I20 * xmin;
		I01offset = triangle.I01 * xmin;

		J12offset = triangle.J12 * ymin;
		J20offset = triangle.J20 * ymin;
		J01offset = triangle.J01 * ymin;

		wy12 = triangle.W12 + J12offset;
		wy20 = triangle.W20 + J20offset;
		wy01 = triangle.W01 + J01offset;

		for (y = ymin; y <= ymax; y++)
		{
			w12 = wy12 + I12offset;
			w20 = wy20 + I20offset;
			w01 = wy01 + I01offset;

			for (x = xmin; x <= xmax; x++)
			{
				do
				{
					if ((w12 < 0) || (w20 < 0) || (w01 < 0))
						break;

					uv.x = w12 * triangle.area_r;
					uv.y = w20 * triangle.area_r;
					uv.z = w01 * triangle.area_r;

					struct vec3_t vertex_rhw;
					vertex_rhw.x = vVertex[v0].rhw;
					vertex_rhw.y = vVertex[v1].rhw;
					vertex_rhw.z = vVertex[v2].rhw;
					const float rhw_r = 1.0f / dot(vertex_rhw, uv);
					struct vec3_t uv_rhw;
					uv_rhw.x = uv.x * rhw_r;
					uv_rhw.y = uv.y * rhw_r;
					uv_rhw.z = uv.z * rhw_r;
#if DEPTH
					struct vec3_t point_z;
					point_z.x = vVertex[v0].point_z;
					point_z.y = vVertex[v1].point_z;
					point_z.z = vVertex[v2].point_z;
					const float pointz = dot(point_z, uv_rhw);
					const uint16_t depth = pointz * 0xffff;
					if (depth < depthbuffer.buffer[y - clipY0][x - clipX0])
						break;
					depthbuffer.buffer[y - clipY0][x - clipX0] = depth;
#endif
					struct vec3_t texcoordU;
					texcoordU.x = triangle.texcoord0.x;
					texcoordU.y = triangle.texcoord1.x;
					texcoordU.z = triangle.texcoord2.x;
					u = dot(texcoordU, uv_rhw);
					struct vec3_t texcoordV;
					texcoordV.x = triangle.texcoord0.y;
					texcoordV.y = triangle.texcoord1.y;
					texcoordV.z = triangle.texcoord2.y;
					v = dot(texcoordV, uv_rhw);
					{
						const uint16_t texcolor = doge[(uint8_t)(u * 31 + 0.5f)][(uint8_t)(v * 31 + 0.5f)];
						colorbuffer[bufferID].buffer[y - clipY0][x - clipX0] = texcolor;
					}

				} while (0);

				w12 += triangle.I12;
				w20 += triangle.I20;
				w01 += triangle.I01;
			}

			wy12 += triangle.J12;
			wy20 += triangle.J20;
			wy01 += triangle.J01;
		}
#endif
	}
}

static struct mat4_t vp;
static void precompute_1(void)
{
	struct mat4_t proj;
	{
		const float fovy = RADIANS(45.0f);
		const float aspect = (float)LCD_W / LCD_H;
		const float znear = 0.1f;
		const float zfar = 10.0f;
		perspective(proj, fovy, aspect, znear, zfar);
	}

	struct mat4_t view;
	{
		struct vec3_t eye;
		eye.x = 2.5f;
		eye.y = 2.5f;
		eye.z = 2.5f;
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
static float angle;
static void precompute_2(void)
{
	struct mat4_t r;
	{
		const float a = RADIANS(angle);
		const float x = 0.0f;
		const float y = 1.0f;
		const float z = 0.0f;
		rotate(r, a, x, y, z);
	}

	mat4_mul_mat4(mvp, vp, r);
}

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

#define PWMA_CCER1      (*(uint8_t volatile __xdata *)0xfecc)
#define PWMA_CCMR1      (*(uint8_t volatile __xdata *)0xfec8)
#define PWMA_ENO        (*(uint8_t volatile __xdata *)0xfeb1)
#define PWMA_BKR        (*(uint8_t volatile __xdata *)0xfedd)
#define PWMA_CCR1       (*(uint16_t volatile __xdata *)0xfed5)
#define PWMA_ARR        (*(uint16_t volatile __xdata *)0xfed2)
#define PWMA_CR1        (*(uint8_t volatile __xdata *)0xfec0)

#define P1SR        (*(uint8_t volatile __xdata *)0xFE21)
#define P3SR        (*(uint8_t volatile __xdata *)0xFE23)

static inline void clearbuffer(void)
{
#if DMA
	__xdata uint8_t* __data src = ((__xdata uint8_t*)&colorbuffer[bufferID].clear) + 1;
	__xdata uint8_t* __data dst = ((__xdata uint8_t*)&colorbuffer[bufferID].buffer[TILE_H - 1][TILE_W - 1]) + 1;
	for (uint8_t i = 0; i < 4; ++i)
	{
		DMA_M2M_TXAH = ((uint16_t)src) >> 8;
		DMA_M2M_TXAL = ((uint16_t)src) & 0xFF;
		DMA_M2M_RXAH = ((uint16_t)dst) >> 8;
		DMA_M2M_RXAL = ((uint16_t)dst) & 0xFF;
		DMA_M2M_CR = 0xC0;
		while (!(DMA_M2M_STA & 0x01));
		DMA_M2M_STA = 0;
		src -= 256;
		dst -= 256;
	}
#if DEPTH
	{
		__xdata uint8_t* __data src = ((__xdata uint8_t*) & depthbuffer.clear) + 1;
		__xdata uint8_t* __data dst = ((__xdata uint8_t*) & depthbuffer.buffer[TILE_H - 1][TILE_W - 1]) + 1;
		for (uint8_t i = 0; i < 4; ++i)
		{
			DMA_M2M_TXAH = ((uint16_t)src) >> 8;
			DMA_M2M_TXAL = ((uint16_t)src) & 0xFF;
			DMA_M2M_RXAH = ((uint16_t)dst) >> 8;
			DMA_M2M_RXAL = ((uint16_t)dst) & 0xFF;
			DMA_M2M_CR = 0xC0;
			while (!(DMA_M2M_STA & 0x01));
			DMA_M2M_STA = 0;
			src -= 256;
			dst -= 256;
		}
	}
#endif
#else
	for (uint8_t x = 0; x < TILE_W; x++)
	{
		for (uint8_t y = 0; y < TILE_H; y++)
		{
			colorbuffer[bufferID].buffer[y][x] = colorbuffer[bufferID].clear;
#if DEPTH
			depthbuffer.buffer[y][x] = depthbuffer.clear;
#endif
		}
	}
#endif
}
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
#if DMA && SPI
static uint8_t it_count;
static __xdata uint8_t* it_src;
static uint8_t it_pending;
static void spi_dma_it(void) __interrupt(49)
{
	DMA_SPI_STA = 0;
	if (it_count == 3)
	{
		LCD_CS_SET();
		SPSTAT = 0xC0;
		it_count = 0;
		it_pending = 0;
	}
	else
	{
		it_count++;
		it_src += 256;
		DMA_SPI_TXAH = ((uint16_t)it_src) >> 8;
		DMA_SPI_TXAL = ((uint16_t)it_src) & 0xFF;
		DMA_SPI_CR = 0xC0;
	}
}
static inline void resolvebuffer(void)
{
	while (it_pending == 1);
	it_pending = 1;
	LCD_Set_Address(32 + clipX0, clipY0, 32 + clipW - 1, clipH - 1);
	it_src = (__xdata uint8_t*)&colorbuffer[bufferID].buffer[0][0];
	LCD_CS_CLR();
	DMA_SPI_TXAH = ((uint16_t)it_src) >> 8;
	DMA_SPI_TXAL = ((uint16_t)it_src) & 0xFF;
	DMA_SPI_CR = 0xC0;
}
#else
static inline void resolvebuffer(void)
{
	LCD_Set_Address(32 + clipX0, clipY0, 32 + clipW - 1, clipH - 1);
	LCD_CS_CLR();
	for (uint8_t y = 0; y < TILE_H; ++y)
	{
		for (uint8_t x = 0; x < TILE_W; ++x)
		{
			uint16_t color = colorbuffer[bufferID].buffer[y][x];
			SPI_Send(color);
			SPI_Send(color >> 8);
		}
	}
	LCD_CS_SET();
}
#endif
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
#if DMA
	P_SW2 |= 0x80;
	DMA_M2M_CFG = 0x30;
	DMA_M2M_AMT = 0xFF;
#if SPI
	DMA_SPI_CFG = 0xC0;
	DMA_SPI_AMT = 0xFF;
	it_count = 0;
	EA = 1;
#endif
#endif

	LCD_Init();

	LCD_Clear();

#if SPI
	P1SR &= ~0x28;
	P3SR &= ~0xe0;
	SPCTL = 0xD3;
#endif

	angle = 0;

	bufferID = 0;

	colorbuffer[0].clear = ((uint16_t)BLACK >> 8) | ((uint16_t)BLACK << 8);
	colorbuffer[1].clear = ((uint16_t)BLACK >> 8) | ((uint16_t)BLACK << 8);
#if DEPTH
	depthbuffer.clear = 0;
#endif

	precompute_1();

	while (1)
	{
		precompute_2();
		makeTriangle();
		angle += 2;

#define M (LCD_W/TILE_W)
#define N (LCD_H/TILE_H)
		for (i = 0; i < M; ++i)
		{
			for (j = 0; j < N; ++j)
			{
				clipX0 = LCD_W * i / M;
				clipY0 = LCD_H * j / N;
				clipW = clipX0 + LCD_W / M;
				clipH = clipY0 + LCD_H / N;
				clearbuffer();
				drawTriangle();
				resolvebuffer();
				bufferID = 1 - bufferID;
			}
		}
	}
}
