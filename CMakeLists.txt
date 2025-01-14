cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
  # This specific value changes as experimental support evolves. See
  # `Help/dev/experimental.rst` in the CMake source corresponding to
  # your CMake build for the exact value to use.
  "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")

project(NRay LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_MODULE_STD 1)

find_package(Vulkan REQUIRED)

set(INCLUDE_DIRS
	source/Core
	source/Base
	source/Shaders/include
	source/Engine
	source/Resources
	deps
	deps/vulkan_backend/include
	deps/vulkan_backend/deps/VulkanMemoryAllocator/include
	deps/fastgltf/include
	deps/spdlog/include
	deps/glfw/include
	deps/fmt/include
	deps/imgui
	deps/imgui/backends
	deps/entt/src
	deps/stb
	deps/modules
	deps/modules/imgui
	deps/modules/stb
)

# Main app
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "source/*.cpp" "tests/FeatureTest/*.cpp")

# ImGui
set(IMGUI_SOURCES
	deps/imgui/imgui.cpp
	deps/imgui/imgui_draw.cpp
	deps/imgui/imgui_tables.cpp
	deps/imgui/imgui_widgets.cpp
	deps/imgui/imgui_demo.cpp
	deps/imgui/backends/imgui_impl_glfw.cpp
	deps/imgui/backends/imgui_impl_vulkan.cpp
)

set(ALL_SOURCES
	${SOURCES}
	${IMGUI_SOURCES}
)

if(MSVC)
	add_definitions(
		/DENGINE
		/DVB_IMGUI
		/DVB_GLFW
		/DSPDLOG_COMPILED_LIB
	)
else()
	add_definitions(
		-DENGINE
		-DVB_IMGUI
		-DVB_GLFW
		-DSPDLOG_COMPILED_LIB
		-Wno-nullability-completeness
	)
endif()

add_executable(${PROJECT_NAME} ${ALL_SOURCES} main.cpp)

target_compile_features(${PROJECT_NAME}
  PRIVATE   cxx_std_23
  INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if(USE_MODULES)
	set(VB_BUILD_CPP_MODULE ON CACHE BOOL "")
	set(VB_USE_STD_MODULE ON CACHE BOOL "")
	set(VB_USE_VULKAN_MODULE ON CACHE BOOL "")
	set(FASTGLTF_ENABLE_CPP_MODULES ON CACHE BOOL "")
	set(FASTGLTF_USE_STD_MODULE ON CACHE BOOL "")
endif()

set(FASTGLTF_COMPILE_AS_CPP20 ON CACHE BOOL "")

add_subdirectory(deps/fastgltf)
add_subdirectory(deps/glfw)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/vulkan_backend)

if(USE_MODULES)
	file(GLOB_RECURSE MODULE_SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "source/*.cppm" "deps/modules/*.cppm" "tests/*.cppm")
	target_sources(${PROJECT_NAME} PUBLIC
		FILE_SET CXX_MODULES
		BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
		FILES ${MODULE_SOURCES}
	)
	
	target_link_libraries(${PROJECT_NAME} PRIVATE
		fastgltf::module
		vulkan_backend::module
	)
	target_compile_definitions(${PROJECT_NAME} PUBLIC "USE_MODULES=$<BOOL:${USE_MODULES}>")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	glfw
	Vulkan::Vulkan
	fastgltf::fastgltf
	spdlog::spdlog
	vulkan_backend::vulkan_backend
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_definitions(spdlog PUBLIC SPDLOG_USE_STD_FORMAT)
endif()
