#ifdef USE_MODULES
export module Util;
import stl;
#define _UTIL_EXPORT export
#else
#pragma once
#define _UTIL_EXPORT
#include <cstdint>
#include <vector>

#ifdef __unix__ 
#include <cstddef>
#endif

#endif

_UTIL_EXPORT
using Hash64 = std::uint64_t;
_UTIL_EXPORT
using UID = uint32_t;

_UTIL_EXPORT
inline size_t HashCombine(size_t hash, size_t x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return hash ^ x + 0x9e3779b9 + (hash << 6) + (hash >> 2);
};

_UTIL_EXPORT
template <typename T>
inline size_t VectorHash(const std::vector<T>& v) {
	size_t hash = 0;
	for (T num : v) {
		hash = HashCombine(hash, num);
	}
	return hash;
}

_UTIL_EXPORT
class UIDGenerator {
public:
	static UID Next();
};