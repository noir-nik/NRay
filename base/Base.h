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

#define ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); abort(); } }

#ifdef APP_DEBUG
#define DEBUG_ASSERT(condition, ...) { if (!(condition)) { LOG_ERROR("[ASSERTION FAILED] {0}", __VA_ARGS__); APP_DEBUGBREAK(); } }
#define DEBUG_VK(res, ...) { if ((res) != VK_SUCCESS) { LOG_ERROR("[VULKAN ERROR = {0}] {1}", VK_ERROR_STRING((res)), __VA_ARGS__); APP_DEBUGBREAK(); } }
#else
#define DEBUG_ASSERT(condition, ...)
#define DEBUG_VK(res, ...)
#endif