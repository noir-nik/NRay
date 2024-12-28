#ifdef USE_MODULES
module;
#define LMATH_EXPORT export 
#else
#pragma once
#define LMATH_EXPORT
#endif

#include <cmath>
#include <cstring>
#include <tuple>
#include "Lmath_types.h"

#ifdef USE_MODULES
export module Lmath;
#endif

#if defined(_MSC_VER)
#define CVEX_ALIGNED(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define CVEX_ALIGNED(x) __attribute__ ((aligned(x)))
#endif
#endif

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

LMATH_EXPORT
namespace Lmath {

using Lmath::uint;
using Lmath::uchar;

using Lmath::int2;
using Lmath::int3;
using Lmath::int4;
using Lmath::uint2;
using Lmath::uint3;
using Lmath::uint4;
using Lmath::float2;
using Lmath::float3;
using Lmath::float4;

using Lmath::float3x3;
using Lmath::float4x4;

using Lmath::ivec2;
using Lmath::ivec3;
using Lmath::ivec4;
using Lmath::uvec2;
using Lmath::uvec3;
using Lmath::uvec4;
using Lmath::vec2;
using Lmath::vec3;
using Lmath::vec4;

using Lmath::mat3;
using Lmath::mat4;

inline int   min(const int   a, const int   b) { return (a < b) ? a : b; }
inline int   max(const int   a, const int   b) { return (a > b) ? a : b; }
inline uint  min(const uint  a, const uint  b) { return (a < b) ? a : b; }
inline uint  max(const uint  a, const uint  b) { return (a > b) ? a : b; }
inline float min(const float a, const float b) { return (a < b) ? a : b; }
inline float max(const float a, const float b) { return (a > b) ? a : b; }

inline int   clamp(int u,   int a,   int b)   { return min(max(a, u), b); }
inline uint  clamp(uint u,  uint a,  uint b)  { return min(max(a, u), b); }
inline float clamp(float u, float a, float b) { return min(max(a, u), b); }

constexpr inline float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;

inline int4 operator+(const int4 a, const int4 b) { return int4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
inline int4 operator-(const int4 a, const int4 b) { return int4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
inline int4 operator*(const int4 a, const int4 b) { return int4{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
inline int4 operator/(const int4 a, const int4 b) { return int4{a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }

inline int4 operator * (const int4 a, int b) { return int4{a.x * b, a.y * b, a.z * b, a.w * b}; }
inline int4 operator / (const int4 a, int b) { return int4{a.x / b, a.y / b, a.z / b, a.w / b}; }
inline int4 operator * (int a, const int4 b) { return int4{a * b.x, a * b.y, a * b.z, a * b.w}; }
inline int4 operator / (int a, const int4 b) { return int4{a / b.x, a / b.y, a / b.z, a / b.w}; }

inline int4 operator + (const int4 a, int b) { return int4{a.x + b, a.y + b, a.z + b, a.w + b}; }
inline int4 operator - (const int4 a, int b) { return int4{a.x - b, a.y - b, a.z - b, a.w - b}; }
inline int4 operator + (int a, const int4 b) { return int4{a + b.x, a + b.y, a + b.z, a + b.w}; }
inline int4 operator - (int a, const int4 b) { return int4{a - b.x, a - b.y, a - b.z, a - b.w}; }

inline int4& operator *= (int4& a, const int4 b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;  return a; }
inline int4& operator /= (int4& a, const int4 b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w;  return a; }
inline int4& operator *= (int4& a, int b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b;  return a; }
inline int4& operator /= (int4& a, int b) { a.x /= b; a.y /= b; a.z /= b; a.w /= b;  return a; }

inline int4& operator += (int4& a, const int4 b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;  return a; }
inline int4& operator -= (int4& a, const int4 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;  return a; }
inline int4& operator += (int4& a, int b) { a.x += b; a.y += b; a.z += b; a.w += b;  return a; }
inline int4& operator -= (int4& a, int b) { a.x -= b; a.y -= b; a.z -= b; a.w -= b;  return a; }

inline void store (int* p, const int4 a_val)  { memcpy((void*)p, (void*)&a_val, sizeof(int)*4); }
inline void load  (int4& dst, const int* src) { memcpy((void*)&dst, (void*)src, sizeof(int)*4); }


inline float2 operator+(const float2 a, const float2 b) { return float2{a.x + b.x, a.y + b.y}; }
inline float2 operator-(const float2 a, const float2 b) { return float2{a.x - b.x, a.y - b.y}; }


inline float3 operator+(const float3 a, const float3 b) { return float3{a.x + b.x, a.y + b.y, a.z + b.z}; }
inline float3 operator-(const float3 a, const float3 b) { return float3{a.x - b.x, a.y - b.y, a.z - b.z}; }
inline float3 operator*(const float3 a, const float3 b) { return float3{a.x * b.x, a.y * b.y, a.z * b.z}; }
inline float3 operator/(const float3 a, const float3 b) { return float3{a.x / b.x, a.y / b.y, a.z / b.z}; }

inline float3 operator * (const float3 a, float b) { return float3{a.x * b, a.y * b, a.z * b}; }
inline float3 operator / (const float3 a, float b) { return float3{a.x / b, a.y / b, a.z / b}; }
inline float3 operator * (float a, const float3 b) { return float3{a * b.x, a * b.y, a * b.z}; }
inline float3 operator / (float a, const float3 b) { return float3{a / b.x, a / b.y, a / b.z}; }

inline float3 operator + (const float3 a, float b) { return float3{a.x + b, a.y + b, a.z + b}; }
inline float3 operator - (const float3 a, float b) { return float3{a.x - b, a.y - b, a.z - b}; }
inline float3 operator + (float a, const float3 b) { return float3{a + b.x, a + b.y, a + b.z}; }
inline float3 operator - (float a, const float3 b) { return float3{a - b.x, a - b.y, a - b.z}; }

inline float3& operator *= (float3& a, const float3 b) { a.x *= b.x; a.y *= b.y; a.z *= b.z;  return a; }
inline float3& operator /= (float3& a, const float3 b) { a.x /= b.x; a.y /= b.y; a.z /= b.z;  return a; }
inline float3& operator *= (float3& a, float b) { a.x *= b; a.y *= b; a.z *= b;  return a; }
inline float3& operator /= (float3& a, float b) { a.x /= b; a.y /= b; a.z /= b;  return a; }

inline float3& operator += (float3& a, const float3 b) { a.x += b.x; a.y += b.y; a.z += b.z;  return a; }
inline float3& operator -= (float3& a, const float3 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z;  return a; }
inline float3& operator += (float3& a, float b) { a.x += b; a.y += b; a.z += b;  return a; }
inline float3& operator -= (float3& a, float b) { a.x -= b; a.y -= b; a.z -= b;  return a; }

inline float3 operator -(const float3 a) { return float3{-a.x, -a.y, -a.z}; }

inline void store  (float* p, const float3 a_val) { memcpy((void*)p, (void*)&a_val, sizeof(float)*3); }
inline void store_u(float* p, const float3 a_val) { memcpy((void*)p, (void*)&a_val, sizeof(float)*3); }  

inline void load (float3& dst, const float* src) { memcpy((void*)&dst, (void*)src, sizeof(float)*3); }

inline float3 min  (const float3 a, const float3 b) { return float3{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)}; }
inline float3 max  (const float3 a, const float3 b) { return float3{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)}; }
inline float3 clamp(const float3 u, const float3 a, const float3 b) { return float3{clamp(u.x, a.x, b.x), clamp(u.y, a.y, b.y), clamp(u.z, a.z, b.z)}; }
inline float3 clamp(const float3 u, float a, float b) { return float3{clamp(u.x, a, b), clamp(u.y, a, b), clamp(u.z, a, b)}; }

inline  float dot(const float3 a, const float3 b)  { return a.x*b.x + a.y*b.y + a.z*b.z; }

inline  float length(const float3 a) { return std::sqrt(dot(a,a)); }
inline  float3 normalize(const float3 a) { float lenInv = float(1)/length(a); return a*lenInv; }

inline float3 shuffle_xzy(float3 a) { return float3{a.x, a.z, a.y}; }
inline float3 shuffle_yxz(float3 a) { return float3{a.y, a.x, a.z}; }
inline float3 shuffle_yzx(float3 a) { return float3{a.y, a.z, a.x}; }
inline float3 shuffle_zxy(float3 a) { return float3{a.z, a.x, a.y}; }
inline float3 shuffle_zyx(float3 a) { return float3{a.z, a.y, a.x}; }
inline float3 cross(const float3 a, const float3 b) 
{
	const float3 a_yzx = shuffle_yzx(a);
	const float3 b_yzx = shuffle_yzx(b);
	return shuffle_yzx(a*b_yzx - a_yzx*b);
}


inline bool operator==(const int4& a, const int4& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
inline bool operator!=(const int4& a, const int4& b) { return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w; }

inline float4 operator+(const float4 a, const float4 b) { return float4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
inline float4 operator-(const float4 a, const float4 b) { return float4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
inline float4 operator*(const float4 a, const float4 b) { return float4{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
inline float4 operator/(const float4 a, const float4 b) { return float4{a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }

inline float4 operator * (const float4 a, float b) { return float4{a.x * b, a.y * b, a.z * b, a.w * b}; }
inline float4 operator / (const float4 a, float b) { return float4{a.x / b, a.y / b, a.z / b, a.w / b}; }
inline float4 operator * (float a, const float4 b) { return float4{a * b.x, a * b.y, a * b.z, a * b.w}; }
inline float4 operator / (float a, const float4 b) { return float4{a / b.x, a / b.y, a / b.z, a / b.w}; }

inline float4 operator + (const float4 a, float b) { return float4{a.x + b, a.y + b, a.z + b, a.w + b}; }
inline float4 operator - (const float4 a, float b) { return float4{a.x - b, a.y - b, a.z - b, a.w - b}; }
inline float4 operator + (float a, const float4 b) { return float4{a + b.x, a + b.y, a + b.z, a + b.w}; }
inline float4 operator - (float a, const float4 b) { return float4{a - b.x, a - b.y, a - b.z, a - b.w}; }

inline float4& operator *= (float4& a, const float4 b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;  return a; }
inline float4& operator /= (float4& a, const float4 b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w;  return a; }
inline float4& operator *= (float4& a, float b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b;  return a; }
inline float4& operator /= (float4& a, float b) { a.x /= b; a.y /= b; a.z /= b; a.w /= b;  return a; }

inline float4& operator += (float4& a, const float4 b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;  return a; }
inline float4& operator -= (float4& a, const float4 b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;  return a; }
inline float4& operator += (float4& a, float b) { a.x += b; a.y += b; a.z += b; a.w += b;  return a; }
inline float4& operator -= (float4& a, float b) { a.x -= b; a.y -= b; a.z -= b; a.w -= b;  return a; }

inline bool operator==(const float4 a, const float4 b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

inline void store  (float* dst, const float4 src) { memcpy((void*)dst, (void*)&src, sizeof(float)*4); }
inline void store_u(float* dst, const float4 src) { memcpy((void*)dst, (void*)&src, sizeof(float)*4); }

inline void load(float4& dst, const float* src) { memcpy((void*)&dst, (void*)src, sizeof(float)*4); }

inline float4 min  (const float4 a, const float4 b) { return float4{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)}; }
inline float4 max  (const float4 a, const float4 b) { return float4{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)}; }
inline float4 clamp(const float4 u, const float4 a, const float4 b) { return float4{clamp(u.x, a.x, b.x), clamp(u.y, a.y, b.y), clamp(u.z, a.z, b.z), clamp(u.w, a.w, b.w)}; }
inline float4 clamp(const float4 u, float a, float b) { return float4{clamp(u.x, a, b), clamp(u.y, a, b), clamp(u.z, a, b), clamp(u.w, a, b)}; }
inline float4 round(const float4 a)                 { return float4{std::round(a.x), std::round(a.y), std::round(a.z), std::round(a.w)}; }
// inline float4 abs (const float4 a) { return float4{std::abs(a.x), std::abs(a.y), std::abs(a.z), std::abs(a.w)}; } 
// inline float4 sign(const float4 a) { return float4{sign(a.x), sign(a.y), sign(a.z), sign(a.w)}; }

inline float4 lerp (const float4 a, const float4 b, float t) { return a + t * (b - a); }
inline float4 mix  (const float4 a, const float4 b, float t) { return a + t * (b - a); }
inline float4 floor(const float4 a)                 { return float4{std::floor(a.x), std::floor(a.y), std::floor(a.z), std::floor(a.w)}; }
inline float4 ceil (const float4 a)                 { return float4{std::ceil(a.x), std::ceil(a.y), std::ceil(a.z), std::ceil(a.w)}; }
inline float4 rcp  (const float4 a)                 { return float4{1.0f/a.x, 1.0f/a.y, 1.0f/a.z, 1.0f/a.w}; }
inline float4 mod  (const float4 x, const float4 y) { return x - y * floor(x/y); }
inline float4 fract(const float4 x)                 { return x - floor(x); }
inline float4 sqrt (const float4 a)                 { return float4{std::sqrt(a.x), std::sqrt(a.y), std::sqrt(a.z), std::sqrt(a.w)}; }
inline float4 inversesqrt(const float4 a)           { return 1.0f/sqrt(a); }

inline  float dot(const float4 a, const float4 b)  { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

inline  float length(const float4 a) { return std::sqrt(dot(a,a)); }


inline uint packRGBA8(float4 const& v) { 
	uvec4 res = uvec4(round(clamp(v, 0.0f, 1.0f) * 255.0f));
	return res.x | (res.y << 8) | (res.z << 16) | (res.w << 24);
}

inline float4 unpackRGBA8(uint rgba) { 
	uvec4 res = uvec4(rgba);
	return float4(res.x/255.0f, res.y/255.0f, res.z/255.0f, res.w/255.0f);
}

template<typename T, typename Ret>
inline Ret operator|(T const& vec, Ret (*func)(T const&)) { return func(vec); }

inline void mat4_colmajor_mul_vec4(float* __restrict RES, const float* __restrict B, const float* __restrict V)
{
	RES[0] = V[0] * B[0] + V[1] * B[4] + V[2] * B[ 8] + V[3] * B[12];
	RES[1] = V[0] * B[1] + V[1] * B[5] + V[2] * B[ 9] + V[3] * B[13];
	RES[2] = V[0] * B[2] + V[1] * B[6] + V[2] * B[10] + V[3] * B[14];
	RES[3] = V[0] * B[3] + V[1] * B[7] + V[2] * B[11] + V[3] * B[15];
}

inline void transpose4(const float4 in_rows[4], float4 out_rows[4]) // SIMD 
{
	CVEX_ALIGNED(16) float rows[16];
	store(rows+0,  in_rows[0]);
	store(rows+4,  in_rows[1]);
	store(rows+8,  in_rows[2]);
	store(rows+12, in_rows[3]);

	out_rows[0] = float4{rows[0], rows[4], rows[8],  rows[12]};
	out_rows[1] = float4{rows[1], rows[5], rows[9],  rows[13]};
	out_rows[2] = float4{rows[2], rows[6], rows[10], rows[14]};
	out_rows[3] = float4{rows[3], rows[7], rows[11], rows[15]};
}

float3x3 inverse3x3(const float3x3& m);

inline float4x4 inverse(const float4x4& m1);
inline float4x4 transpose(const float4x4& m1);
inline float4x4 affineInverse(const float4x4& m);
inline void decompose(float4x4 const& m, float3& translation, float3& rotation, float3& scale);



inline float3x3 inverse3x3(const float3x3& m) 
{
	float a = m.m_col[0].x, b = m.m_col[0].y, c = m.m_col[0].z;
	float d = m.m_col[1].x, e = m.m_col[1].y, f = m.m_col[1].z;
	float g = m.m_col[2].x, h = m.m_col[2].y, i = m.m_col[2].z;

	float det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);

	float inv_det = 1.0f / det;

	float3x3 res;

	res.m_col[0].x = (e * i - f * h) * inv_det;
	res.m_col[0].y = (c * h - b * i) * inv_det;
	res.m_col[0].z = (b * f - c * e) * inv_det;

	res.m_col[1].x = (f * g - d * i) * inv_det;
	res.m_col[1].y = (a * i - c * g) * inv_det;
	res.m_col[1].z = (c * d - a * f) * inv_det;

	res.m_col[2].x = (d * h - e * g) * inv_det;
	res.m_col[2].y = (b * g - a * h) * inv_det;
	res.m_col[2].z = (a * e - b * d) * inv_det;

	return res;
}

inline void mat3_colmajor_mul_vec3(float* __restrict RES, const float* __restrict B, const float* __restrict V)
{
	RES[0] = V[0] * B[0] + V[1] * B[3] + V[2] * B[ 6];
	RES[1] = V[0] * B[1] + V[1] * B[4] + V[2] * B[ 7];
	RES[2] = V[0] * B[2] + V[1] * B[5] + V[2] * B[ 8];
}

inline float3 mul(const float3x3& m, const float3& v)
{
	float3 res;
	mat3_colmajor_mul_vec3((float*)&res, (const float*)&m, (const float*)&v);
	return res;
}

inline float3 operator*(const float3x3& m, const float3& v) { return mul(m, v); }

inline float4 mul(const float4x4& m, const float4& v)
{
	float4 res;
	mat4_colmajor_mul_vec4((float*)&res, (const float*)&m, (const float*)&v);
	return res;
}


inline float4x4 translate4x4(float3 t)
{
	float4x4 res;
	res.set_col(3, float4{t.x,  t.y,  t.z, 1.0f });
	return res;
}

inline float4x4 scale4x4(float3 t)
{
	float4x4 res;
	res.set_col(0, float4{t.x, 0.0f, 0.0f,  0.0f});
	res.set_col(1, float4{0.0f, t.y, 0.0f,  0.0f});
	res.set_col(2, float4{0.0f, 0.0f,  t.z, 0.0f});
	res.set_col(3, float4{0.0f, 0.0f, 0.0f, 1.0f});
	return res;
}

inline float4x4 rotate4x4X(float phi)
{
	float4x4 res;
	res.set_col(0, float4{1.0f,      0.0f,       0.0f, 0.0f  });
	res.set_col(1, float4{0.0f, +cosf(phi),  +sinf(phi), 0.0f});
	res.set_col(2, float4{0.0f, -sinf(phi),  +cosf(phi), 0.0f});
	res.set_col(3, float4{0.0f,      0.0f,       0.0f, 1.0f  });
	return res;
}

inline float4x4 rotate4x4Y(float phi)
{
	float4x4 res;
	res.set_col(0, float4{+cosf(phi), 0.0f, -sinf(phi), 0.0f});
	res.set_col(1, float4{     0.0f, 1.0f,      0.0f, 0.0f  });
	res.set_col(2, float4{+sinf(phi), 0.0f, +cosf(phi), 0.0f});
	res.set_col(3, float4{     0.0f, 0.0f,      0.0f, 1.0f  });
	return res;
}

inline float4x4 rotate4x4Z(float phi)
{
	float4x4 res;
	res.set_col(0, float4{+cosf(phi), sinf(phi), 0.0f, 0.0f});
	res.set_col(1, float4{-sinf(phi), cosf(phi), 0.0f, 0.0f});
	res.set_col(2, float4{     0.0f,     0.0f, 1.0f, 0.0f  });
	res.set_col(3, float4{     0.0f,     0.0f, 0.0f, 1.0f  });
	return res;
}

// Rotation around an arbitrary axis
inline float4x4 rotate4x4(float3 axis, float angle)
{
	axis = normalize(axis);
	float c = cosf(angle);
	float s = sinf(angle);
	float omc = 1.0f - c;
	float4x4 res;
	res.set_col(0, float4{
		axis.x * axis.x * omc + c,
		axis.y * axis.x * omc + axis.z * s,
		axis.z * axis.x * omc - axis.y * s,
		0.0f});
	res.set_col(1, float4{
		axis.x * axis.y * omc - axis.z * s,
		axis.y * axis.y * omc + c,
		axis.z * axis.y * omc + axis.x * s,
		0.0f});
	res.set_col(2, float4{
		axis.x * axis.z * omc + axis.y * s,
		axis.y * axis.z * omc - axis.x * s,
		axis.z * axis.z * omc + c,
		0.0f});
	res.set_col(3, float4{0.0f, 0.0f, 0.0f, 1.0f});
	return res;
}

inline float4x4 mul(float4x4 m1, float4x4 m2)
{
	const float4 column1 = mul(m1, m2.col(0));
	const float4 column2 = mul(m1, m2.col(1));
	const float4 column3 = mul(m1, m2.col(2));
	const float4 column4 = mul(m1, m2.col(3));
	float4x4 res;
	res.set_col(0, column1);
	res.set_col(1, column2);
	res.set_col(2, column3);
	res.set_col(3, column4);

	return res;
}

inline float4x4 operator*(float4x4 m1, float4x4 m2)
{
	const float4 column1 = mul(m1, m2.col(0));
	const float4 column2 = mul(m1, m2.col(1));
	const float4 column3 = mul(m1, m2.col(2));
	const float4 column4 = mul(m1, m2.col(3));

	float4x4 res;
	res.set_col(0, column1);
	res.set_col(1, column2);
	res.set_col(2, column3);
	res.set_col(3, column4);
	return res;
}

/* 
inline float4x4 transpose(const float4x4& rhs)
{
	float4x4 res;
	transpose4(rhs.m_col, res.m_col);
	return res;
}
 */
inline float4x4 transpose(const float4x4& m1)
{
	float4x4 res;
	res.set_row(0, float4{m1(0, 0), m1(1, 0), m1(2, 0), m1(3, 0)});
	res.set_row(1, float4{m1(0, 1), m1(1, 1), m1(2, 1), m1(3, 1)});
	res.set_row(2, float4{m1(0, 2), m1(1, 2), m1(2, 2), m1(3, 2)});
	res.set_row(3, float4{m1(0, 3), m1(1, 3), m1(2, 3), m1(3, 3)});
	return res;
}

inline float4x4 inverse(const float4x4& m1)
{
	CVEX_ALIGNED(16) float tmp[12]; // temp array for pairs
	float4x4 m;

	// calculate pairs for first 8 elements (cofactors)
	//
	tmp[0]  = m1(2,2) * m1(3,3);
	tmp[1]  = m1(2,3) * m1(3,2);
	tmp[2]  = m1(2,1) * m1(3,3);
	tmp[3]  = m1(2,3) * m1(3,1);
	tmp[4]  = m1(2,1) * m1(3,2);
	tmp[5]  = m1(2,2) * m1(3,1);
	tmp[6]  = m1(2,0) * m1(3,3);
	tmp[7]  = m1(2,3) * m1(3,0);
	tmp[8]  = m1(2,0) * m1(3,2);
	tmp[9]  = m1(2,2) * m1(3,0);
	tmp[10] = m1(2,0) * m1(3,1);
	tmp[11] = m1(2,1) * m1(3,0);

	// calculate first 8 m1.rowents (cofactors)
	//
	m(0,0) = tmp[0]  * m1(1,1) + tmp[3] * m1(1,2) + tmp[4]  * m1(1,3);
	m(0,0) -= tmp[1] * m1(1,1) + tmp[2] * m1(1,2) + tmp[5]  * m1(1,3);
	m(1,0) = tmp[1]  * m1(1,0) + tmp[6] * m1(1,2) + tmp[9]  * m1(1,3);
	m(1,0) -= tmp[0] * m1(1,0) + tmp[7] * m1(1,2) + tmp[8]  * m1(1,3);
	m(2,0) = tmp[2]  * m1(1,0) + tmp[7] * m1(1,1) + tmp[10] * m1(1,3);
	m(2,0) -= tmp[3] * m1(1,0) + tmp[6] * m1(1,1) + tmp[11] * m1(1,3);
	m(3,0) = tmp[5]  * m1(1,0) + tmp[8] * m1(1,1) + tmp[11] * m1(1,2);
	m(3,0) -= tmp[4] * m1(1,0) + tmp[9] * m1(1,1) + tmp[10] * m1(1,2);
	m(0,1) = tmp[1]  * m1(0,1) + tmp[2] * m1(0,2) + tmp[5]  * m1(0,3);
	m(0,1) -= tmp[0] * m1(0,1) + tmp[3] * m1(0,2) + tmp[4]  * m1(0,3);
	m(1,1) = tmp[0]  * m1(0,0) + tmp[7] * m1(0,2) + tmp[8]  * m1(0,3);
	m(1,1) -= tmp[1] * m1(0,0) + tmp[6] * m1(0,2) + tmp[9]  * m1(0,3);
	m(2,1) = tmp[3]  * m1(0,0) + tmp[6] * m1(0,1) + tmp[11] * m1(0,3);
	m(2,1) -= tmp[2] * m1(0,0) + tmp[7] * m1(0,1) + tmp[10] * m1(0,3);
	m(3,1) = tmp[4]  * m1(0,0) + tmp[9] * m1(0,1) + tmp[10] * m1(0,2);
	m(3,1) -= tmp[5] * m1(0,0) + tmp[8] * m1(0,1) + tmp[11] * m1(0,2);

	// calculate pairs for second 8 m1.rowents (cofactors)
	//
	tmp[0]  = m1(0,2) * m1(1,3);
	tmp[1]  = m1(0,3) * m1(1,2);
	tmp[2]  = m1(0,1) * m1(1,3);
	tmp[3]  = m1(0,3) * m1(1,1);
	tmp[4]  = m1(0,1) * m1(1,2);
	tmp[5]  = m1(0,2) * m1(1,1);
	tmp[6]  = m1(0,0) * m1(1,3);
	tmp[7]  = m1(0,3) * m1(1,0);
	tmp[8]  = m1(0,0) * m1(1,2);
	tmp[9]  = m1(0,2) * m1(1,0);
	tmp[10] = m1(0,0) * m1(1,1);
	tmp[11] = m1(0,1) * m1(1,0);

	// calculate second 8 m1 (cofactors)
	//
	m(0,2) = tmp[0]   * m1(3,1) + tmp[3]  * m1(3,2) + tmp[4]  * m1(3,3);
	m(0,2) -= tmp[1]  * m1(3,1) + tmp[2]  * m1(3,2) + tmp[5]  * m1(3,3);
	m(1,2) = tmp[1]   * m1(3,0) + tmp[6]  * m1(3,2) + tmp[9]  * m1(3,3);
	m(1,2) -= tmp[0]  * m1(3,0) + tmp[7]  * m1(3,2) + tmp[8]  * m1(3,3);
	m(2,2) = tmp[2]   * m1(3,0) + tmp[7]  * m1(3,1) + tmp[10] * m1(3,3);
	m(2,2) -= tmp[3]  * m1(3,0) + tmp[6]  * m1(3,1) + tmp[11] * m1(3,3);
	m(3,2) = tmp[5]   * m1(3,0) + tmp[8]  * m1(3,1) + tmp[11] * m1(3,2);
	m(3,2) -= tmp[4]  * m1(3,0) + tmp[9]  * m1(3,1) + tmp[10] * m1(3,2);
	m(0,3) = tmp[2]   * m1(2,2) + tmp[5]  * m1(2,3) + tmp[1]  * m1(2,1);
	m(0,3) -= tmp[4]  * m1(2,3) + tmp[0]  * m1(2,1) + tmp[3]  * m1(2,2);
	m(1,3) = tmp[8]   * m1(2,3) + tmp[0]  * m1(2,0) + tmp[7]  * m1(2,2);
	m(1,3) -= tmp[6]  * m1(2,2) + tmp[9]  * m1(2,3) + tmp[1]  * m1(2,0);
	m(2,3) = tmp[6]   * m1(2,1) + tmp[11] * m1(2,3) + tmp[3]  * m1(2,0);
	m(2,3) -= tmp[10] * m1(2,3) + tmp[2]  * m1(2,0) + tmp[7]  * m1(2,1);
	m(3,3) = tmp[10]  * m1(2,2) + tmp[4]  * m1(2,0) + tmp[9]  * m1(2,1);
	m(3,3) -= tmp[8]  * m1(2,1) + tmp[11] * m1(2,2) + tmp[5]  * m1(2,0);

	// calculate matrix inverse
	//
	const float k = 1.0f / (m1(0,0) * m(0,0) + m1(0,1) * m(1,0) + m1(0,2) * m(2,0) + m1(0,3) * m(3,0));
	const float4 vK{k,k,k,k};

	m.set_col(0, m.get_col(0)*vK);
	m.set_col(1, m.get_col(1)*vK);
	m.set_col(2, m.get_col(2)*vK);
	m.set_col(3, m.get_col(3)*vK);

	return m;
}

inline float4x4 affineInverse(const float4x4& m) {
	auto inv = inverse3x3(m);
	auto l = -(inv * m.m_col[3].xyz());
	float4x4 res;
	res.m_col[0] = float4(inv.m_col[0],  0.0f);
	res.m_col[1] = float4(inv.m_col[1],  0.0f);
	res.m_col[2] = float4(inv.m_col[2],  0.0f);
	res.m_col[3] = float4(l.x, l.y, l.z, 1.0f);
	return res;
}


/* 
	Utility functions
*/

// Look At matrix creation
// return the inverse view matrix
//
inline float4x4 lookAt1(float3 eye, float3 center, float3 up) {
	const float3 f = normalize(center - eye);
	const float3 s = normalize(cross(f, up));
	const float3 u = cross(s, f);
	float4x4 res;
	res.m_col[0] = float4{ s.x, u.x, -f.x, 0.0f };
	res.m_col[1] = float4{ s.y, u.y, -f.y, 0.0f };
	res.m_col[2] = float4{ s.z, u.z, -f.z, 0.0f };
	res.m_col[3] = float4{ -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0f };
	return res;
}

inline float4x4 lookAtInv(float3 eye, float3 center, float3 up) {
	const float3 f = normalize(center - eye);
	const float3 s = normalize(cross(f, up));
	const float3 u = cross(s, f);
	float4x4 res;
	res.m_col[0] = float4{ s.x, s.y, s.z, dot(s, eye) };
	res.m_col[1] = float4{ u.x, u.y, u.z, dot(u, eye) };
	res.m_col[2] = float4{ f.x, f.y, f.z, -dot(f, eye) };
	res.m_col[3] = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
	return res;
}

inline float4x4 lookAt(float3 eye, float3 center, float3 up)
{
	float3 x, y, z; // basis; will make a rotation matrix
	z.x = eye.x - center.x;
	z.y = eye.y - center.y;
	z.z = eye.z - center.z;
	z = normalize(z);
	y.x = up.x;
	y.y = up.y;
	y.z = up.z;
	x = cross(y, z); // X floattor = Y cross Z
	y = cross(z, x); // Recompute Y = Z cross X
	// cross product gives area of parallelogram, which is < 1.0 for
	// non-perpendicular unit-length floattors; so normalize x, y here
	x = normalize(x);
	y = normalize(y);
	float4x4 M;
	M.set_col(0, float4{ x.x, y.x, z.x, 0.0f });
	M.set_col(1, float4{ x.y, y.y, z.y, 0.0f });
	M.set_col(2, float4{ x.z, y.z, z.z, 0.0f });
	M.set_col(3, float4{ -x.x * eye.x - x.y * eye.y - x.z*eye.z,
						-y.x * eye.x - y.y * eye.y - y.z*eye.z,
						-z.x * eye.x - z.y * eye.y - z.z*eye.z,
						1.0f });
	return M;
}

inline float4x4 perspective(float const fov, float const aspect, float const zNear, float const zFar, bool useFovX = true)
{
	float ymax, xmax;

	if (useFovX) {
		// Using horizontal field of view (fovx)
		xmax = zNear * tanf(fov * DEG_TO_RAD / 2.0f);
		ymax = xmax / aspect;
	} else {
		// Using vertical field of view (fovy)
		ymax = zNear * tanf(fov * DEG_TO_RAD / 2.0f);
		xmax = ymax * aspect;
	}

	const float left = -xmax;
	const float right = +xmax;
	const float bottom = -ymax;
	const float top = +ymax;
	const float temp = 2.0f * zNear;
	const float temp2 = right - left;
	const float temp3 = top - bottom;
	const float temp4 = zFar - zNear;
	float4x4 res;
	res.m_col[0] = float4{ temp / temp2, 0.0f, 0.0f, 0.0f };
	res.m_col[1] = float4{ 0.0f, temp / temp3, 0.0f, 0.0f };
	res.m_col[2] = float4{ (right + left) / temp2,  (top + bottom) / temp3, (-zFar - zNear) / temp4, -1.0 };
	res.m_col[3] = float4{ 0.0f, 0.0f, (-temp * zFar) / temp4, 0.0f };
	return res;
}

inline float4x4 perspectiveX(float fovX, float aspect, float zNear, float zFar)
{
	float tanHalfFov = tanf(fovX * DEG_TO_RAD / 2.0f);
	float4x4 result;
	result(0, 0) = 1.0f / tanHalfFov;
	result(1, 1) = aspect * 1.0f / (tanHalfFov);
	result(2, 2) = -(zFar + zNear) / (zFar - zNear);
	result(3, 2) = -1.0f;
	result(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
	result(3, 3) = 0.0f;
	return result;
}

inline float4x4 perspectiveY(float fovY, float aspect, float zNear, float zFar)
{
	float tanHalfFov = tanf(fovY * DEG_TO_RAD / 2.0f);
	float4x4 result;
	result(0, 0) = aspect * 1.0f / (tanHalfFov);
	result(1, 1) = 1.0f / tanHalfFov;
	result(2, 2) = -(zFar + zNear) / (zFar - zNear);
	result(3, 2) = -1.0f;
	result(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
	result(3, 3) = 0.0f;
	return result;
}

inline void decompose(float4x4 const& m, float3& translation, float3& rotation, float3& scale)
{
    // Extract translation
    translation.x = m.m_col[3].x;
    translation.y = m.m_col[3].y;
    translation.z = m.m_col[3].z;

    // Extract scale
    scale.x = length(float3{ m.m_col[0].x, m.m_col[0].y, m.m_col[0].z });
    scale.y = length(float3{ m.m_col[1].x, m.m_col[1].y, m.m_col[1].z });
    scale.z = length(float3{ m.m_col[2].x, m.m_col[2].y, m.m_col[2].z });

    // Remove scale from the matrix to extract rotation
    float4x4 rotationMatrix = m;
    rotationMatrix.m_col[0].x /= scale.x;
    rotationMatrix.m_col[0].y /= scale.x;
    rotationMatrix.m_col[0].z /= scale.x;

    rotationMatrix.m_col[1].x /= scale.y;
    rotationMatrix.m_col[1].y /= scale.y;
    rotationMatrix.m_col[1].z /= scale.y;

    rotationMatrix.m_col[2].x /= scale.z;
    rotationMatrix.m_col[2].y /= scale.z;
    rotationMatrix.m_col[2].z /= scale.z;

    // Extract rotation angles from the rotation matrix
    rotation.y = asin(-rotationMatrix.m_col[2].x);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(rotationMatrix.m_col[2].y, rotationMatrix.m_col[2].z);
        rotation.z = atan2(rotationMatrix.m_col[1].x, rotationMatrix.m_col[0].x);
    } else {
        rotation.x = atan2(-rotationMatrix.m_col[1].z, rotationMatrix.m_col[1].y);
        rotation.z = 0;
    }
}

inline float4x4 operator|(const float4x4& m, float4x4(*func)(const float4x4&)) {
	return func(m);
}

struct translate_op {
	float3 translation;
	inline translate_op(float x, float y, float z) : translation(x, y, z) {}
	inline translate_op(float3 t) : translation(t) {}
};

inline translate_op translate(float x, float y, float z) {
	return translate_op(x, y, z);
}

inline translate_op translate(float3 t) {
	return translate_op(t);
}

inline float4x4 operator|(float4x4 m, const translate_op& op) {
	m.col(3) += float4{op.translation.x, op.translation.y, op.translation.z, 0.0f};
	return m;
}

inline float4x4& operator|=(float4x4& m, const translate_op& op) {
	m.col(3) += float4{op.translation.x, op.translation.y, op.translation.z, 0.0f};
	return m;
}

struct rotateX_op {
	float rotation;
	inline rotateX_op(float x) : rotation(x) {}
};

inline rotateX_op rotateX(float x) {
	return rotateX_op(x);
}

inline float4x4 operator|(float4x4 m, const rotateX_op& op) {
	return rotate4x4X(op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, const rotateX_op& op) {
	m = rotate4x4X(op.rotation) * m;
	return m;
}

struct rotateY_op {
	float rotation;
	inline rotateY_op(float x) : rotation(x) {}
};

inline rotateY_op rotateY(float x) {
	return rotateY_op(x);
}

inline float4x4 operator|(float4x4 m, const rotateY_op& op) {
	return rotate4x4Y(op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, const rotateY_op& op) {
	m = rotate4x4Y(op.rotation) * m;
	return m;
}

struct rotateZ_op {
	float rotation;
	inline rotateZ_op(float x) : rotation(x) {}
};

inline rotateZ_op rotateZ(float x) {
	return rotateZ_op(x);
}

inline float4x4 operator|(float4x4 m, const rotateZ_op& op) {
	return rotate4x4Z(op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, const rotateZ_op& op) {
	m = rotate4x4Z(op.rotation) * m;
	return m;
}

struct rotate_op {
	float3 axis;
	float rotation;
	inline rotate_op(float x, float y, float z, float r) : axis(x, y, z), rotation(r) {}
	inline rotate_op(float3 a, float r) : axis(a), rotation(r) {}
};

inline rotate_op rotate(float x, float y, float z, float r) {
	return rotate_op(x, y, z, r);
}

inline rotate_op rotate(float3 a, float r) {
	return rotate_op(a, r);
}

inline float4x4 operator|(float4x4 m, const rotate_op& op) {
	return rotate4x4(op.axis, op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, const rotate_op& op) {
	m = rotate4x4(op.axis, op.rotation) * m;
	return m;
}

struct scale_op {
	float3 scale;
	inline scale_op(float x, float y, float z) : scale(x, y, z) {}
	inline scale_op(float3 s) : scale(s) {}
};

inline scale_op scale(float x, float y, float z) {
	return scale_op(x, y, z);
}

inline scale_op scale(float3 s) {
	return scale_op(s);
}

inline float4x4 operator|(float4x4 m, const scale_op& op) {
	return scale4x4(op.scale) * m;
}

inline float4x4& operator|=(float4x4& m, const scale_op& op) {
	m = scale4x4(op.scale) * m;
	return m;
}

#if __cplusplus >= 201703L
inline float4x4 operator|(const float4x4& matrix, std::tuple<float3&, float3&, float3&> params) {
	auto& [translation, rotation, scale] = params;
	decompose(matrix, translation, rotation, scale);
	return matrix;
}
#else
inline float4x4 operator|(const float4x4& matrix, std::tuple<float3&, float3&, float3&> params) {
	float3& translation = std::get<0>(params);
	float3& rotation = std::get<1>(params);
	float3& scale = std::get<2>(params);
	decompose(matrix, translation, rotation, scale);
	return matrix;
}
#endif

}