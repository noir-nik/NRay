#pragma once

#include <stdint.h>

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

#ifdef _WIN32
	#define APP_PLATFORM_WINDOWS
#endif

#ifdef __linux__
	#define APP_PLATFORM_LINUX
#endif

#ifdef APP_DEBUG
	#ifdef APP_PLATFORM_WINDOWS
		#define APP_DEBUGBREAK() __debugbreak()
	#endif

	#ifdef APP_PLATFORM_LINUX
        #include <csignal>
		#define APP_DEBUGBREAK() raise(SIGTRAP)
	#endif
#else
	#define APP_DEBUGBREAK()
#endif 

#define ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0} in {1}:{2}", __VA_ARGS__, __FILE__, __LINE__); exit(1); } }

#ifdef APP_DEBUG
#define DEBUG_ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); APP_DEBUGBREAK(); } }
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[VULKAN ERROR = {0}] {1}", VK_ERROR_STRING((res)), __VA_ARGS__); APP_DEBUGBREAK(); } }
#else
#define DEBUG_ASSERT(condition, ...)
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[{0}] {1} in {2}:{3}", VK_ERROR_STRING(res), __VA_ARGS__, __FILE__, __LINE__); exit(1);}}
#endif