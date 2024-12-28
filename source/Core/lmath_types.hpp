#ifndef LMATH_TYPES_HPP
#define LMATH_TYPES_HPP

#ifdef LMATH_CONVERT
#include <type_traits>
#endif
#ifndef LMATH_EXPORT
#define LMATH_EXPORT
#endif

LMATH_EXPORT
namespace lmath
{
typedef unsigned int uint;
typedef unsigned char	uchar;

struct int2;
struct int3;
struct int4;
struct uint2;
struct uint3;
struct uint4;
struct float2;
struct float3;
struct float4;

struct quat;

struct uchar4;

struct float3x3;
struct float4x4;

typedef struct int2
{
	inline int2() : x(0), y(0) {}
	inline int2(int x, int y) : x(x), y(y) {}
	inline explicit int2(int val) : x(val), y(val) {}
	inline explicit int2(int const a[2]) : x(a[0]), y(a[1]) {}

	inline explicit int2(float2 a); 
	inline explicit int2(uint2 a); 
		
	inline int& operator[](int i)			 { return M[i]; }
	inline int	operator[](int i) const { return M[i]; }

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, int, int>
	constexpr inline operator T() const { return T{x, y}; }
#endif

	union
	{
	struct { int x, y; };
	struct { int r, g; };
	int M[2];
	};
} ivec2;

typedef struct int3
{
	inline int3() : x(0), y(0), z(0) {}
	inline int3(int x, int y, int z) : x(x), y(y), z(z) {}
	inline explicit int3(int val) : x(val), y(val), z(val) {}
	inline explicit int3(int const a[3]) : x(a[0]), y(a[1]), z(a[2]) {}

	inline explicit int3(float3 a); 
	inline explicit int3(uint3 a); 
	
	inline int& operator[](int i)			 { return M[i]; }
	inline int	operator[](int i) const { return M[i]; }

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, int, int, int>
	constexpr inline operator T() const { return T{x, y, z}; }
#endif

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

typedef struct int4
{
	inline int4() : x(0), y(0), z(0), w(0) {}
	inline int4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
	inline explicit int4(int val) : x(val), y(val), z(val), w(val) {}
	inline explicit int4(int const a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
	
	inline int& operator[](int i)			 { return M[i]; }
	inline int	operator[](int i) const { return M[i]; }

	inline explicit int4(float4 a); 
	inline explicit int4(uint4 a); 

	inline int2& xy()  { return reinterpret_cast<int2&>(M); }
	inline int2& zw()  { return reinterpret_cast<int2&>(M[2]); }
	inline int3& xyz() { return reinterpret_cast<int3&>(M); }
	inline int2 const& xy()  const { return reinterpret_cast<const int2&>(M); }
	inline int2 const& zw()  const { return reinterpret_cast<const int2&>(M[2]); }
	inline int3 const& xyz() const { return reinterpret_cast<const int3&>(M); }

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, int, int, int, int>
	constexpr inline operator T() const { return T{x, y, z, w}; }
#endif

	union
	{
		struct { int x, y, z, w; };
		struct { int r, g, b, a; };
		int M[4];
	};
} ivec4;


typedef struct uint2
{
	inline uint2() : x(0), y(0) {}
	inline uint2(uint x, uint y) : x(x), y(y) {}
	inline explicit uint2(uint val) : x(val), y(val) {}
	inline explicit uint2(uint const a[2]) : x(a[0]), y(a[1]) {}

	inline explicit uint2(float2 a);
	inline explicit uint2(int2 a);
	
	inline uint& operator[](int i)			 { return M[i]; }
	inline uint	operator[](int i) const { return M[i]; } 
	
#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, uint, uint>
	constexpr inline operator T() const { return T{x, y}; }
#endif

	union
	{
		struct { uint x, y; };
		struct { uint r, g; };
		uint M[2];
	};
} uvec2;

typedef struct uint3
{
	inline uint3() : x(0), y(0), z(0) {}
	inline uint3(uint x, uint y, uint z) : x(x), y(y), z(z) {}
	inline explicit uint3(uint val) : x(val), y(val), z(val) {}
	inline explicit uint3(uint const a[3]) : x(a[0]), y(a[1]), z(a[2]) {}

	inline explicit uint3(float3 a); 
	inline explicit uint3(int3 a); 
	
	inline uint& operator[](int i)			 { return M[i]; }
	inline uint	operator[](int i) const { return M[i]; }

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, uint, uint, uint>
	constexpr inline operator T() const { return T{x, y, z}; }
#endif

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

typedef struct uint4
{
	inline uint4() : x(0), y(0), z(0), w(0) {}
	inline uint4(uint x, uint y, uint z, uint w) : x(x), y(y), z(z), w(w) {}
	inline explicit uint4(uint val) : x(val), y(val), z(val), w(val) {}
	inline explicit uint4(uint const a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
	
	inline explicit uint4(float4 a); 
	inline explicit uint4(int4 a);

	inline uint& operator[](int i)			 { return M[i]; }
	inline uint	operator[](int i) const { return M[i]; }
	// inline explicit operator bool() { return x > 0 || y > 0 || z > 0 || w > 0; }

	inline auto xy()        -> uint2&;
	inline auto zw()        -> uint2&;
	inline auto xyz()       -> uint3&;
	inline auto xy()  const -> uint2 const&;
	inline auto zw()  const -> uint2 const&;
	inline auto xyz() const -> uint3 const&;

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, uint, uint, uint, uint>
	constexpr inline operator T() const { return T{x, y, z, w}; }
#endif

	union
	{
		struct { uint x, y, z, w; };
		struct { uint r, g, b, a; };
		uint M[4];
	};
} uvec4;


typedef struct float2
{
	inline float2() : x(0.0f), y(0.0f) {}
	inline float2(float x, float y) : x(x), y(y) {}
	inline explicit float2(float val) : x(val), y(val) {}
	inline explicit float2(float const a[2]) : x(a[0]), y(a[1]) {}

	inline float& operator[](int i)			 { return M[i]; }
	inline float  operator[](int i) const { return M[i]; }

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, float, float>
	constexpr inline operator T() const { return T{x, y}; }
#endif

	union
	{
		struct { float x, y; };
		struct { float r, g; };
		float M[2];
	};
} vec2;

typedef struct float3
{
	inline float3() : x(0.0f), y(0.0f), z(0.0f) {}
	inline float3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline explicit float3(float val) : x(val), y(val), z(val) {}
	inline explicit float3(float const a[3]) : x(a[0]), y(a[1]), z(a[2]) {}

	inline float& operator[](int i)			 { return M[i]; }
	inline float  operator[](int i) const { return M[i]; }

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, float, float, float>
	constexpr inline operator T() const { return T{x, y, z}; }
#endif

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

typedef struct float4
{
	inline float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	inline float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline explicit float4(float val) : x(val), y(val), z(val), w(val) {}
	inline explicit float4(float const a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

	inline explicit float4(float2 a, float z = 0.0f, float w = 0.0f) : x(a.x), y(a.y), z(z), w(w) {}
	inline explicit float4(float2 a, float2 b) : x(a.x), y(a.y), z(b.x), w(b.y) {}
	inline explicit float4(float3 a, float w = 0.0f) : x(a.x), y(a.y), z(a.z), w(w) {}

	inline explicit float4(uint4 a);
	inline explicit float4(int4 a);

	inline float& operator[](int i)			 { return M[i]; }
	inline float  operator[](int i) const { return M[i]; }

	inline auto xy()        -> float2&;
	inline auto zw()        -> float2&;
	inline auto xyz()       -> float3&;
	inline auto xy()  const -> float2 const&;
	inline auto zw()  const -> float2 const&;
	inline auto xyz() const -> float3 const&;

#ifdef LMATH_CONVERT
	template<typename T>
	requires std::is_constructible_v<T, float, float, float, float>
	constexpr inline operator T() const { return T{x, y, z, w}; }
#endif
	
	union
	{
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	float M[4];
	};
} vec4;

typedef struct quat : float4
{
	using float4::float4;
} quat;

typedef struct uchar4
{
	uchar r, g, b, a;
} uchar4;

typedef struct float3x3
{
	inline float3x3() { identity(); }

	inline void identity()
	{
		m_col[0] = float3{ 1.0f, 0.0f, 0.0f };
		m_col[1] = float3{ 0.0f, 1.0f, 0.0f };
		m_col[2] = float3{ 0.0f, 0.0f, 1.0f };
	}

	inline explicit float3x3(float A00, float A01, float A02,
							float A10, float A11, float A12,
							float A20, float A21, float A22)
	{
		m_col[0] = float3{ A00, A01, A02 };
		m_col[1] = float3{ A10, A11, A12 };
		m_col[2] = float3{ A20, A21, A22 };
	}

	inline float3x3(float4x4 const& from4x4);

	inline float3 get_col(int i) const                { return m_col[i]; }
	inline void   set_col(int i, float3 const& a_col) { m_col[i] = a_col; }

	inline float3 get_row(int i) const { return float3{ m_col[0][i], m_col[1][i], m_col[2][i]}; }
	inline void   set_row(int i, float3 const& a_col) { m_col[0][i] = a_col[0]; m_col[1][i] = a_col[1]; m_col[2][i] = a_col[2]; }

	inline       float3& col(int i)       { return m_col[i]; }
	inline float3 const& col(int i) const { return m_col[i]; }

	inline float& operator()(int row, int col)       { return m_col[col][row]; }
	inline float  operator()(int row, int col) const { return m_col[col][row]; }

#if __cplusplus >= 202302L
	inline auto operator[](int row, int col) -> float& { return m_col[col][row]; }
	inline auto operator[](int row, int col) const -> float { return m_col[col][row]; }
#endif
	float3 m_col[3];
} mat3;

typedef struct float4x4
{
	inline float4x4()  { identity(); }

	inline explicit float4x4(float const A[16])
	{
		m_col[0] = float4{ A[0], A[4], A[8],  A[12] };
		m_col[1] = float4{ A[1], A[5], A[9],  A[13] };
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

	inline explicit float4x4(float3x3 const& m) {
		m_col[0] = float4{ m.m_col[0].x, m.m_col[0].y, m.m_col[0].z, 0.0f };
		m_col[1] = float4{ m.m_col[1].x, m.m_col[1].y, m.m_col[1].z, 0.0f };
		m_col[2] = float4{ m.m_col[2].x, m.m_col[2].y, m.m_col[2].z, 0.0f };
		m_col[3] = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	inline void identity()
	{
		m_col[0] = float4{ 1.0f, 0.0f, 0.0f, 0.0f };
		m_col[1] = float4{ 0.0f, 1.0f, 0.0f, 0.0f };
		m_col[2] = float4{ 0.0f, 0.0f, 1.0f, 0.0f };
		m_col[3] = float4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	inline float4 get_col(int i) const                { return m_col[i]; }
	inline void   set_col(int i, float4 const& a_col) { m_col[i] = a_col; }

	inline float4 get_row(int i) const { return float4{ m_col[0][i], m_col[1][i], m_col[2][i], m_col[3][i] }; }
	inline void   set_row(int i, float4 const& a_col) { m_col[0][i] = a_col[0]; m_col[1][i] = a_col[1]; m_col[2][i] = a_col[2]; m_col[3][i] = a_col[3]; }

	inline       float4& col(int i)       { return m_col[i]; }
	inline float4 const& col(int i) const { return m_col[i]; }

	inline float& operator()(int row, int col)       { return m_col[col][row]; }
	inline float  operator()(int row, int col) const { return m_col[col][row]; }

#if __cplusplus >= 202302L
	inline auto operator[](int row, int col) -> float& { return m_col[col][row]; }
	inline auto operator[](int row, int col) const -> float { return m_col[col][row]; }
#endif

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

	struct ConstRowTmp 
	{
		const float4x4* self;
		int             row;
		inline float operator[](int col) const { return self->m_col[col][row]; }
	};

	inline ConstRowTmp operator[](int a_row) const 
	{
		ConstRowTmp row;
		row.self = this;
		row.row  = a_row;
		return row;
	}

	float4 m_col[4];
} mat4;

inline auto float4::xy()  -> float2& { return reinterpret_cast<float2&>(M); }
inline auto float4::zw()  -> float2& { return reinterpret_cast<float2&>(M[2]); }
inline auto float4::xyz() -> float3& { return reinterpret_cast<float3&>(M); }
inline auto float4::xy()  const -> float2 const& { return reinterpret_cast<float2 const&>(M); }
inline auto float4::zw()  const -> float2 const& { return reinterpret_cast<float2 const&>(M[2]); }
inline auto float4::xyz() const -> float3 const& { return reinterpret_cast<float3 const&>(M); }

inline auto uint4::xy()  -> uint2& { return reinterpret_cast<uint2&>(M); }
inline auto uint4::zw()  -> uint2& { return reinterpret_cast<uint2&>(M[2]); }
inline auto uint4::xyz() -> uint3& { return reinterpret_cast<uint3&>(M); }
inline auto uint4::xy()  const -> uint2 const& { return reinterpret_cast<uint2 const&>(M); }
inline auto uint4::zw()  const -> uint2 const& { return reinterpret_cast<uint2 const&>(M[2]); }
inline auto uint4::xyz() const -> uint3 const& { return reinterpret_cast<uint3 const&>(M); }

inline int2::int2(float2 a) : x(int(a.x)), y(int(a.y)) {}
inline int2::int2(uint2 a) : x(int(a.x)), y(int(a.y)) {}

inline int3::int3(float3 a) : x(int(a.x)), y(int(a.y)), z(int(a.z)) {}
inline int3::int3(uint3 a) : x(int(a.x)), y(int(a.y)), z(int(a.z)) {}

inline int4::int4(float4 a) : x(int(a.x)), y(int(a.y)), z(int(a.z)), w(int(a.w)) {}
inline int4::int4(uint4 a) : x(int(a.x)), y(int(a.y)), z(int(a.z)), w(int(a.w)) {}

inline uint2::uint2(float2 a) : x(uint(a.x)), y(uint(a.y)) {}
inline uint2::uint2(int2 a) : x(uint(a.x)), y(uint(a.y)) {}

inline uint3::uint3(float3 a) : x(uint(a.x)), y(uint(a.y)), z(uint(a.z)) {}
inline uint3::uint3(int3 a) : x(uint(a.x)), y(uint(a.y)), z(uint(a.z)) {}

inline uint4::uint4(float4 a) : x(uint(a.x)), y(uint(a.y)), z(uint(a.z)), w(uint(a.w)) {}
inline uint4::uint4(int4 a) : x(uint(a.x)), y(uint(a.y)), z(uint(a.z)), w(uint(a.w)) {}

inline float4::float4(uint4 a) : x(float(a.x)), y(float(a.y)), z(float(a.z)), w(float(a.w)) {}
inline float4::float4(int4 a) : x(float(a.x)), y(float(a.y)), z(float(a.z)), w(float(a.w)) {}

inline float3x3::float3x3(float4x4 const& from4x4)
{
	m_col[0] = from4x4.m_col[0].xyz();
	m_col[1] = from4x4.m_col[1].xyz();
	m_col[2] = from4x4.m_col[2].xyz();
}

}

#endif // LMATH_TYPES_HPP