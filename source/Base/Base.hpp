#pragma once

#include <cstdint>
#include <stdint.h>

#include "Log.hpp"

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

#define BIN_PATH "bin"
#define IMG_PATH "img"

#ifdef _WIN32
	#define NRAY_PLATFORM_WINDOWS
#endif

#ifdef __linux__
	#define NRAY_PLATFORM_LINUX
#endif

#ifdef NRAY_DEBUG
	#ifdef NRAY_PLATFORM_WINDOWS
		#define NRAY_DEBUGBREAK() __debugbreak()
	#endif

	#ifdef NRAY_PLATFORM_LINUX
        #include <csignal>
		#define NRAY_DEBUGBREAK() raise(SIGTRAP)
	#endif
#else
	#define NRAY_DEBUGBREAK()
#endif 

#define ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] in {}:{} {}", __FILE__, __LINE__, __VA_ARGS__); exit(1); } }

#ifdef NRAY_DEBUG
#define DEBUG_ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); NRAY_DEBUGBREAK(); } }
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[VULKAN ERROR = {0}] {1}", VK_ERROR_STRING((res)), __VA_ARGS__); NRAY_DEBUGBREAK(); } }
#else
#define DEBUG_ASSERT(condition, ...)
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[{0}] {1} in {2}:{3}", VK_ERROR_STRING(res), __VA_ARGS__, __FILE__, __LINE__); exit(1);}}
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))