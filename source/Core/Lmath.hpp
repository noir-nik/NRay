#pragma once

#include <cmath>

#if defined(_MSC_VER)
#define CVEX_ALIGNED(x) __declspec(align(x))
#else
//#if defined(__GNUC__)
#define CVEX_ALIGNED(x) __attribute__ ((aligned(x)))
//#endif
#endif

namespace Lmath
{
typedef unsigned int uint;
typedef unsigned char	uchar;

constexpr float DEG_TO_RAD   = float(3.14159265358979323846f) / 180.0f;

typedef struct int4
{
	inline int4() : x(0), y(0), z(0), w(0) {}
	inline int4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
	inline explicit int4(int val) : x(val), y(val), z(val), w(val) {}
	inline explicit int4(const int a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
	
	inline int& operator[](int i)			 { return M[i]; }
	inline int	operator[](int i) const { return M[i]; }
	union
	{
		struct { int x, y, z, w; };
		struct { int r, g, b, a; };
		int M[4];
	};
} ivec4;

typedef struct int3
{
	inline int3() : x(0), y(0), z(0) {}
	inline int3(int x, int y, int z) : x(x), y(y), z(z) {}
	inline explicit int3(int val) : x(val), y(val), z(val) {}
	inline explicit int3(const int a[3]) : x(a[0]), y(a[1]), z(a[2]) {}
	
	inline int& operator[](int i)			 { return M[i]; }
	inline int	operator[](int i) const { return M[i]; }

	union
	{
	struct { int x, y, z; };
	struct { int r, g, b; };
	#ifdef LAYOUT_STD140
	int M[4];
	#else
	int M[3];
	#endif
	};
} ivec3;

typedef struct int2
{
	inline int2() : x(0), y(0) {}
	inline int2(int x, int y) : x(x), y(y) {}
	inline explicit int2(int val) : x(val), y(val) {}
	inline explicit int2(const int a[2]) : x(a[0]), y(a[1]) {}
	
	inline int& operator[](int i)			 { return M[i]; }
	inline int	operator[](int i) const { return M[i]; }

	union
	{
	struct { int x, y; };
	struct { int r, g; };
	int M[2];
	};
} ivec2;

typedef struct uint4
{
	inline uint4() : x(0), y(0), z(0), w(0) {}
	inline uint4(uint x, uint y, uint z, uint w) : x(x), y(y), z(z), w(w) {}
	inline explicit uint4(uint val) : x(val), y(val), z(val), w(val) {}
	inline explicit uint4(const uint a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
	
	inline uint& operator[](int i)			 { return M[i]; }
	inline uint	operator[](int i) const { return M[i]; }
	union
	{
		struct { uint x, y, z, w; };
		struct { uint r, g, b, a; };
		uint M[4];
	};
} uvec4;

typedef struct uint3
{
	inline uint3() : x(0), y(0), z(0) {}
	inline uint3(uint x, uint y, uint z) : x(x), y(y), z(z) {}
	inline explicit uint3(uint val) : x(val), y(val), z(val) {}
	inline explicit uint3(const uint a[3]) : x(a[0]), y(a[1]), z(a[2]) {}
	
	inline uint& operator[](int i)			 { return M[i]; }
	inline uint	operator[](int i) const { return M[i]; }

	union
	{
	struct { uint x, y, z; };
	struct { uint r, g, b; };
	#ifdef LAYOUT_STD140
	uint M[4];
	#else
	uint M[3];
	#endif
	};
} uvec3;

typedef struct uint2
{
	inline uint2() : x(0), y(0) {}
	inline uint2(uint x, uint y) : x(x), y(y) {}
	inline explicit uint2(uint val) : x(val), y(val) {}
	inline explicit uint2(const uint a[2]) : x(a[0]), y(a[1]) {}
	
	inline uint& operator[](int i)			 { return M[i]; }
	inline uint	operator[](int i) const { return M[i]; }

	union
	{
	struct { uint x, y; };
	struct { uint r, g; };
	uint M[2];
	};
} uvec2;

typedef struct float4
{
	inline float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	inline float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline explicit float4(float val) : x(val), y(val), z(val), w(val) {}
	inline explicit float4(const float a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

	inline float& operator[](int i)			 { return M[i]; }
	inline float	operator[](int i) const { return M[i]; }

	union
	{
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	#ifdef LAYOUT_STD140
	float M[4];
	#else
	float M[3];
	#endif
	};
} vec4;

static inline float4 operator*(const float4 a, const float4 b) { return float4{a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }

static inline void store  (float* p, const float4 a_val) { memcpy((void*)p, (void*)&a_val, sizeof(float)*4); }
static inline void store_u(float* p, const float4 a_val) { memcpy((void*)p, (void*)&a_val, sizeof(float)*4); }  


typedef struct float3
{
	inline float3() : x(0.0f), y(0.0f), z(0.0f) {}
	inline float3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline explicit float3(float val) : x(val), y(val), z(val) {}
	inline explicit float3(const float a[3]) : x(a[0]), y(a[1]), z(a[2]) {}

	inline float& operator[](int i)			 { return M[i]; }
	inline float	operator[](int i) const { return M[i]; }

	union
	{
	struct { float x, y, z; };
	struct { float r, g, b; };
	#ifdef LAYOUT_STD140
	float M[4];
	#else
	float M[3];
	#endif
	};

} vec3;
static inline float3 operator-(const float3 a, const float3 b) { return float3{a.x - b.x, a.y - b.y, a.z - b.z}; }
static inline float3 operator*(const float3 a, const float3 b) { return float3{a.x * b.x, a.y * b.y, a.z * b.z}; }

static inline float3 operator * (const float3 a, float b) { return float3{a.x * b, a.y * b, a.z * b}; }

static inline  float dot(const float3 a, const float3 b)  { return a.x*b.x + a.y*b.y + a.z*b.z; }

static inline  float length(const float3 a) { return std::sqrt(dot(a,a)); }
static inline  float3 normalize(const float3 a) { float lenInv = float(1)/length(a); return a*lenInv; }

static inline float3 shuffle_xzy(float3 a) { return float3{a.x, a.z, a.y}; }
static inline float3 shuffle_yxz(float3 a) { return float3{a.y, a.x, a.z}; }
static inline float3 shuffle_yzx(float3 a) { return float3{a.y, a.z, a.x}; }
static inline float3 shuffle_zxy(float3 a) { return float3{a.z, a.x, a.y}; }
static inline float3 shuffle_zyx(float3 a) { return float3{a.z, a.y, a.x}; }
static inline float3 cross(const float3 a, const float3 b) 
{
	const float3 a_yzx = shuffle_yzx(a);
	const float3 b_yzx = shuffle_yzx(b);
	return shuffle_yzx(a*b_yzx - a_yzx*b);
}

typedef struct float2
{
	inline float2() : x(0.0f), y(0.0f) {}
	inline float2(float x, float y) : x(x), y(y) {}
	inline explicit float2(float val) : x(val), y(val) {}
	inline explicit float2(const float a[2]) : x(a[0]), y(a[1]) {}

	inline float& operator[](int i)			 { return M[i]; }
	inline float	operator[](int i) const { return M[i]; }

	union
	{
	struct { float x, y; };
	struct { float r, g; };
	float M[2];
	};
} vec2;

static inline float2 operator+(const float2 a, const float2 b) { return float2{a.x + b.x, a.y + b.y}; }
static inline float2 operator-(const float2 a, const float2 b) { return float2{a.x - b.x, a.y - b.y}; }

typedef struct uchar4
{
	uchar r, g, b, a;
} uchar4;

static inline void mat4_rowmajor_mul_mat4(float* __restrict M, const float* __restrict A, const float* __restrict B) // modern gcc compiler succesfuly vectorize such implementation!
{
	M[ 0] = A[ 0] * B[ 0] + A[ 1] * B[ 4] + A[ 2] * B[ 8] + A[ 3] * B[12];
	M[ 1] = A[ 0] * B[ 1] + A[ 1] * B[ 5] + A[ 2] * B[ 9] + A[ 3] * B[13];
	M[ 2] = A[ 0] * B[ 2] + A[ 1] * B[ 6] + A[ 2] * B[10] + A[ 3] * B[14];
	M[ 3] = A[ 0] * B[ 3] + A[ 1] * B[ 7] + A[ 2] * B[11] + A[ 3] * B[15];
	M[ 4] = A[ 4] * B[ 0] + A[ 5] * B[ 4] + A[ 6] * B[ 8] + A[ 7] * B[12];
	M[ 5] = A[ 4] * B[ 1] + A[ 5] * B[ 5] + A[ 6] * B[ 9] + A[ 7] * B[13];
	M[ 6] = A[ 4] * B[ 2] + A[ 5] * B[ 6] + A[ 6] * B[10] + A[ 7] * B[14];
	M[ 7] = A[ 4] * B[ 3] + A[ 5] * B[ 7] + A[ 6] * B[11] + A[ 7] * B[15];
	M[ 8] = A[ 8] * B[ 0] + A[ 9] * B[ 4] + A[10] * B[ 8] + A[11] * B[12];
	M[ 9] = A[ 8] * B[ 1] + A[ 9] * B[ 5] + A[10] * B[ 9] + A[11] * B[13];
	M[10] = A[ 8] * B[ 2] + A[ 9] * B[ 6] + A[10] * B[10] + A[11] * B[14];
	M[11] = A[ 8] * B[ 3] + A[ 9] * B[ 7] + A[10] * B[11] + A[11] * B[15];
	M[12] = A[12] * B[ 0] + A[13] * B[ 4] + A[14] * B[ 8] + A[15] * B[12];
	M[13] = A[12] * B[ 1] + A[13] * B[ 5] + A[14] * B[ 9] + A[15] * B[13];
	M[14] = A[12] * B[ 2] + A[13] * B[ 6] + A[14] * B[10] + A[15] * B[14];
	M[15] = A[12] * B[ 3] + A[13] * B[ 7] + A[14] * B[11] + A[15] * B[15];
}

static inline void mat4_colmajor_mul_vec4(float* __restrict RES, const float* __restrict B, const float* __restrict V) // modern gcc compiler succesfuly vectorize such implementation!
{
	RES[0] = V[0] * B[0] + V[1] * B[4] + V[2] * B[ 8] + V[3] * B[12];
	RES[1] = V[0] * B[1] + V[1] * B[5] + V[2] * B[ 9] + V[3] * B[13];
	RES[2] = V[0] * B[2] + V[1] * B[6] + V[2] * B[10] + V[3] * B[14];
	RES[3] = V[0] * B[3] + V[1] * B[7] + V[2] * B[11] + V[3] * B[15];
}

static inline void transpose4(const float4 in_rows[4], float4 out_rows[4])
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


struct float4x4
{
	inline float4x4()  { identity(); }

	inline explicit float4x4(const float A[16])
	{
		m_col[0] = float4{ A[0], A[4], A[8],	A[12] };
		m_col[1] = float4{ A[1], A[5], A[9],	A[13] };
		m_col[2] = float4{ A[2], A[6], A[10], A[14] };
		m_col[3] = float4{ A[3], A[7], A[11], A[15] };
	}

	inline explicit float4x4(float A0, float A1, float A2, float A3,
														float A4, float A5, float A6, float A7,
														float A8, float A9, float A10, float A11,
														float A12, float A13, float A14, float A15)
	{
		m_col[0] = float4{ A0, A4, A8,	A12 };
		m_col[1] = float4{ A1, A5, A9,	A13 };
		m_col[2] = float4{ A2, A6, A10, A14 };
		m_col[3] = float4{ A3, A7, A11, A15 };
	}

	inline void identity()
	{
	m_col[0] = float4{ 1.0f, 0.0f, 0.0f, 0.0f };
	m_col[1] = float4{ 0.0f, 1.0f, 0.0f, 0.0f };
	m_col[2] = float4{ 0.0f, 0.0f, 1.0f, 0.0f };
	m_col[3] = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	inline float4 get_col(int i) const                { return m_col[i]; }
	inline void   set_col(int i, const float4& a_col) { m_col[i] = a_col; }

	inline float4 get_row(int i) const { return float4{ m_col[0][i], m_col[1][i], m_col[2][i], m_col[3][i] }; }
	inline void   set_row(int i, const float4& a_col)
	{
	m_col[0][i] = a_col[0];
	m_col[1][i] = a_col[1];
	m_col[2][i] = a_col[2];
	m_col[3][i] = a_col[3];
	}

	inline float4& col(int i)       { return m_col[i]; }
	inline float4  col(int i) const { return m_col[i]; }

	inline float& operator()(int row, int col)       { return m_col[col][row]; }
	inline float  operator()(int row, int col) const { return m_col[col][row]; }

	struct RowTmp 
	{
	float4x4* self;
	int       row;
	inline float& operator[](int col)       { return self->m_col[col][row]; }
	inline float  operator[](int col) const { return self->m_col[col][row]; }
	};

	inline RowTmp operator[](int a_row) 
	{
	RowTmp row;
	row.self = this;
	row.row  = a_row;
	return row;
	}

	float4 m_col[4];
};

static inline float4 mul(const float4x4& m, const float4& v)
{
	float4 res;
	mat4_colmajor_mul_vec4((float*)&res, (const float*)&m, (const float*)&v);
	return res;
}


static inline float4x4 translate4x4(float3 t)
{
	float4x4 res;
	res.set_col(3, float4{t.x,  t.y,  t.z, 1.0f });
	return res;
}

static inline float4x4 scale4x4(float3 t)
{
	float4x4 res;
	res.set_col(0, float4{t.x, 0.0f, 0.0f,  0.0f});
	res.set_col(1, float4{0.0f, t.y, 0.0f,  0.0f});
	res.set_col(2, float4{0.0f, 0.0f,  t.z, 0.0f});
	res.set_col(3, float4{0.0f, 0.0f, 0.0f, 1.0f});
	return res;
}

static inline float4x4 rotate4x4X(float phi)
{
	float4x4 res;
	res.set_col(0, float4{1.0f,      0.0f,       0.0f, 0.0f  });
	res.set_col(1, float4{0.0f, +cosf(phi),  +sinf(phi), 0.0f});
	res.set_col(2, float4{0.0f, -sinf(phi),  +cosf(phi), 0.0f});
	res.set_col(3, float4{0.0f,      0.0f,       0.0f, 1.0f  });
	return res;
}

static inline float4x4 rotate4x4Y(float phi)
{
	float4x4 res;
	res.set_col(0, float4{+cosf(phi), 0.0f, -sinf(phi), 0.0f});
	res.set_col(1, float4{     0.0f, 1.0f,      0.0f, 0.0f  });
	res.set_col(2, float4{+sinf(phi), 0.0f, +cosf(phi), 0.0f});
	res.set_col(3, float4{     0.0f, 0.0f,      0.0f, 1.0f  });
	return res;
}

static inline float4x4 rotate4x4Z(float phi)
{
	float4x4 res;
	res.set_col(0, float4{+cosf(phi), sinf(phi), 0.0f, 0.0f});
	res.set_col(1, float4{-sinf(phi), cosf(phi), 0.0f, 0.0f});
	res.set_col(2, float4{     0.0f,     0.0f, 1.0f, 0.0f  });
	res.set_col(3, float4{     0.0f,     0.0f, 0.0f, 1.0f  });
	return res;
}

static inline float4x4 mul(float4x4 m1, float4x4 m2)
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

static inline float4x4 operator*(float4x4 m1, float4x4 m2)
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

static inline float4x4 inverse4x4(float4x4 m1)
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


/* 
	Utility functions
*/

// Look At matrix creation
// return the inverse view matrix
//
static inline float4x4 lookAt(float3 eye, float3 center, float3 up)
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

static inline float4x4 perspectiveMatrix(float fovy, float aspect, float zNear, float zFar)
{
	const float ymax = zNear * tanf(fovy * 3.14159265358979323846f / 360.0f);
	const float xmax = ymax * aspect;
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

}