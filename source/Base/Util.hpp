#pragma once
#include <cstdint>
#include <vector>

using Hash64 = std::uint64_t;
using UID = uint32_t;

template <typename T>
struct VectorHash {
	Hash64 operator()(const std::vector<T>& v) const {
		Hash64 hash = v.size();
		for (T num : v) {
			hash ^= std::hash<T>{}(num) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};

template <typename T>
struct VectorEqual {
	bool operator()(const std::vector<T>& a, const std::vector<T>& b) const {
		return a == b;
	}
};

template <typename T>
size_t HashCombine(size_t hash, const T x) {
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = (x >> 16) ^ x;
	return hash ^ x + 0x9e3779b9 + (hash << 6) + (hash >> 2);
};


class UIDGenerator {
public:
	static UID Next();
};