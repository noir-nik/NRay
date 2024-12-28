# CC := g++
CC := clang++
# CC := cl
TARGET := NRay

STATIC_LINK := 0
COMPILE_IMGUI := 1

USE_MODULES := 1
USE_HEADER_UNITS := 1

ifeq ($(CC),g++)
USE_MODULES := 0
endif

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
DEPS_PATH := deps

# fastgltf

# SUBMODULE_LIBS_LIST := fastgltf spdlog glfw3
# SUBMODULE_LIBS_LIST := fastgltf

OPT_DEBUG := -O0
OPT_RELEASE := -O2

INCLUDES := \
	-Isource/Core \
	-Isource/Base \
	-Isource/Shaders \
	-Isource/Engine \
	-Isource/Resources \
	-Ideps \
	-Ideps/fastgltf/include \
	-Ideps/spdlog/include \
	-Ideps/glfw/include \
	-Ideps/vma/include \
	-Ideps/fmt/include \
	-Ideps/imgui \
	-Ideps/imgui/backends \
	-Ideps/entt/src \
	-Ideps/stb \
	-Ideps/modules \
	-Ideps/modules/imgui \
	-Ideps/modules/stb \

-O := -o
-OUT := -o
OBJ_EXT := o
LIB_EXT := a
-C := -c

DEFINES := -DENGINE -DUSE_VLA

CXXFLAGS := -MMD -MP $(INCLUDES) $(DEFINES) -std=c++20 -Wno-vla

ifeq ($(STATIC_LINK), 1)
	LDFLAGS += -static -static-libgcc -static-libstdc++
# Not needed
	CXXFLAGS += -static -static-libgcc -static-libstdc++
endif


# endif
# $(info $(INCLUDES))

AR := ar
ARFLAGS = rcs



BUILD_DIR := build
BIN_DIR := bin

ifeq ($(OS),Windows_NT)
	LDFLAGS += -fuse-ld=lld
	PYTHON := python
# -Lbin/lib
	LIBS := $(LIBS) Gdi32 vulkan-1
	PLATFORM_BUILD_DIR := $(BUILD_DIR)/win
	CMAKE_BUILD_DIR := build
	TARGET_DIR := .
	CMD := cmd /c
	
	RM := $(CMD) del /Q
else
	CC := g++
	PYTHON := python3
	LDFLAGS += -lpthread -Lbin/lib-linux
	LIBS := $(LIBS) vulkan GL m
	PLATFORM_BUILD_DIR := $(BUILD_DIR)/linux
	CMAKE_BUILD_DIR := build-linux
	TARGET_DIR := $(PLATFORM_BUILD_DIR)
	RM := rm -rf
# USE_MODULES = 0
endif

LDFLAGS += $(foreach lib,$(LIBS),-l$(lib))
LIB_PATH := $(PLATFORM_BUILD_DIR)/lib
SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/lib$(lib).a)
OBJS_BUILD_DIR := $(PLATFORM_BUILD_DIR)/objs
MODULES_BUILD_DIR := $(PLATFORM_BUILD_DIR)/modules
MODULES_BUILD_DIR_CLANGD := $(PLATFORM_BUILD_DIR)/modules-clangd
MODULES_OBJS_DIR := $(PLATFORM_BUILD_DIR)/modules-objs
HEADERS_BUILD_DIR := $(PLATFORM_BUILD_DIR)/headers

ifeq ($(USE_MODULES), 1)
CXXFLAGS += -fprebuilt-module-path=$(MODULES_BUILD_DIR) -D USE_MODULES  -fmodule-file-deps
# LDFLAGS += $(MODULES_BUILD_DIR)/*.pcm -fprebuilt-module-path=$(MODULES_BUILD_DIR)
# MODULE_NAMES_MAP := $(foreach module,$(CPP_MODULES),-fmodule-file=$(MAIN_MODULE_TARGET):$(module)=$(MODULES_BUILD_DIR)/$(module).pcm)
# CXXFLAGS += $(MODULE_NAMES_MAP)
endif

ifeq ($(USE_HEADER_UNITS), 1)
CXXFLAGS += -D USE_HEADER_UNITS
endif

# $(info $(CPP_MODULE_TARGETS))
# $(info $(MODULE_NAMES_MAP))

ifeq ($(CC),cl)
    INCLUDES := $(patsubst -I%,/I%,$(INCLUDES))
    -O := /Fo
    -OUT := /OUT:
    OBJ_EXT := obj
	LIB_EXT := lib
    -C := /c
    CXXFLAGS := /std:c++20 -DENGINE $(INCLUDES) /utf-8
# LDFLAGS := /L"bin/lib/"
	LDFLAGS := 
    OPT_DEBUG := /O0
    OPT_RELEASE := /O2
	SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/$(lib).lib)
