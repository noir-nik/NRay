typedef unsigned int uint;

struct ivec3
{
	inline ivec3() : x(0), y(0), z(0) {}
	inline ivec3(uint x, uint y, uint z) : x(x), y(y), z(z) {}
	inline explicit ivec3(uint val) : x(val), y(val), z(val) {}
	inline explicit ivec3(const uint a[3]) : x(a[0]), y(a[1]), z(a[2]) {}
	
	inline uint& operator[](int i)       { return M[i]; }
	inline uint  operator[](int i) const { return M[i]; }

	union
	{
	struct { uint x, y, z; };
	#ifdef LAYOUT_STD140
	uint M[4];
	#else
	uint M[3];
	#endif
	};
};

typedef struct ivec3 ivec3;
typedef struct ivec3 uint3;