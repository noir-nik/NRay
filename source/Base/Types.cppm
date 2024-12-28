#ifdef USE_MODULES
module;
#define _TYPES_EXPORT export
#else
#pragma once
#define _TYPES_EXPORT
#endif

#include <cstdint>

#ifdef USE_MODULES
export module Types;
#endif


_TYPES_EXPORT using u8  = std::uint8_t;
_TYPES_EXPORT using u16 = std::uint16_t;
_TYPES_EXPORT using u32 = std::uint32_t;
_TYPES_EXPORT using u32 = std::uint32_t;
_TYPES_EXPORT using u64 = std::uint64_t;
_TYPES_EXPORT using i8  = std::int8_t;
_TYPES_EXPORT using i16 = std::int16_t;
_TYPES_EXPORT using i32 = std::int32_t;
_TYPES_EXPORT using i64 = std::int64_t;
_TYPES_EXPORT using f32 = float;
_TYPES_EXPORT using f64 = double;


_TYPES_EXPORT using EntityType = u32;