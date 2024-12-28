#ifdef USE_MODULES
module;
#define _STRUCTS_EXPORT export
#else
#pragma once
#define _STRUCTS_EXPORT
#endif

#include <cstdint>

#ifdef USE_MODULES
export module Structs;
#endif

_STRUCTS_EXPORT
struct DeleteCopy {
	DeleteCopy() = default;
	DeleteCopy(const DeleteCopy&) = delete;
	DeleteCopy& operator=(const DeleteCopy&) = delete;
	DeleteCopy(DeleteCopy&&) = default;
	DeleteCopy& operator=(DeleteCopy&&) = default;
	~DeleteCopy() = default;
};

_STRUCTS_EXPORT
struct DeleteCopyDeleteMove {
	DeleteCopyDeleteMove() = default;
	DeleteCopyDeleteMove(const DeleteCopyDeleteMove&) = delete;
	DeleteCopyDeleteMove& operator=(const DeleteCopyDeleteMove&) = delete;
	DeleteCopyDeleteMove(DeleteCopyDeleteMove&&) = delete;
	DeleteCopyDeleteMove& operator=(DeleteCopyDeleteMove&&) = delete;
	inline ~DeleteCopyDeleteMove() = default;
};