cmake_minimum_required(VERSION 3.30)
project(NRay)

set(CMAKE_CXX_STANDARD 23)
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
	${VB_SOURCES}
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

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

add_subdirectory(deps/fastgltf)
add_subdirectory(deps/glfw)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/vulkan_backend)

target_link_libraries(${PROJECT_NAME} PRIVATE
	glfw
	Vulkan::Vulkan
	fastgltf::fastgltf
	spdlog::spdlog
	vulkan_backend
)