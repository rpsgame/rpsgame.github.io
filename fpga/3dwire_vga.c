/*
* sdcc 3dwire.c --model-large
*/

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define W 160
#define H 120

__sfr __at(0xc1) uart;
__sfr __at(0xc2) vgacolorL;
__sfr __at(0xc4) vgacolorH;

int putchar(int c)
{
	while (uart);
	uart = c;
	while (uart);
	return c;
}

static uint8_t buffer[20][120];
static inline void drawpixel(int16_t x, int16_t y)
{
	if (x < 0 || x > W - 1 || y < 0 || y > H - 1)
		return;
	buffer[x >> 3][y] |= (uint8_t)1 << (x & 7);
}
static inline void clear(void)
{
	for (uint8_t x = 0; x < 20; ++x)
	{
		for (uint8_t y = 0; y < 120; ++y)
		{
			buffer[x][y] = 0;
		}
	}
}
static inline int16_t Abs(int16_t x)
{
	return x > 0 ? x : -x;
}
static void drawline(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	int dx = Abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -Abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
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
	printf_small("Hello, world!\n");

#define VGACOLOR 0X9b43
	//vgacolorL = VGACOLOR & 0xff;
	//vgacolorH = (VGACOLOR >> 8) & 0xff;

	uint16_t vgacolor;
	vgacolor = 0;

	uint8_t r, g, b;
	r = (VGACOLOR >> 11) & 0x1f;
	g = (VGACOLOR >> 5) & 0x3f;
	b = (VGACOLOR >> 0) & 0x1f;

	uint8_t rf, gf, bf;
	rf = 0;	gf = 0;	bf = 0;

	angle = 0;
	precompute();

	while (1)
	{
		if (r == 0) rf = 0;
		if (g == 0) gf = 0;
		if (b == 0) bf = 0;

		if (r == 0x1f) rf = 1;
		if (g == 0x3f) gf = 1;
		if (b == 0x1f) bf = 1;

		if (rf) r--; else r++;
		if (gf) g--; else g++;
		if (bf) b--; else b++;

		vgacolor = ((uint16_t)r << 11) | ((uint16_t)g << 5) | (uint16_t)b;
		vgacolorL = vgacolor & 0xff;
		vgacolorH = (vgacolor >> 8) & 0xff;

		clear();
		draw3d();
		angle += 2;

		for (uint8_t x = 0; x < 20; ++x)
		{
			for (uint8_t y = 0; y < 120; ++y)
			{
				(*(uint8_t volatile __xdata*)((uint16_t)0x8000 + (uint16_t)x + (uint16_t)y * 256)) = buffer[x][y];
			}
		}
	}
}
