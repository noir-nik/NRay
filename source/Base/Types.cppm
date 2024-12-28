#ifdef USE_MODULES
module;
#define _TYPES_EXPORT export
#else
#pragma once
#define _TYPES_EXPORT
#endif

#include <cstdint>
#include <stdint.h>

#ifdef USE_MODULES
export module Types;
#endif


_TYPES_EXPORT using u8  = uint8_t;
_TYPES_EXPORT using u16 = uint16_t;
_TYPES_EXPORT using u32 = uint32_t;
_TYPES_EXPORT using u32 = uint32_t;
_TYPES_EXPORT using u64 = uint64_t;
_TYPES_EXPORT using i8  = int8_t;
_TYPES_EXPORT using i16 = int16_t;
_TYPES_EXPORT using i32 = int32_t;
_TYPES_EXPORT using i64 = int64_t;
_TYPES_EXPORT using f32 = float;
_TYPES_EXPORT using f64 = double;