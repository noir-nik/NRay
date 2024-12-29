#ifndef LMATH_HPP_
#define LMATH_HPP_

#include <cmath>
#include <cstring>
#include <tuple>
#include "lmath_types.hpp"


#ifndef LMATH_EXPORT
#define LMATH_EXPORT
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
namespace lmath {

inline int   min(int const   a, int const   b) { return (a < b) ? a : b; }
inline int   max(int const   a, int const   b) { return (a > b) ? a : b; }
inline uint  min(uint const  a, uint const  b) { return (a < b) ? a : b; }
inline uint  max(uint const  a, uint const  b) { return (a > b) ? a : b; }
inline float min(float const a, float const b) { return (a < b) ? a : b; }
inline float max(float const a, float const b) { return (a > b) ? a : b; }

inline int   clamp(int u,   int a,   int b)   { return min(max(a, u), b); }
inline uint  clamp(uint u,  uint a,  uint b)  { return min(max(a, u), b); }
inline float clamp(float u, float a, float b) { return min(max(a, u), b); }

constexpr inline float DEG_TO_RAD = 3.14159265358979323846f / 180.0f;

inline int4 operator+(int4 const a, int4 const b) { return int4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
inline int4 operator-(int4 const a, int4 const b) { return int4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
inline int4 operator*(int4 const a, int4 const b) { return int4{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
inline int4 operator/(int4 const a, int4 const b) { return int4{a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }

inline int4 operator * (int4 const a, int b) { return int4{a.x * b, a.y * b, a.z * b, a.w * b}; }
inline int4 operator / (int4 const a, int b) { return int4{a.x / b, a.y / b, a.z / b, a.w / b}; }
inline int4 operator * (int a, int4 const b) { return int4{a * b.x, a * b.y, a * b.z, a * b.w}; }
inline int4 operator / (int a, int4 const b) { return int4{a / b.x, a / b.y, a / b.z, a / b.w}; }

inline int4 operator + (int4 const a, int b) { return int4{a.x + b, a.y + b, a.z + b, a.w + b}; }
inline int4 operator - (int4 const a, int b) { return int4{a.x - b, a.y - b, a.z - b, a.w - b}; }
inline int4 operator + (int a, int4 const b) { return int4{a + b.x, a + b.y, a + b.z, a + b.w}; }
inline int4 operator - (int a, int4 const b) { return int4{a - b.x, a - b.y, a - b.z, a - b.w}; }

inline int4& operator *= (int4& a, int4 const b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;  return a; }
inline int4& operator /= (int4& a, int4 const b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w;  return a; }
inline int4& operator *= (int4& a, int b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b;  return a; }
inline int4& operator /= (int4& a, int b) { a.x /= b; a.y /= b; a.z /= b; a.w /= b;  return a; }

inline int4& operator += (int4& a, int4 const b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;  return a; }
inline int4& operator -= (int4& a, int4 const b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;  return a; }
inline int4& operator += (int4& a, int b) { a.x += b; a.y += b; a.z += b; a.w += b;  return a; }
inline int4& operator -= (int4& a, int b) { a.x -= b; a.y -= b; a.z -= b; a.w -= b;  return a; }

inline void store (int* p, int4 const a_val)  { memcpy((void*)p, (void*)&a_val, sizeof(int)*4); }
inline void load  (int4& dst, const int* src) { memcpy((void*)&dst, (void*)src, sizeof(int)*4); }


inline float2 operator+(float2 const a, float2 const b) { return float2{a.x + b.x, a.y + b.y}; }
inline float2 operator-(float2 const a, float2 const b) { return float2{a.x - b.x, a.y - b.y}; }


inline float3 operator+(float3 const a, float3 const b) { return float3{a.x + b.x, a.y + b.y, a.z + b.z}; }
inline float3 operator-(float3 const a, float3 const b) { return float3{a.x - b.x, a.y - b.y, a.z - b.z}; }
inline float3 operator*(float3 const a, float3 const b) { return float3{a.x * b.x, a.y * b.y, a.z * b.z}; }
inline float3 operator/(float3 const a, float3 const b) { return float3{a.x / b.x, a.y / b.y, a.z / b.z}; }

inline float3 operator * (float3 const a, float b) { return float3{a.x * b, a.y * b, a.z * b}; }
inline float3 operator / (float3 const a, float b) { return float3{a.x / b, a.y / b, a.z / b}; }
inline float3 operator * (float a, float3 const b) { return float3{a * b.x, a * b.y, a * b.z}; }
inline float3 operator / (float a, float3 const b) { return float3{a / b.x, a / b.y, a / b.z}; }

inline float3 operator + (float3 const a, float b) { return float3{a.x + b, a.y + b, a.z + b}; }
inline float3 operator - (float3 const a, float b) { return float3{a.x - b, a.y - b, a.z - b}; }
inline float3 operator + (float a, float3 const b) { return float3{a + b.x, a + b.y, a + b.z}; }
inline float3 operator - (float a, float3 const b) { return float3{a - b.x, a - b.y, a - b.z}; }

inline float3& operator *= (float3& a, float3 const b) { a.x *= b.x; a.y *= b.y; a.z *= b.z;  return a; }
inline float3& operator /= (float3& a, float3 const b) { a.x /= b.x; a.y /= b.y; a.z /= b.z;  return a; }
inline float3& operator *= (float3& a, float b) { a.x *= b; a.y *= b; a.z *= b;  return a; }
inline float3& operator /= (float3& a, float b) { a.x /= b; a.y /= b; a.z /= b;  return a; }

inline float3& operator += (float3& a, float3 const b) { a.x += b.x; a.y += b.y; a.z += b.z;  return a; }
inline float3& operator -= (float3& a, float3 const b) { a.x -= b.x; a.y -= b.y; a.z -= b.z;  return a; }
inline float3& operator += (float3& a, float b) { a.x += b; a.y += b; a.z += b;  return a; }
inline float3& operator -= (float3& a, float b) { a.x -= b; a.y -= b; a.z -= b;  return a; }

inline float3 operator -(float3 const a) { return float3{-a.x, -a.y, -a.z}; }

inline void store  (float* p, float3 const a_val) { memcpy((void*)p, (void*)&a_val, sizeof(float)*3); }
inline void store_u(float* p, float3 const a_val) { memcpy((void*)p, (void*)&a_val, sizeof(float)*3); }  

inline void load (float3& dst, const float* src) { memcpy((void*)&dst, (void*)src, sizeof(float)*3); }
inline float3 make_float3(const float* p) { float3 r; memcpy((void*)&r, (void*)p, sizeof(r)); return r; }

inline float3 min  (float3 const a, float3 const b) { return float3{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)}; }
inline float3 max  (float3 const a, float3 const b) { return float3{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)}; }
inline float3 clamp(float3 const u, float3 const a, float3 const b) { return float3{clamp(u.x, a.x, b.x), clamp(u.y, a.y, b.y), clamp(u.z, a.z, b.z)}; }
inline float3 clamp(float3 const u, float a, float b) { return float3{clamp(u.x, a, b), clamp(u.y, a, b), clamp(u.z, a, b)}; }

inline float  dot(float3 const a, float3 const b)  { return a.x*b.x + a.y*b.y + a.z*b.z; }

inline float  length(float3 const a) { return std::sqrt(dot(a,a)); }
inline float3 normalize(float3 const a) { float lenInv = float(1)/length(a); return a*lenInv; }

inline float3 shuffle_xzy(float3 a) { return float3{a.x, a.z, a.y}; }
inline float3 shuffle_yxz(float3 a) { return float3{a.y, a.x, a.z}; }
inline float3 shuffle_yzx(float3 a) { return float3{a.y, a.z, a.x}; }
inline float3 shuffle_zxy(float3 a) { return float3{a.z, a.x, a.y}; }
inline float3 shuffle_zyx(float3 a) { return float3{a.z, a.y, a.x}; }
inline float3 cross(float3 const a, float3 const b) 
{
	float3 const a_yzx = shuffle_yzx(a);
	float3 const b_yzx = shuffle_yzx(b);
	return shuffle_yzx(a*b_yzx - a_yzx*b);
}


inline bool operator==(int4 const& a, int4 const& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
inline bool operator!=(int4 const& a, int4 const& b) { return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w; }

inline float4 operator+(float4 const a, float4 const b) { return float4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
inline float4 operator-(float4 const a, float4 const b) { return float4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }
inline float4 operator*(float4 const a, float4 const b) { return float4{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
inline float4 operator/(float4 const a, float4 const b) { return float4{a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }

inline float4 operator * (float4 const a, float b) { return float4{a.x * b, a.y * b, a.z * b, a.w * b}; }
inline float4 operator / (float4 const a, float b) { return float4{a.x / b, a.y / b, a.z / b, a.w / b}; }
inline float4 operator * (float a, float4 const b) { return float4{a * b.x, a * b.y, a * b.z, a * b.w}; }
inline float4 operator / (float a, float4 const b) { return float4{a / b.x, a / b.y, a / b.z, a / b.w}; }

inline float4 operator + (float4 const a, float b) { return float4{a.x + b, a.y + b, a.z + b, a.w + b}; }
inline float4 operator - (float4 const a, float b) { return float4{a.x - b, a.y - b, a.z - b, a.w - b}; }
inline float4 operator + (float a, float4 const b) { return float4{a + b.x, a + b.y, a + b.z, a + b.w}; }
inline float4 operator - (float a, float4 const b) { return float4{a - b.x, a - b.y, a - b.z, a - b.w}; }

inline float4& operator *= (float4& a, float4 const b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w;  return a; }
inline float4& operator /= (float4& a, float4 const b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w;  return a; }
inline float4& operator *= (float4& a, float b) { a.x *= b; a.y *= b; a.z *= b; a.w *= b;  return a; }
inline float4& operator /= (float4& a, float b) { a.x /= b; a.y /= b; a.z /= b; a.w /= b;  return a; }

inline float4& operator += (float4& a, float4 const b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w;  return a; }
inline float4& operator -= (float4& a, float4 const b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w;  return a; }
inline float4& operator += (float4& a, float b) { a.x += b; a.y += b; a.z += b; a.w += b;  return a; }
inline float4& operator -= (float4& a, float b) { a.x -= b; a.y -= b; a.z -= b; a.w -= b;  return a; }

inline bool operator==(float4 const a, float4 const b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }

inline void store  (float* dst, float4 const src) { memcpy((void*)dst, (void*)&src, sizeof(float)*4); }
inline void store_u(float* dst, float4 const src) { memcpy((void*)dst, (void*)&src, sizeof(float)*4); }

inline void load(float4& dst, const float* src) { memcpy((void*)&dst, (void*)src, sizeof(float)*4); }
inline float4 make_float4(const float* p) { float4 r; memcpy((void*)&r, (void*)p, sizeof(r)); return r; }

inline float4 clamp(float4 const u, float4 const a, float4 const b) { return float4{clamp(u.x, a.x, b.x), clamp(u.y, a.y, b.y), clamp(u.z, a.z, b.z), clamp(u.w, a.w, b.w)}; }
inline float4 min  (float4 const a, float4 const b)   { return float4{min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w)}; }
inline float4 max  (float4 const a, float4 const b)   { return float4{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w)}; }
inline float4 clamp(float4 const u, float a, float b) { return float4{clamp(u.x, a, b), clamp(u.y, a, b), clamp(u.z, a, b), clamp(u.w, a, b)}; }
inline float4 round(float4 const a)                   { return float4{std::round(a.x), std::round(a.y), std::round(a.z), std::round(a.w)}; }
inline float4 abs (float4 const a) { return float4{std::abs(a.x), std::abs(a.y), std::abs(a.z), std::abs(a.w)}; } 
// inline float4 sign(float4 const a) { return float4{sign(a.x), sign(a.y), sign(a.z), sign(a.w)}; }

inline float4 floor(float4 const a) { return float4{std::floor(a.x), std::floor(a.y), std::floor(a.z), std::floor(a.w)}; }
inline float4 ceil (float4 const a) { return float4{std::ceil(a.x), std::ceil(a.y), std::ceil(a.z), std::ceil(a.w)}; }
inline float4 rcp  (float4 const a) { return float4{1.0f/a.x, 1.0f/a.y, 1.0f/a.z, 1.0f/a.w}; }
inline float4 sqrt (float4 const a) { return float4{std::sqrt(a.x), std::sqrt(a.y), std::sqrt(a.z), std::sqrt(a.w)}; }
inline float4 fract(float4 const x) { return x - floor(x); }
inline float4 mod  (float4 const x, float4 const y) { return x - y * floor(x/y); }
inline float4 lerp (float4 const a, float4 const b, float t) { return a + t * (b - a); }
inline float4 mix  (float4 const a, float4 const b, float t) { return a + t * (b - a); }
inline float4 inversesqrt(float4 const a)           { return 1.0f/sqrt(a); }

inline float dot(float4 const a, float4 const b)  { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
inline float length(float4 const a) { return std::sqrt(dot(a,a)); }


inline uint packRGBA(float4 const& v) { 
	uvec4 res = uvec4(round(clamp(v, 0.0f, 1.0f) * 255.0f));
	return res.x | (res.y << 8) | (res.z << 16) | (res.w << 24);
}

inline float4 unpackRGBA(uint rgba) { 
	uvec4 res = uvec4(rgba);
	return float4(res.x/255.0f, res.y/255.0f, res.z/255.0f, res.w/255.0f);
}

inline quat make_quat(const float* p) { quat r; memcpy((void*)&r, (void*)p, sizeof(r)); return r; }

template<typename T, typename Ret>
inline Ret operator|(T const& vec, Ret (*func)(T const&)) { return func(vec); }

inline void mat4_colmajor_mul_vec4(float* __restrict RES, const float* __restrict B, const float* __restrict V)
{
	RES[0] = V[0] * B[0] + V[1] * B[4] + V[2] * B[ 8] + V[3] * B[12];
	RES[1] = V[0] * B[1] + V[1] * B[5] + V[2] * B[ 9] + V[3] * B[13];
	RES[2] = V[0] * B[2] + V[1] * B[6] + V[2] * B[10] + V[3] * B[14];
	RES[3] = V[0] * B[3] + V[1] * B[7] + V[2] * B[11] + V[3] * B[15];
}

inline void transpose4(float4 const in_rows[4], float4 out_rows[4]) // SIMD 
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

inline float3x3 inverse(float3x3 const& m);
inline float3x3 transpose(float3x3 const& m1);

inline float4x4 inverse(float4x4 const& m1);
inline float4x4 transpose(float4x4 const& m1);
inline float4x4 affineInverse(float4x4 const& m);
inline void decompose(float4x4 m, float3& translation, float3& rotation, float3& scale);

inline float3x3 transpose(float3x3 const& m1)
{
	float3x3 res;
	res.set_row(0, float3{m1(0, 0), m1(1, 0), m1(2, 0)});
	res.set_row(1, float3{m1(0, 1), m1(1, 1), m1(2, 1)});
	res.set_row(2, float3{m1(0, 2), m1(1, 2), m1(2, 2)});
	return res;
}

inline float3x3 inverse(float3x3 const& m) 
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

inline float3 mul(float3x3 const& m, float3 const& v)
{
	float3 res;
	mat3_colmajor_mul_vec3((float*)&res, (const float*)&m, (const float*)&v);
	return res;
}

inline float3 operator*(float3x3 const& m, float3 const& v) { return mul(m, v); }

inline float4 mul(float4x4 const& m, float4 const& v)
{
	float4 res;
	mat4_colmajor_mul_vec4((float*)&res, (const float*)&m, (const float*)&v);
	return res;
}

inline float3x3 rotate3x3(quat const& q)
{
	auto q1 = q / length(q);
	float x2 = q1.x * q1.x;
	float y2 = q1.y * q1.y;
	float z2 = q1.z * q1.z;
	float xy = q1.x * q1.y;
	float xz = q1.x * q1.z;
	float xw = q1.x * q1.w;
	float yz = q1.y * q1.z;
	float yw = q1.y * q1.w;
	float zw = q1.z * q1.w;

	return float3x3(
		1.0f - 2.0f * (y2 + z2), 2.0f * (xy + zw),        2.0f * (xz - yw),
		2.0f * (xy - zw),        1.0f - 2.0f * (x2 + z2), 2.0f * (yz + xw),
		2.0f * (xz + yw),        2.0f * (yz - xw),        1.0f - 2.0f * (x2 + y2)
	);
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
	float4 const column1 = mul(m1, m2.col(0));
	float4 const column2 = mul(m1, m2.col(1));
	float4 const column3 = mul(m1, m2.col(2));
	float4 const column4 = mul(m1, m2.col(3));
	float4x4 res;
	res.set_col(0, column1);
	res.set_col(1, column2);
	res.set_col(2, column3);
	res.set_col(3, column4);

	return res;
}

inline float4x4 operator*(float4x4 m1, float4x4 m2)
{
	float4 const column1 = mul(m1, m2.col(0));
	float4 const column2 = mul(m1, m2.col(1));
	float4 const column3 = mul(m1, m2.col(2));
	float4 const column4 = mul(m1, m2.col(3));

	float4x4 res;
	res.set_col(0, column1);
	res.set_col(1, column2);
	res.set_col(2, column3);
	res.set_col(3, column4);
	return res;
}
inline float4x4 make_float4x4(const float* p) { float4x4 r; memcpy((void*)&r, (void*)p, sizeof(r)); return r; }

/* 
inline float4x4 transpose(float4x4 const& rhs)
{
	float4x4 res;
	transpose4(rhs.m_col, res.m_col);
	return res;
}
 */
inline float4x4 transpose(float4x4 const& m1)
{
	float4x4 res;
	res.set_row(0, float4{m1(0, 0), m1(1, 0), m1(2, 0), m1(3, 0)});
	res.set_row(1, float4{m1(0, 1), m1(1, 1), m1(2, 1), m1(3, 1)});
	res.set_row(2, float4{m1(0, 2), m1(1, 2), m1(2, 2), m1(3, 2)});
	res.set_row(3, float4{m1(0, 3), m1(1, 3), m1(2, 3), m1(3, 3)});
	return res;
}

inline float4x4 inverse(float4x4 const& m1)
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
	float const k = 1.0f / (m1(0,0) * m(0,0) + m1(0,1) * m(1,0) + m1(0,2) * m(2,0) + m1(0,3) * m(3,0));
	float4 const vK{k,k,k,k};

	m.set_col(0, m.get_col(0)*vK);
	m.set_col(1, m.get_col(1)*vK);
	m.set_col(2, m.get_col(2)*vK);
	m.set_col(3, m.get_col(3)*vK);

	return m;
}

inline float4x4 affineInverse(float4x4 const& m) {
	auto inv = inverse(float3x3(m));
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
// WIP
inline float4x4 lookAt1(float3 eye, float3 center, float3 up)
{
	float3 const f = normalize(center - eye);
	float3 const s = normalize(cross(f, up));
	float3 const u = cross(s, f);
	float4x4 res;
	res.m_col[0] = float4{ s.x, u.x, -f.x, 0.0f };
	res.m_col[1] = float4{ s.y, u.y, -f.y, 0.0f };
	res.m_col[2] = float4{ s.z, u.z, -f.z, 0.0f };
	res.m_col[3] = float4{ -dot(s, eye), -dot(u, eye), dot(f, eye), 1.0f };
	return res;
}

inline float4x4 lookAtInv(float3 eye, float3 center, float3 up)
{
	float3 const f = normalize(center - eye);
	float3 const s = normalize(cross(f, up));
	float3 const u = cross(s, f);
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

	float const left = -xmax;
	float const right = +xmax;
	float const bottom = -ymax;
	float const top = +ymax;
	float const temp = 2.0f * zNear;
	float const temp2 = right - left;
	float const temp3 = top - bottom;
	float const temp4 = zFar - zNear;
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

inline void decompose(float4x4 m, float3& translation, float3& rotationEuler, float3& scale)
{
    // Extract translation
    translation = m.col(3).xyz();

    // Extract scale
    scale.x = length(m.col(0).xyz());
    scale.y = length(m.col(1).xyz());
    scale.z = length(m.col(2).xyz());

    // Remove scale from the matrix to extract rotation
    m.col(0).xyz() /= scale.x;
    m.col(1).xyz() /= scale.y;
    m.col(2).xyz() /= scale.z;

    // Extract rotation angles from the rotation matrix
    rotationEuler.y = std::asin(-m.col(2).x);
    if (std::cos(rotationEuler.y) != 0) {
        rotationEuler.x = std::atan2(m.col(2).y, m.col(2).z);
        rotationEuler.z = std::atan2(m.col(1).x, m.col(0).x);
    } else {
        rotationEuler.x = std::atan2(-m.col(1).z, m.col(1).y);
        rotationEuler.z = 0;
    }
}

inline void decompose(float4x4 m, float3& translation, quat& rotation, float3& scale)
{
	// Extract translation
	translation = m.get_col(3).xyz();
	m.col(3).xyz() = float3(0.0f, 0.0f, 0.0f);

    // Extract scale
    scale.x = length(m.col(0).xyz());
    scale.y = length(m.col(1).xyz());
    scale.z = length(m.col(2).xyz());

    // Remove scale from the matrix to extract rotation
    m.col(0).xyz() /= scale.x;
    m.col(1).xyz() /= scale.y;
    m.col(2).xyz() /= scale.z;

    // Extract rotation quaternion
    rotation = quat(
        max(0.0f, 1.0f + m(0, 0) - m(1, 1) - m(2, 2)),
        max(0.0f, 1.0f - m(0, 0) + m(1, 1) - m(2, 2)),
        max(0.0f, 1.0f - m(0, 0) - m(1, 1) + m(2, 2)),
        max(0.0f, 1.0f + m(0, 0) + m(1, 1) + m(2, 2))
    );

    rotation.x = sqrtf(rotation.x) / 2.0f;
    rotation.y = sqrtf(rotation.y) / 2.0f;
    rotation.z = sqrtf(rotation.z) / 2.0f;
    rotation.w = sqrtf(rotation.w) / 2.0f;

    rotation.x = std::copysignf(rotation.x, m(1, 2) - m(2, 1));
    rotation.y = std::copysignf(rotation.y, m(2, 0) - m(0, 2));
    rotation.z = std::copysignf(rotation.z, m(0, 1) - m(1, 0));
}

[[nodiscard]] inline auto decomposeEuler(float4x4 const& m) -> std::tuple<float3, float3, float3> {
	float3 translation, rotation, scale;
	decompose(m, translation, rotation, scale);
	return std::tuple{translation, rotation, scale};
}

[[nodiscard]] inline auto decompose(float4x4 const& m) -> std::tuple<float3, quat, float3> {
	float3 translation, scale;
	quat rotation;
	decompose(m, translation, rotation, scale);
	return std::tuple{translation, rotation, scale};
}

inline float4x4 operator|(float4x4 const& m, float4x4(*func)(const float4x4&)) {
	return func(m);
}

struct translate_op {
	float3 translation;
	inline translate_op(float3 t) : translation(t) {}
};


inline translate_op translate(float3 t) {
	return translate_op(t);
}

inline float4x4 operator|(float4x4 m, translate_op const& op) {
	m.col(3) += float4{op.translation.x, op.translation.y, op.translation.z, 0.0f};
	return m;
}

inline float4x4& operator|=(float4x4& m, translate_op const& op) {
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

inline float4x4 operator|(float4x4 m, rotateX_op const& op) {
	return rotate4x4X(op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, rotateX_op const& op) {
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

inline float4x4 operator|(float4x4 m, rotateY_op const& op) {
	return rotate4x4Y(op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, rotateY_op const& op) {
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

inline float4x4 operator|(float4x4 m, rotateZ_op const& op) {
	return rotate4x4Z(op.rotation) * m;
}

inline float4x4& operator|=(float4x4& m, rotateZ_op const& op) {
	m = rotate4x4Z(op.rotation) * m;
	return m;
}

struct rotate_euler_op {
	float3 axis;
	float rotation;
	inline rotate_euler_op(float3 a, float r) : axis(a), rotation(r) {}
};

inline rotate_euler_op rotate(float3 a, float r) {
	return rotate_euler_op(a, r);
}

inline float4x4 operator|(float4x4 const& m, rotate_euler_op const& op) {
	return rotate4x4(op.axis, op.rotation) * m;
}

struct rotate_quat_op {
	quat q;
	inline rotate_quat_op(quat q) : q(q) {}
};

inline rotate_quat_op rotate(quat q) {
	return rotate_quat_op(q);
}

inline float4x4 operator|(float4x4 const& m, rotate_quat_op const& op) {
	return m * float4x4(rotate3x3(op.q));
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

inline float4x4 operator|(float4x4 m, scale_op const& op) {
	return scale4x4(op.scale) * m;
}

inline float4x4& operator|=(float4x4& m, scale_op const& op) {
	m = scale4x4(op.scale) * m;
	return m;
}

#if __cplusplus >= 201703L
inline float4x4 operator|(float4x4 const& matrix, std::tuple<float3&, float3&, float3&> params) {
	auto& [translation, rotationEuler, scale] = params;
	decompose(matrix, translation, rotationEuler, scale);
	return matrix;
}

inline float4x4 operator|(float4x4 const& matrix, std::tuple<float3&, quat&, float3&> params) {
	auto& [translation, rotation, scale] = params;
	decompose(matrix, translation, rotation, scale);
	return matrix;
}

#else
inline float4x4 operator|(float4x4 const& matrix, std::tuple<float3&, float3&, float3&> params) {
	float3& translation = std::get<0>(params);
	float3& rotationEuler = std::get<1>(params);
	float3& scale = std::get<2>(params);
	decompose(matrix, translation, rotationEuler, scale);
	return matrix;
}

inline float4x4 operator|(float4x4 const& matrix, std::tuple<float3&, quat&, float3&> params) {
	float3& translation = std::get<0>(params);
	quat& rotation = std::get<1>(params);
	float3& scale = std::get<2>(params);
	decompose(matrix, translation, rotation, scale);
	return matrix;
}
#endif

}

#endif // LMATH_HPP_