/*
* Game Boy Advance Ray Tracing
* https://liuliqiang.com/51/3drt_gba.c
* https://liuliqiang.com/51/3drt_gba.gba
* by liuliqiang 2025-04-03
*/

#include <stdint.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

#define spp 1
#define max_depth 3

static uint8_t i, j, k;

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

static const float triangles[] =
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
    struct triangle_t* surface;
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
    interaction.surface = &((struct triangle_t*)triangles)[intersection.i];
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

static struct triangle_t* s;
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
            s = &((struct triangle_t*)triangles)[k];
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
    struct triangle_t* light = &((struct triangle_t*)triangles)[lightIdx];
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

    struct triangle_t* light = &((struct triangle_t*)triangles)[lightIdx];
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

__attribute__((section(".ewram")))
static struct vec3_t colorbuffer[LCD_W][LCD_H];
static uint32_t framecount;

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

    vec3_add(colorbuffer[X][Y], colorbuffer[X][Y], sample);

    vec3_assign(sample, colorbuffer[X][Y]);

    vec3_div_assign_s(sample, framecount);

    struct vec3_t one;
    vec3_assign_s(one, 1.0f);

    struct vec3_t sample_one;
    vec3_add(sample_one, sample, one);

    vec3_div_assign(sample, sample_one);
}

#define drawpixel(x, y, c) \
{ \
	((uint16_t*)0x06000000)[(y+(240-LCD_W)/2)*240+x+(160-LCD_H)/2] = c; \
}
static void draw3drt(void)
{
    samplePixel();
    const uint16_t r = sample.x * 0x1f;
    const uint16_t g = sample.y * 0x1f;
    const uint16_t b = sample.z * 0x1f;
    const uint16_t c16 = (b << 10) | (g << 5) | r;
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

int main(void)
{
    *((volatile uint32_t*)(0x04000000)) = 0x0003 | 0x0400;

    for (i = 0; i < 240; ++i)
    {
        for (j = 0; j < 160; ++j)
        {
            ((uint16_t*)0x06000000)[j * 240 + i] = 0;
        }
    }
    
    for (i = 0; i < LCD_W; ++i)
    {
        for (j = 0; j < LCD_H; ++j)
        {
            vec3_assign_s(colorbuffer[i][j], 0.0f);
        }
    }

    precompute_rt();

    while (1)
    {
        if (X == 0 && Y == 0)
        {
            framecount += spp;
        }
        draw3drt();
    }

    return 0;
}
