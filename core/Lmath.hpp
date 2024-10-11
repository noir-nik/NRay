#pragma once
typedef unsigned int uint;

typedef struct ivec4
{
	inline ivec4() : x(0), y(0), z(0), w(0) {}
	inline ivec4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
	inline explicit ivec4(int val) : x(val), y(val), z(val), w(val) {}
	inline explicit ivec4(const int a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
	
	inline int& operator[](int i)       { return M[i]; }
	inline int  operator[](int i) const { return M[i]; }
	union
	{
		struct { int x, y, z, w; };
		struct { int r, g, b, a; };
		int M[4];
	};
} int4;

typedef struct uvec4
{
	inline uvec4() : x(0), y(0), z(0), w(0) {}
	inline uvec4(uint x, uint y, uint z, uint w) : x(x), y(y), z(z), w(w) {}
	inline explicit uvec4(uint val) : x(val), y(val), z(val), w(val) {}
	inline explicit uvec4(const uint a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}
	
	inline uint& operator[](int i)       { return M[i]; }
	inline uint  operator[](int i) const { return M[i]; }
	union
	{
		struct { uint x, y, z, w; };
		struct { uint r, g, b, a; };
		uint M[4];
	};
} uint4;

typedef struct ivec3
{
	inline ivec3() : x(0), y(0), z(0) {}
	inline ivec3(int x, int y, int z) : x(x), y(y), z(z) {}
	inline explicit ivec3(int val) : x(val), y(val), z(val) {}
	inline explicit ivec3(const int a[3]) : x(a[0]), y(a[1]), z(a[2]) {}
	
	inline int& operator[](int i)       { return M[i]; }
	inline int  operator[](int i) const { return M[i]; }

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
} int3;

typedef struct ivec2
{
	inline ivec2() : x(0), y(0) {}
	inline ivec2(int x, int y) : x(x), y(y) {}
	inline explicit ivec2(int val) : x(val), y(val) {}
	inline explicit ivec2(const int a[2]) : x(a[0]), y(a[1]) {}
	
	inline int& operator[](int i)       { return M[i]; }
	inline int  operator[](int i) const { return M[i]; }

	union
	{
	struct { int x, y; };
	struct { int r, g; };
	int M[2];
	};
} int2;

typedef struct uvec3
{
	inline uvec3() : x(0), y(0), z(0) {}
	inline uvec3(uint x, uint y, uint z) : x(x), y(y), z(z) {}
	inline explicit uvec3(uint val) : x(val), y(val), z(val) {}
	inline explicit uvec3(const uint a[3]) : x(a[0]), y(a[1]), z(a[2]) {}
	
	inline uint& operator[](int i)       { return M[i]; }
	inline uint  operator[](int i) const { return M[i]; }

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
} uint3;

typedef struct uvec2
{
	inline uvec2() : x(0), y(0) {}
	inline uvec2(uint x, uint y) : x(x), y(y) {}
	inline explicit uvec2(uint val) : x(val), y(val) {}
	inline explicit uvec2(const uint a[2]) : x(a[0]), y(a[1]) {}
	
	inline uint& operator[](int i)       { return M[i]; }
	inline uint  operator[](int i) const { return M[i]; }

	union
	{
	struct { uint x, y; };
	struct { uint r, g; };
	uint M[2];
	};
} uint2;

typedef struct vec4
{
	inline vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	inline vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline explicit vec4(float val) : x(val), y(val), z(val), w(val) {}
	inline explicit vec4(const float a[4]) : x(a[0]), y(a[1]), z(a[2]), w(a[3]) {}

	inline float& operator[](int i)       { return M[i]; }
	inline float  operator[](int i) const { return M[i]; }

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
} float4;

typedef struct vec3
{
	inline vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	inline vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline explicit vec3(float val) : x(val), y(val), z(val) {}
	inline explicit vec3(const float a[3]) : x(a[0]), y(a[1]), z(a[2]) {}

	inline float& operator[](int i)       { return M[i]; }
	inline float  operator[](int i) const { return M[i]; }

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
} float3;

typedef struct vec2
{
	inline vec2() : x(0.0f), y(0.0f) {}
	inline vec2(float x, float y) : x(x), y(y) {}
	inline explicit vec2(float val) : x(val), y(val) {}
	inline explicit vec2(const float a[2]) : x(a[0]), y(a[1]) {}

	inline float& operator[](int i)       { return M[i]; }
	inline float  operator[](int i) const { return M[i]; }

	union
	{
	struct { float x, y; };
	struct { float r, g; };
	float M[2];
	};
} float2;
