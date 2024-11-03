#pragma once
#include <cstdint>
#include <vector>
#include <cstddef>

using Hash64 = std::uint64_t;
using UID = uint32_t;

inline size_t HashCombine(size_t hash, size_t x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return hash ^ x + 0x9e3779b9 + (hash << 6) + (hash >> 2);
};

template <typename T>
inline size_t VectorHash(const std::vector<T>& v) {
	size_t hash = 0;
	for (T num : v) {
		hash = HashCombine(hash, num);
	}
	return hash;
}


class UIDGenerator {
public:
	static UID Next();
};