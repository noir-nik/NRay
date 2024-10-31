#pragma once
#include <cstdint>
#include <vector>

using Hash64 = std::uint64_t;

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