endif


# Folders
SRC_MAIN := .
SRC_CORE := source/Core
SRC_BASE := source/Base
SRC_ENGINE := source/Engine
SRC_RESOURCES := source/Resources
SRC_SHADERS := source/Shaders
SRC_TEST := tests



# # Tests
# SRC_NEURALSFD := tests/NeuralSdf
# SRC_IMAGEOPT := tests/ImageOptimization
# SRC_SLANG := tests/SlangTest
SRC_FEATURE := tests/FeatureTest
# SRC_RADIENCE := tests/RadienceField
# SRC_HELLOTRIANGLE := tests/HelloTriangle
# SRC_WINDOW := tests/Window

# SRC_ALL := $(SRC_NEURALSFD) $(SRC_IMAGEOPT) $(SRC_SLANG) $(SRC_FEATURE) $(SRC_RADIENCE)
tst_dirs := $(wildcard tests/*)
files := $(foreach dir,$(tst_dirs),$(wildcard $(dir)/*.cpp))
SRC_CPP := $(filter %.cpp, $(files) $(tst_dirs))
SRC_CPP_RAW := $(notdir $(SRC_CPP))
# SRC_CPP_RAW := $(notdir $(files))
# $(info $(files))
# $(info $(SRC_CPP))
ifeq ($(findstring clang,$(CC)),clang)
	ifeq ($(OS),Windows_NT)
	CXXFLAGS += -target x86_64-w64-mingw32
	LDFLAGS += -target x86_64-w64-mingw32
	else
	LDFLAGS += -stdlib=libc++
	endif
# for clangd
	CXXFLAGS += -fretain-comments-from-system-headers
	LDFLAGS += -pthread
endif

SRCS := \
	$(wildcard $(SRC_MAIN)/*.cpp) \
	$(wildcard $(SRC_CORE)/*.cpp) \
	$(wildcard $(SRC_BASE)/*.cpp) \
	$(wildcard $(SRC_ENGINE)/*.cpp) \
	$(wildcard $(SRC_RESOURCES)/*.cpp) \
	$(wildcard $(SRC_TEST)/*.cpp) \
	$(wildcard $(SRC_FEATURE)/*.cpp) \

CPP_MODULE_SRCS := \
	$(wildcard $(SRC_BASE)/*.cppm) \
	$(wildcard $(SRC_CORE)/*.cppm) \
	$(wildcard $(SRC_ENGINE)/*.cppm) \
	$(wildcard $(SRC_RESOURCES)/*.cppm) \
	$(wildcard $(SRC_SHADERS)/*.cppm) \
	$(wildcard $(SRC_TEST)/*.cppm) \
	$(wildcard $(SRC_FEATURE)/*.cppm) \

OBJS := $(patsubst %.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(notdir $(SRCS)))

# $(info $(OBJS))
# $(info $(CPP_MODULE_SRCS))

# Libs
# ImGui
SRC_IMGUI := $(DEPS_PATH)/imgui
IMGUI_BACKENDS_DIR := $(DEPS_PATH)/imgui/backends
SRC_IMGUI_BACKENDS := imgui_impl_vulkan.cpp imgui_impl_glfw.cpp
SRC_IMGUI_BACKENDS := $(foreach file,$(SRC_IMGUI_BACKENDS),$(IMGUI_BACKENDS_DIR)/$(file))
OBJS_IMGUI := 	$(patsubst $(SRC_IMGUI)/%.cpp, $(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT), $(wildcard $(SRC_IMGUI)/*.cpp)) \
				$(patsubst $(IMGUI_BACKENDS_DIR)/%.cpp, $(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT), $(SRC_IMGUI_BACKENDS)) 

# Stb	
# SRC_STB := $(DEPS_PATH)/stb_src
# OBJS_STB := $(patsubst $(SRC_STB)/%.c, $(PLATFORM_BUILD_DIR)/stb/%.$(OBJ_EXT), $(wildcard $(SRC_STB)/*.c))

# GLFW
SRC_GLFW := $(DEPS_PATH)/glfw/src
OBJS_GLFW  := $(patsubst $(DEPS_PATH)/glfw/src/%.c, $(PLATFORM_BUILD_DIR)/glfw/%.$(OBJ_EXT), $(wildcard $(SRC_GLFW)/*.c))

# fastgltf
SRC_FASTGLTF := $(DEPS_PATH)/fastgltf/src
OBJS_FASTGLTF := $(patsubst $(SRC_FASTGLTF)/%.cpp, $(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT), $(wildcard $(SRC_FASTGLTF)/*.cpp))

# simdjson
SRC_SIMDJSON :=  $(DEPS_PATH)/fastgltf/deps/simdjson
OBJS_SIMDJSON := $(patsubst $(SRC_SIMDJSON)/%.cpp, $(PLATFORM_BUILD_DIR)/simdjson/%.$(OBJ_EXT), $(SRC_SIMDJSON)/simdjson.cpp)

# spdlog
SRC_SPDLOG := $(DEPS_PATH)/spdlog/src
OBJS_SPDLOG := $(patsubst $(SRC_SPDLOG)/%.cpp, $(PLATFORM_BUILD_DIR)/spdlog/%.$(OBJ_EXT), $(wildcard $(SRC_SPDLOG)/*.cpp))

#fmt
SRC_FMT := $(DEPS_PATH)/fmt/src
OBJS_FMT := $(patsubst $(SRC_FMT)/%.cc, $(PLATFORM_BUILD_DIR)/fmt/%.$(OBJ_EXT), $(wildcard $(SRC_FMT)/*.cc))

# $(info $(OBJS))
# $(patsubst %.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(SRC_CPP_RAW)) \


all: create_dirs release
debug: create_dirs

.PHONY: debug
ifeq ($(CC),cl)
debug: CXXFLAGS += /Zi /Od /DNRAY_DEBUG /EHsc /Fd$(PLATFORM_BUILD_DIR)/$(TARGET).pdb /FS
debug: LDFLAGS  += vulkan-1.lib
	
debug: SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/$(lib)d.lib)
else
debug: CXXFLAGS += -g -ggdb $(OPT_DEBUG) -DNRAY_DEBUG 
debug: LDFLAGS  += -g -ggdb $(OPT_DEBUG)
endif
debug: build_target

.PHONY: release
release: CXXFLAGS += $(OPT_RELEASE)
release: LDFLAGS  += $(OPT_RELEASE)
release: build_target

# build_target: create_dirs .WAIT
# ifeq ($(USE_HEADER_UNITS), 1)
# build_target: build_headers .WAIT
# endif
# ifeq ($(USE_MODULES), 1)
# build_target: build_modules .WAIT
# endif
# build_target: $(TARGET)

build_target: create_dirs

ifeq ($(USE_MODULES), 1)
build_target: get_cpp_module_dependencies
else
# build_target: clean_cpp_module_dependencies
endif
build_target: $(TARGET)

# clean_cpp_module_dependencies:
# @echo "Cleaning cpp module dependencies"
# @echo "" > $(CPP_MODULE_DEPENDENCIES_FILE)


_WINBDIR := $(subst /,\,$(PLATFORM_BUILD_DIR))
_WOBDIR := $(subst /,\,$(OBJS_BUILD_DIR))
_WMBDIR := $(subst /,\,$(MODULES_BUILD_DIR))
_WMODIR := $(subst /,\,$(MODULES_OBJS_DIR))
_WMBDIRC := $(subst /,\,$(MODULES_BUILD_DIR_CLANGD))
_WHBDIR := $(subst /,\,$(HEADERS_BUILD_DIR))
create_dirs:
ifeq ($(OS),Windows_NT)
	@cmd /c if not exist $(TARGET_DIR) mkdir $(TARGET_DIR)
	@cmd /c if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@cmd /c if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cmd /c if not exist $(_WINBDIR) mkdir $(_WINBDIR)
	@cmd /c if not exist $(_WOBDIR) mkdir $(_WOBDIR)
	@cmd /c if not exist $(_WMBDIR) mkdir $(_WMBDIR)
	@cmd /c if not exist $(_WMODIR) mkdir $(_WMODIR)
	@cmd /c if not exist $(_WMBDIRC) mkdir $(_WMBDIRC)
	@cmd /c if not exist $(_WHBDIR) mkdir $(_WHBDIR)
	@cmd /c if not exist $(_WINBDIR)\imgui mkdir $(_WINBDIR)\imgui
# @cmd /c if not exist $(_WINBDIR)\stb mkdir $(_WINBDIR)\stb
	@cmd /c if not exist $(_WINBDIR)\glfw mkdir $(_WINBDIR)\glfw
	@cmd /c if not exist $(_WINBDIR)\fastgltf mkdir $(_WINBDIR)\fastgltf
	@cmd /c if not exist $(DEPS_PATH)\fastgltf\deps\simdjson mkdir $(DEPS_PATH)\fastgltf\deps\simdjson
	@cmd /c if not exist $(_WINBDIR)\simdjson mkdir $(_WINBDIR)\simdjson
	@cmd /c if not exist $(_WINBDIR)\spdlog mkdir $(_WINBDIR)\spdlog
	@cmd /c if not exist $(_WINBDIR)\fmt mkdir $(_WINBDIR)\fmt
else
	@mkdir -p \
	$(TARGET_DIR) $(BIN_DIR) \
	$(BUILD_DIR) \
	$(PLATFORM_BUILD_DIR) \
	$(OBJS_BUILD_DIR) \
	$(MODULES_BUILD_DIR) \
	$(MODULES_OBJS_DIR) \
	$(MODULES_BUILD_DIR_CLANGD) \
	$(HEADERS_BUILD_DIR) \
	$(PLATFORM_BUILD_DIR)/imgui \
	$(PLATFORM_BUILD_DIR)/glfw \
	$(PLATFORM_BUILD_DIR)/fastgltf \
	$(DEPS_PATH)/fastgltf/deps/simdjson \
	$(PLATFORM_BUILD_DIR)/simdjson \
	$(PLATFORM_BUILD_DIR)/spdlog \
	$(PLATFORM_BUILD_DIR)/fmt \

# $(PLATFORM_BUILD_DIR)/stb \


endif

CPP_SYSTEM_HEADERS := \
	array \
	atomic \
	chrono \
	filesystem \
	fstream \
	map \
	memory \
	mutex \
	numeric \
	random \
	set \
	span \
	string \
	string_view \
	thread \
	tuple \
	unordered_map \
	vector \
	\
	iostream \
	\
	cassert \
	cmath \
	csignal \
	cstddef \
	cstdint \
	cstdlib \
	cstring \

CPP_SYSTEM_HEADER_TARGETS := \
	$(patsubst %, $(HEADERS_BUILD_DIR)/%.pcm, $(CPP_SYSTEM_HEADERS)) \
	
CPP_HEADERS := entt-entity-registry.hpp
# CPP_HEADERS := registry

CPP_HEADER_TARGETS := \
	$(patsubst %.hpp, $(HEADERS_BUILD_DIR)/%.pcm, $(CPP_HEADERS)) \
	

CPP_MODULE_DEPENDENCIES_FILE := $(BUILD_DIR)/cpp_module_dependencies.mk

$(BUILD_DIR)/%.mk: scripts/generate_cpp_module_dependencies.py $(CPP_MODULE_SRCS) $(SRCS)
ifeq ($(OS),Windows_NT)
	@cmd /c if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
else
	@mkdir -p $(BUILD_DIR)
endif
	@echo "Generating $(CPP_MODULE_DEPENDENCIES_FILE)"
	@PYTHON $< > $@


	
# $(subst .cppm,,$(wildcard $(SRC_SHADERS)/*.cppm))
# CPP_MODULES := $(CPP_MODULES_PRIMARY) $(filter-out $(CPP_MODULES_PRIMARY),$(subst .cppm,,$(CPP_MODULES))))
CPP_MODULES := $(notdir $(subst .cppm,,$(CPP_MODULE_SRCS)))
# CPP_MODULES := $(CPP_MODULES_PRIMARY) $(notdir $(subst .cppm,,$(CPP_MODULES)))
# $(_MBD)/UI.pcm:  $(_MBD)/FeatureTest.pcm
# $(info $(CPP_MODULES))
# $(info $(CPP_MODULE_DEPENDENCIES_FILE))

EXTERNAL_MODULES_DIR := deps/modules

EXTERNAL_MODULES := \
	$(patsubst %.cppm,%,$(notdir $(wildcard $(EXTERNAL_MODULES_DIR)/*.cppm))) \
	$(patsubst %.cppm,%,$(notdir $(wildcard $(EXTERNAL_MODULES_DIR)/imgui/*.cppm))) \
	$(patsubst %.cppm,%,$(notdir $(wildcard $(EXTERNAL_MODULES_DIR)/stb/*.cppm))) \
	fastgltf 

EXTERNAL_MODULE_TARGETS := $(foreach module,$(EXTERNAL_MODULES),$(MODULES_BUILD_DIR)/$(module).pcm) 

MAIN_MODULE_TARGET := NRay
CPP_MODULE_TARGETS := $(foreach module,$(CPP_MODULES),$(MODULES_BUILD_DIR)/$(module).pcm)

CPP_MODULE_OBJS := $(patsubst %.pcm, $(MODULES_OBJS_DIR)/%.o, $(notdir $(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS)))

# $(CPP_MODULE_TARGETS) : get_cpp_module_dependencies
# .PHONY: get_cpp_module_dependencies
get_cpp_module_dependencies: $(CPP_MODULE_DEPENDENCIES_FILE)

# $(info $(CPP_MODULE_TARGETS))

# $(info $(EXTERNAL_MODULE_TARGETS))
STL_MODULE_TARGET := $(MODULES_BUILD_DIR)/stl.pcm

$(TARGET): $(SUBMODULE_LIBS)
# $(TARGET): $(wildcard $(LIB_PATH)/*.lib)
# $(CPP_MODULE_TARGETS): $(EXTERNAL_MODULE_TARGETS)

ifeq ($(USE_HEADER_UNITS), 1)
$(STL_MODULE_TARGET) : $(CPP_SYSTEM_HEADER_TARGETS)
endif

# $(OBJS): $(CPP_MODULE_TARGETS)
_MBD := $(MODULES_BUILD_DIR)
_OBD := $(OBJS_BUILD_DIR)
ifeq ($(USE_MODULES), 1)
-include $(CPP_MODULE_DEPENDENCIES_FILE)
endif
# $(TARGET): $(OBJS) $(OBJS_IMGUI) $(OBJS_STB)
$(TARGET): \
	$(OBJS) \
	$(OBJS_IMGUI) \
	$(OBJS_GLFW) \
	$(OBJS_FASTGLTF) \
	$(OBJS_SIMDJSON) \
	$(OBJS_SPDLOG) \

ifeq ($(USE_MODULES), 1)
$(TARGET): $(CPP_MODULE_OBJS)
endif
	
	
# $(OBJS_STB) \
# $(OBJS_FMT)
# @echo "Linking $(notdir $^)"
	@echo "Linking"
ifeq ($(CC),cl)
	@link $(-OUT)$(TARGET_DIR)/$@ $^ $(LDFLAGS) 
else
	@$(CC) $(-OUT)$(TARGET_DIR)/$@ $^ $(LDFLAGS)
endif
	@echo "=== Done ==="

#compile
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_MAIN)/%.cpp # ./
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_CORE)/%.cpp # core/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $< -Wno-nullability-completeness
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_BASE)/%.cpp # base/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_TEST)/%.cpp # test/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_ENGINE)/%.cpp # engine/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_RESOURCES)/%.cpp # resources/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $< 

# Tests
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_NEURALSFD)/%.cpp # tests/NeuralSdf
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_IMAGEOPT)/%.cpp # tests/ImageOptimization
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_SLANG)/%.cpp # tests/SlangTest
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_FEATURE)/%.cpp # tests/FeatureTest
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_RADIENCE)/%.cpp # tests/RadienceField
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_HELLOTRIANGLE)/%.cpp # tests/HelloTriangle
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_WINDOW)/%.cpp # tests/Window
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

# ============================================ Headers ===================================================

HEADER_FLAGS := $(filter-out -fmodule-file-deps,$(CXXFLAGS)) \
	-Wno-pragma-system-header-outside-header \
	-Wno-user-defined-literals \
	-Wno-invalid-constexpr \
	-Wno-unknown-warning-option

$(HEADERS_BUILD_DIR)/%.pcm:
	@echo "Compiling <$(patsubst %.pcm,%,$(notdir $@))>"
	@$(CC) $(HEADER_FLAGS) --precompile -xc++-system-header $(patsubst %.pcm,%,$(notdir $@)) -o $@

# $(HEADERS_BUILD_DIR)/entt-entity-registry.pcm: deps/entt/src/entt/entity/registry.hpp
# 	@echo "Compiling <$(patsubst %.pcm,%,$(notdir $@))>"
# 	@$(CC) $(HEADER_FLAGS) --precompile -xc++-user-header $< -o $@


# stl module
_STL_MODULE_FLAGS := \
	$(filter-out -fmodule-file-deps,$(CXXFLAGS)) \
	$(foreach header,$(CPP_SYSTEM_HEADERS),-fmodule-file=$(HEADERS_BUILD_DIR)/$(header).pcm) \
	-Wno-experimental-header-units

# $(STL_MODULE_TARGET) : $(CPP_SYSTEM_HEADER_TARGETS)

$(MODULES_BUILD_DIR)/stl.pcm: source/Base/stl.cppm
# @rm -f $@
	@echo "Compiling module stl"
	@$(CC) $(_STL_MODULE_FLAGS) --precompile -c -x c++-module $< -o $@

# $(MODULES_BUILD_DIR)/entt.pcm: $(CPP_HEADER_TARGETS)

# deps/modules/entt.cppm: entt-entity-registry.pcm 

# $(MODULES_BUILD_DIR)/entt.pcm: $(HEADERS_BUILD_DIR)/entt-entity-registry.pcm 
# @rm -f $@
# 	@echo "Compiling module entt"
# 	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) -fmodule-file=$< --precompile -c -x c++-module $< -o $@



# ============================================ Modules ===================================================
modules: CXXFLAGS += -fprebuilt-module-path=$(MODULES_BUILD_DIR) -D USE_MODULES -fmodule-file-deps
modules: $(CPP_MODULE_DEPENDENCIES_FILE) $(EXTERNAL_MODULE_TARGETS) $(CPP_MODULE_TARGETS) 

CLANGD_MODULE_TARGETS := $(subst $(MODULES_BUILD_DIR),$(MODULES_BUILD_DIR_CLANGD),$(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS) $(STL_MODULE_TARGET))

# $(info $(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS) $(STL_MODULE_TARGET))
# $(info $(CLANGD_MODULE_TARGETS))

modules_clangd: modules
modules_clangd: $(CLANGD_MODULE_TARGETS)

$(MODULES_BUILD_DIR_CLANGD)/%.pcm: $(MODULES_BUILD_DIR)/%.pcm
	@rm -f $@
	@cp $< $@
# ln -sf $< $@
# @echo "mklink $(subst /,\,$@) ..\modules\$(notdir $<)"

# ifeq (modules,$(MAKECMDGOALS))
ifeq ($(findstring modules,$(MAKECMDGOALS)),modules)
-include $(CPP_MODULE_DEPENDENCIES_FILE)
endif


# $(_MBD)/*.pcm: 

# build_modules: build_internal_modules

# .NOTPARALLEL: build_internal_modules
# build_internal_modules: $(CPP_MODULE_TARGETS)

# MODULE_FLAGS := $(filter-out -fmodule-file-deps,$(CXXFLAGS))
# $(info $(CXXFLAGS))
# $(info $(filter-out -fmodule-file-deps,$(CXXFLAGS)))

# $(patsubst $(MODULES_BUILD_DIR)/%.pcm,$(MODULES_BUILD_DIR)/$(MAIN_MODULE_TARGET)-%.pcm, $@)
# $(MODULES_BUILD_DIR)/%.pcm: source/Core/%.cppm

# _MBD := $(MODULES_BUILD_DIR)
# _OBD := $(OBJS_BUILD_DIR)


$(_MBD)/%.pcm: source/Core/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: source/Base/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: source/Engine/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: source/Resources/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: source/Shaders/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(SRC_FEATURE)/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@


# build_external_modules: $(EXTERNAL_MODULE_TARGETS)

$(_MBD)/%.pcm: $(EXTERNAL_MODULES_DIR)/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@ -Wno-nullability-completeness

$(_MBD)/%.pcm: $(EXTERNAL_MODULES_DIR)/imgui/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(EXTERNAL_MODULES_DIR)/stb/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

# #entt
# $(_MBD)/%.pcm: deps\entt\src\entt\entity\registry.hpp
# @rm -f $@
# 	@echo "Compiling module $(notdir $<)"
# 	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

# fastgltf
$(_MBD)/%.pcm: deps/fastgltf/src/%.ixx
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@


# ========================== Module Objs =============================

$(MODULES_OBJS_DIR)/%.o: $(MODULES_BUILD_DIR)/%.pcm
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) -c $< -o $@ -Wno-unused-command-line-argument

# ====================================================================

# ============================================ Libraries ===================================================

#ImGui
$(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT): $(SRC_IMGUI)/%.cpp # imgui/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT): $(IMGUI_BACKENDS_DIR)/%.cpp # imgui/backends/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

#Stb
$(PLATFORM_BUILD_DIR)/stb/%.$(OBJ_EXT): $(SRC_STB)/%.c # stb/
	@echo "Compiling $(notdir $<)"
	@$(CC) -x c $(filter-out -std=c++20,$(CXXFLAGS)) $(-O)$@ $(-C) $<

# GLFW
ifeq ($(OS),Windows_NT)
GLFW_PLATFORM := _GLFW_WIN32
else
GLFW_PLATFORM := _GLFW_X11
endif

ifeq ($(findstring clang,$(CC)),clang)
_CLANG_LIBFLAGS := -target x86_64-w64-mingw32
endif

$(PLATFORM_BUILD_DIR)/glfw/%.$(OBJ_EXT): $(SRC_GLFW)/%.c # glfw/
	@echo "Compiling $(notdir $<)"
	@$(CC) -x c $(filter-out -std=c++20,$(CXXFLAGS)) -c $< $(-O)$@ -D$(GLFW_PLATFORM) -Wno-unused-command-line-argument

ifeq ($(findstring clang,$(CC)),clang)
_CLANG_FASTGLTF := -femulated-tls
endif


# Fastgltf
$(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT): $(SRC_FASTGLTF)/%.cpp $(SRC_SIMDJSON)/simdjson.h
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fastgltf/include/ -I$(DEPS_PATH)/fastgltf/deps/simdjson $(_CLANG_FASTGLTF)

# Simdjson
SIMDJSON_TARGET_VERSION := 3.9.4
$(SRC_SIMDJSON)/simdjson.cpp:
	@echo "Downloading $(notdir $@) v$(SIMDJSON_TARGET_VERSION)"
	@curl -L -o $@ https://raw.githubusercontent.com/simdjson/simdjson/v$(SIMDJSON_TARGET_VERSION)/singleheader/simdjson.cpp

$(SRC_SIMDJSON)/simdjson.h:
	@echo "Downloading $(notdir $@) v$(SIMDJSON_TARGET_VERSION)"
	@curl -L -o $@ https://raw.githubusercontent.com/simdjson/simdjson/v$(SIMDJSON_TARGET_VERSION)/singleheader/simdjson.h
# curl -L -o
$(PLATFORM_BUILD_DIR)/simdjson/%.$(OBJ_EXT): $(SRC_SIMDJSON)/%.cpp $(SRC_SIMDJSON)/%.h # simdjson/
	@echo "Compiling $(notdir $<)"
	@$(CC) -MMD -MP $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fastgltf/deps/simdjson -O3 $(_CLANG_LIBFLAGS)


# spdlog
$(PLATFORM_BUILD_DIR)/spdlog/%.$(OBJ_EXT): $(SRC_SPDLOG)/%.cpp # spdlog/
	@echo "Compiling $(notdir $<)"
	@$(CC) -MMD -MP $(-O)$@ $(-C) $< -O3 -DSPDLOG_COMPILED_LIB -DSPDLOG_NO_EXCEPTIONS -I$(DEPS_PATH)/spdlog/include $(_CLANG_LIBFLAGS)
# -I$(DEPS_PATH)/fmt/include -DSPDLOG_FMT_EXTERNAL -DSPDLOG_FMT_EXTERNAL_HO

#fmt
$(PLATFORM_BUILD_DIR)/fmt/%.$(OBJ_EXT): $(SRC_FMT)/%.cc # fmt/
	@echo "Compiling $(notdir $<)"
	@$(CC) -MMD -MP $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fmt/include -fmodules-ts $(_CLANG_LIBFLAGS)




DEPFILES =  $(OBJS:.$(OBJ_EXT)=.d) \
			$(OBJS_IMGUI:.$(OBJ_EXT)=.d) \
			$(OBJS_GLFW:.$(OBJ_EXT)=.d) \
			$(CPP_MODULE_TARGETS:.pcm=.d) \

# $(OBJS_STB:.$(OBJ_EXT)=.d) \
# $(OBJS_FASTGLTF:.$(OBJ_EXT)=.d) \
# $(OBJS_SPDLOG:.$(OBJ_EXT)=.d) \
# $(OBJS_IMGUI:.$(OBJ_EXT)=.d) \
# $(OBJS_GLFW:.$(OBJ_EXT)=.d) \

# $(OBJS_FMT:.$(OBJ_EXT)=.d)

-include $(wildcard $(DEPFILES))

# Run
run:
	@./$(TARGET_DIR)/$(TARGET)


# VLGRND_OUTPUT := --log-file="bin/mem"
# VLGRND_FULL := --leak-check=full 
# VLGRND_FULL += --show-leak-kinds=all

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(TARGET_DIR)/$(TARGET) 
# @valgrind ./$(TARGET_DIR)/$(TARGET)

# $(info $(DEPFILES))
# .PHONY: clean cleanall cleanheaders cleanmodules cleanlib cleanimgui cleanstb cleanglfw cleanfastgltf cleanspdlog cleanfmt .WAIT

clean:
	@rm -f $(OBJS) $(DEPFILES) $(PLATFORM_BUILD_DIR)/$(TARGET).pdb $(wildcard $(OBJS_BUILD_DIR)/*.o) $(wildcard $(OBJS_BUILD_DIR)/*.obj)
	@echo "=== Cleaned ==="

cleanall:
	@rm -rf $(PLATFORM_BUILD_DIR)/* 
	@echo "=== Cleaned ==="

cleanheaders:
	@rm -f $(HEADERS_BUILD_DIR)/* 
	@echo "=== Cleaned ==="

cleanmodules:
	@rm -f $(MODULES_BUILD_DIR)/*
# @rm -f $(MODULES_BUILD_DIR_CLANGD)/*
	@echo "=== Cleaned ==="

cleancl:
	@rm -f $(MODULES_BUILD_DIR_CLANGD)/*
	@echo "=== Cleaned ==="

cleanlib:
	@rm -f $(wildcard $(LIB_PATH)/*.lib) $(wildcard $(LIB_PATH)/*.a)
	@echo "=== Cleaned ==="

cleanimgui:
	@rm -f $(LIB_PATH)/libimgui.a
	@rm -f $(OBJS_IMGUI) $(OBJS_IMGUI:.o=.d)
	@echo "=== Cleaned ==="

# cleanstb:
# 	@rm -f $(OBJS_STB) $(OBJS_STB:.o=.d)
# 	@echo "=== Cleaned ==="

cleanglfw:
	@rm -f $(OBJS_GLFW) $(OBJS_GLFW:.o=.d) $(wildcard ./*.obj)
	@echo "=== Cleaned ==="

rm:
	$(RM) $(wildcard *.bmp)


# ===== Libraries ======



CMAKE_FLAGS :=  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(abspath $(LIB_PATH)) \
				-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$(abspath $(LIB_PATH)) \
				-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG=$(abspath $(LIB_PATH)) \
				-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG=$(abspath $(LIB_PATH)) \
				-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE=$(abspath $(LIB_PATH)) \
				-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE=$(abspath $(LIB_PATH)) \

# -DMSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>"

ifeq ($(CC),cl)


CMAKE_CXX_FLAGS :=
CMAKE_FLAGS += 

$(LIB_PATH)/fastgltf.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS}
	@cmake --build $(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR)

$(LIB_PATH)/fmt.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/fmt -B$(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" -DFMT_TEST=OFF
	@cmake --build $(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR)

$(LIB_PATH)/spdlog.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/spdlog -B$(DEPS_PATH)/spdlog/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" -DSPDLOG_BUILD_TESTS=OFF 
# -DSPDLOG_FMT_EXTERNAL=ON -Dfmt_DIR="$(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR)"
	@cmake --build $(DEPS_PATH)/spdlog/$(CMAKE_BUILD_DIR)


else

ifeq ($(OS),Windows_NT)
# CMAKE_CXX_FLAGS := -target x86_64-w64-mingw32
# CMAKE_FLAGS +=  -G "MinGW Makefiles" \
# 				-DCMAKE_EXE_LINKER_FLAGS_INIT="-fuse-ld=lld" \
# 				-DCMAKE_MODULE_LINKER_FLAGS_INIT="-fuse-ld=lld" \
# 				-DCMAKE_SHARED_LINKER_FLAGS_INIT="-fuse-ld=lld" \
# 				-DCMAKE_LINKER_FLAGS_INIT="-fuse-ld=lld" \
# 				-DCMAKE_C_COMPILER_TARGET="x86_64-windows-gnu"


else

endif

ifeq ($(OS),Windows_NT)
$(LIB_PATH)/libfastgltf.$(LIB_EXT):
# cmake $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/build -G "MinGW Makefiles" ${CMAKE_FLAGS} -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ 
# @cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++  -DFASTGLTF_COMPILE_AS_CPP20=YES -DFASTGLTF_ENABLE_EXAMPLES=NO -DFASTGLTF_ENABLE_CPP_MODULES=YES
	@cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="-target x86_64-w64-mingw32 -femulated-tls" -DFASTGLTF_COMPILE_AS_CPP20=YES -DFASTGLTF_ENABLE_EXAMPLES=NO -DFASTGLTF_ENABLE_CPP_MODULES=YES
	@cmake --build $(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR)
else
$(LIB_PATH)/libfastgltf.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DFASTGLTF_COMPILE_AS_CPP20=ON
	@cmake --build $(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR)
endif

$(LIB_PATH)/libfmt.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/fmt -B$(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" -DFMT_TEST=OFF
	@cmake --build $(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR)

$(LIB_PATH)/libspdlog.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/spdlog -B$(DEPS_PATH)/spdlog/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" -DSPDLOG_BUILD_TESTS=OFF 
# -DSPDLOG_FMT_EXTERNAL=ON -Dfmt_DIR="$(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR)"
	@cmake --build $(DEPS_PATH)/spdlog/$(CMAKE_BUILD_DIR)



# apt install pkg-config
# apt install libxkbcommon-dev
# apt install libxinerama-dev
# apt install libxcursor-dev
# apt install libxi-dev

endif

$(LIB_PATH)/glfw3.$(LIB_EXT):
	@cmake --fresh $(DEPS_PATH)/glfw -B$(DEPS_PATH)/glfw/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DGLFW_BUILD_TESTS=0
	@cmake --build $(DEPS_PATH)/glfw/$(CMAKE_BUILD_DIR)


# $(LIB_PATH)/libimgui.a: $(OBJS_IMGUI)
# 	$(AR) $(ARFLAGS) $@ $^

# $(LIB_PATH)/libstb.a: $(OBJS_STB)
# 	$(AR) $(ARFLAGS) $@ $^

# $(LIB_PATH)/libglfw3.a: $(GLFW_OBJS)
#	@$(AR) $(ARFLAGS) $@ $^
 