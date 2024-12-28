#ifdef USE_MODULES
export module Structs;
#define _STRUCTS_EXPORT export
#else
#pragma once
#define _STRUCTS_EXPORT
#endif

_STRUCTS_EXPORT
namespace Structs {
struct NoCopy {
	NoCopy() = default;
	NoCopy(const NoCopy&) = delete;
	NoCopy& operator=(const NoCopy&) = delete;
	NoCopy(NoCopy&&) = default;
	NoCopy& operator=(NoCopy&&) = default;
	~NoCopy() = default;
};

struct NoCopyNoMove {
	NoCopyNoMove() = default;
	NoCopyNoMove(const NoCopyNoMove&) = delete;
	NoCopyNoMove& operator=(const NoCopyNoMove&) = delete;
	NoCopyNoMove(NoCopyNoMove&&) = delete;
	NoCopyNoMove& operator=(NoCopyNoMove&&) = delete;
	~NoCopyNoMove() = default;
};

}; // namespace Structs

#undef _STRUCTS_EXPORT