CXX := clang++

TARGET := NRay

STATIC_LINK := 0
COMPILE_IMGUI := 1

USE_MODULES := 1
USE_HEADER_UNITS := 0
USE_EXCEPTIONS := 0
USE_VLA := 1

SPDLOG_COMPILED_LIB := 0

STD_MODULE_AVAILABLE := 1

CPP_STD := -std=c++23

AR := ar
ARFLAGS = rcs

BUILD_DIR := build
BIN_DIR := bin
DEPS_PATH := deps

OPT_DEBUG := -O0
OPT_RELEASE := -O2

-O := -o
-OUT := -o
OBJ_EXT := o
LIB_EXT := a
-C := -c

ifeq ($(OS),Windows_NT)
	LDFLAGS += -fuse-ld=lld
	PYTHON := python
	LIBS := $(LIBS) Gdi32 vulkan-1
	PLATFORM_BUILD_DIR := $(BUILD_DIR)/win
	CMAKE_BUILD_DIR := build
	TARGET_DIR := .
	CMD := cmd /c
	
	RM := $(CMD) del /Q
else
	CXX := g++
	PYTHON := python3
	LDFLAGS += -lpthread
	LIBS := $(LIBS) vulkan GL m
	PLATFORM_BUILD_DIR := $(BUILD_DIR)/linux
	CMAKE_BUILD_DIR := build-linux
	TARGET_DIR := $(PLATFORM_BUILD_DIR)
	RM := rm -rf
endif

ifeq ($(findstring clang,$(CXX)),clang)
	ifeq ($(OS),Windows_NT)
		TARGET_TRIPLE := -target x86_64-w64-mingw32
	else
		LDFLAGS += -stdlib=libc++
	endif
	CXXFLAGS += -fretain-comments-from-system-headers
	LDFLAGS += -pthread
endif

ifneq ($(findstring clang,$(CXX)),clang)
USE_MODULES := 0
STD_MODULE_AVAILABLE := 0
endif

CXXFLAGS += $(TARGET_TRIPLE)
LDFLAGS += $(TARGET_TRIPLE)

# ifeq ($(USE_MODULES),1)
# CXXFLAGS += -DVB_VMA_IMPLEMENTATION=1
CXXFLAGS += -D_VB_EXT_MODULES
# endif

ifeq ($(USE_MODULES), 0)
	SPDLOG_COMPILED_LIB := 1
endif

INCLUDES := \
	-Isource/Core \
	-Isource/Base \
	-Isource/Shaders/include \
	-Isource/Engine \
	-Isource/Resources \
	-Ideps \
	-Ideps/vulkan_backend/include \
	-Ideps/vulkan_backend/deps/VulkanMemoryAllocator/include \
	-Ideps/fastgltf/include \
	-Ideps/spdlog/include \
	-Ideps/glfw/include \
	-Ideps/fmt/include \
	-Ideps/imgui \
	-Ideps/imgui/backends \
	-Ideps/entt/src \
	-Ideps/stb \
	-Ideps/modules \
	-Ideps/modules/imgui \
	-Ideps/modules/stb \

WARNINGS_DISABLE := \
	-Wno-missing-field-initializers \
	-Wno-unused-variable \
	-Wno-unused-parameter \
	-Wno-unused-function \
	\
	-Wno-unknown-pragmas \

ifeq ($(findstring clang,$(CXX)),clang)
WARNINGS_DISABLE += \
	-Wno-missing-designated-field-initializers \
	-Wno-unused-command-line-argument \
	-Wno-nullability-completeness

endif

WARNINGS_ENABLE := \
	-Wsequence-point

VB_DEFINES := \
	-DVB_USE_STD_MODULE=1 \
	-DVB_USE_VLA \
	-D VB_IMGUI \
	-DVB_GLFW \

DEFINES := -DENGINE
WARNINGS = -Wall -Wextra

CXXFLAGS += \
	-MMD -MP \
	$(INCLUDES) \
	$(DEFINES) \
	$(CPP_STD) \
	$(WARNINGS) \
	$(WARNINGS_DISABLE) \
	$(WARNINGS_ENABLE) \
	$(VB_DEFINES) \
	-g -ggdb


ifeq ($(USE_EXCEPTIONS), 0)
	CXXFLAGS += -fno-exceptions -DSPDLOG_NO_EXCEPTIONS
endif

ifeq ($(USE_VLA), 1)
	CXXFLAGS += -DUSE_VLA -Wno-vla
endif

ifeq ($(SPDLOG_COMPILED_LIB), 1)
	CXXFLAGS += -DSPDLOG_COMPILED_LIB
endif

ifeq ($(STATIC_LINK), 1)
	LDFLAGS += -static -static-libgcc -static-libstdc++
# Not needed
	CXXFLAGS += -static -static-libgcc -static-libstdc++
endif


LDFLAGS += $(foreach lib,$(LIBS),-l$(lib))
LIB_PATH := $(PLATFORM_BUILD_DIR)/lib
SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/lib$(lib).a)
OBJS_BUILD_DIR := $(PLATFORM_BUILD_DIR)/objs
MODULES_BUILD_DIR := $(PLATFORM_BUILD_DIR)/modules
MODULES_BUILD_DIR_CLANGD := $(PLATFORM_BUILD_DIR)/modules-clangd
MODULES_OBJS_DIR := $(PLATFORM_BUILD_DIR)/modules-objs
HEADERS_BUILD_DIR := $(PLATFORM_BUILD_DIR)/headers

DIRS_CREATED_FILE := $(OBJS_BUILD_DIR)/dirs_created

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

ifeq ($(CXX),cl)
    INCLUDES := $(patsubst -I%,/I%,$(INCLUDES))
    -O := /Fo
    -OUT := /OUT:
    OBJ_EXT := obj
	LIB_EXT := lib
    -C := /c
    CXXFLAGS := /std:c++23 -DENGINE $(INCLUDES) /utf-8
# LDFLAGS := /L"bin/lib/"
	LDFLAGS := 
    OPT_DEBUG := /O0
    OPT_RELEASE := /O2
	SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/$(lib).lib)
endif


# Folders
SRC_MAIN := .
SRC_DIR := source
SRC_CORE      := $(SRC_DIR)/Core
SRC_BASE      := $(SRC_DIR)/Base
SRC_ENGINE    := $(SRC_DIR)/Engine
SRC_RESOURCES := $(SRC_DIR)/Resources
SRC_SHADERS   := $(SRC_DIR)/Shaders
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

# vulkan_backend
SRC_VULKAN_BACKEND := $(DEPS_PATH)/vulkan_backend/src
OBJS_VULKAN_BACKEND := $(patsubst $(SRC_VULKAN_BACKEND)/%.cpp, $(PLATFORM_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_VULKAN_BACKEND)/*.cpp))

# $(info $(SRC_VULKAN_BACKEND))
# $(info $(OBJS_VULKAN_BACKEND))
# $(patsubst %.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(SRC_CPP_RAW)) \


all: debug

.PHONY: debug
debug: $(DIRS_CREATED_FILE)
ifeq ($(CXX),cl)
debug: CXXFLAGS += /Zi /Od /DNRAY_DEBUG /EHsc /Fd$(PLATFORM_BUILD_DIR)/$(TARGET).pdb /FS
debug: LDFLAGS  += vulkan-1.lib
	
debug: SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/$(lib)d.lib)
else
debug: CXXFLAGS += -g -ggdb $(OPT_DEBUG) -DNRAY_DEBUG 
debug: LDFLAGS  += -g -ggdb $(OPT_DEBUG)
endif
debug: build_target
debug: modules_clangd
.PHONY: release
release: $(DIRS_CREATED_FILE)
release: CXXFLAGS += $(OPT_RELEASE)
release: LDFLAGS  += $(OPT_RELEASE)
release: build_target

# build_target: dirs .WAIT
# ifeq ($(USE_HEADER_UNITS), 1)
# build_target: build_headers .WAIT
# endif
# ifeq ($(USE_MODULES), 1)
# build_target: build_modules .WAIT
# endif
# build_target: $(TARGET)

build_target: $(DIRS_CREATED_FILE)

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


$(DIRS_CREATED_FILE):
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
	@cmd /c if not exist $(_WINBDIR)\vulkan_backend mkdir $(_WINBDIR)\vulkan_backend
	@cmd /c if not exist $(_WINBDIR)\imgui mkdir $(_WINBDIR)\imgui
# @cmd /c if not exist $(_WINBDIR)\stb mkdir $(_WINBDIR)\stb
	@cmd /c if not exist $(_WINBDIR)\glfw mkdir $(_WINBDIR)\glfw
	@cmd /c if not exist $(_WINBDIR)\fastgltf mkdir $(_WINBDIR)\fastgltf
	@cmd /c if not exist $(DEPS_PATH)\fastgltf\deps\simdjson mkdir $(DEPS_PATH)\fastgltf\deps\simdjson
	@cmd /c if not exist $(_WINBDIR)\simdjson mkdir $(_WINBDIR)\simdjson
	@cmd /c if not exist $(_WINBDIR)\spdlog mkdir $(_WINBDIR)\spdlog
	@cmd /c if not exist $(_WINBDIR)\fmt mkdir $(_WINBDIR)\fmt

	@cmd /c if not exist $(_WOBDIR)\source mkdir $(_WOBDIR)\source
	@xcopy /t /e source $(_WOBDIR)\source

	@echo 0 > $(DIRS_CREATED_FILE)
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
	$(PLATFORM_BUILD_DIR)/vulkan_backend \
	$(PLATFORM_BUILD_DIR)/imgui \
	$(PLATFORM_BUILD_DIR)/glfw \
	$(PLATFORM_BUILD_DIR)/fastgltf \
	$(DEPS_PATH)/fastgltf/deps/simdjson \
	$(PLATFORM_BUILD_DIR)/simdjson \
	$(PLATFORM_BUILD_DIR)/spdlog \
	$(PLATFORM_BUILD_DIR)/fmt \

	@find $(SRC_DIR) -type d -exec mkdir -p -- $(OBJS_BUILD_DIR)/{} \;

	@echo 0 > $(DIRS_CREATED_FILE)

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
	vulkan_backend \
	fastgltf \

# $(info $(EXTERNAL_MODULES))
EXTERNAL_MODULE_TARGETS := $(foreach module,$(EXTERNAL_MODULES),$(MODULES_BUILD_DIR)/$(module).pcm) 
# $(info $(EXTERNAL_MODULE_TARGETS))
MAIN_MODULE_TARGET := NRay
CPP_MODULE_TARGETS := $(foreach module,$(CPP_MODULES),$(MODULES_BUILD_DIR)/$(module).pcm)

CPP_MODULE_OBJS := $(patsubst %.pcm, $(MODULES_OBJS_DIR)/%.o, $(notdir $(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS)))
# $(info $(CPP_MODULE_OBJS))

# $(CPP_MODULE_TARGETS) : get_cpp_module_dependencies
# .PHONY: get_cpp_module_dependencies
get_cpp_module_dependencies: $(CPP_MODULE_DEPENDENCIES_FILE)

$(TARGET): $(SUBMODULE_LIBS)

_MBD := $(MODULES_BUILD_DIR)
_OBD := $(OBJS_BUILD_DIR)
ifeq ($(USE_MODULES), 1)
-include $(CPP_MODULE_DEPENDENCIES_FILE)
endif
# $(TARGET): $(OBJS) $(OBJS_IMGUI) $(OBJS_STB)

# $(info $(LDFLAGS))

$(TARGET): \
	$(OBJS) \
	$(OBJS_VULKAN_BACKEND) \
	$(OBJS_IMGUI) \
	$(OBJS_GLFW) \
	$(OBJS_FASTGLTF) \
	$(OBJS_SIMDJSON) \

ifeq ($(SPDLOG_COMPILED_LIB), 1)
$(TARGET): $(OBJS_SPDLOG)
endif

ifeq ($(USE_MODULES), 1)
$(TARGET): $(CPP_MODULE_OBJS)
endif
	
	
# $(OBJS_STB) \
# $(OBJS_FMT)
# @echo "Linking $(notdir $^)"
	@echo "Linking"
ifeq ($(CXX),cl)
	@link $(-OUT)$(TARGET_DIR)/$@ $^ $(LDFLAGS) 
else
	@$(CXX) $(-OUT)$(TARGET_DIR)/$@ $^ $(LDFLAGS)
endif
	@echo "=== Done ==="

#compile
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_MAIN)/%.cpp # ./
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_CORE)/%.cpp # core/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_BASE)/%.cpp # base/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_TEST)/%.cpp # test/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_ENGINE)/%.cpp # engine/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_RESOURCES)/%.cpp # resources/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $< 

# Tests
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_NEURALSFD)/%.cpp # tests/NeuralSdf
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_IMAGEOPT)/%.cpp # tests/ImageOptimization
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_SLANG)/%.cpp # tests/SlangTest
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_FEATURE)/%.cpp # tests/FeatureTest
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_RADIENCE)/%.cpp # tests/RadienceField
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_HELLOTRIANGLE)/%.cpp # tests/HelloTriangle
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_WINDOW)/%.cpp # tests/Window
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<

# ============================================ Headers ===================================================

HEADER_FLAGS := $(filter-out -fmodule-file-deps,$(CXXFLAGS)) \
	-Wno-pragma-system-header-outside-header \
	-Wno-user-defined-literals \
	-Wno-invalid-constexpr \
	-Wno-unknown-warning-option

$(HEADERS_BUILD_DIR)/%.pcm:
	@echo "Compiling <$(patsubst %.pcm,%,$(notdir $@))>"
	@$(CXX) $(HEADER_FLAGS) --precompile -xc++-system-header $(patsubst %.pcm,%,$(notdir $@)) -o $@


# ============================================ std module ===================================================

STD_MODULE_TARGET := $(MODULES_BUILD_DIR)/std.pcm
STD_COMPAT_TARGET := $(MODULES_BUILD_DIR)/std.compat.pcm

$(STD_COMPAT_TARGET): $(STD_MODULE_TARGET)

ifeq ($(STD_MODULE_AVAILABLE), 1)

STD_MODULE_PATH_FILE := $(BUILD_DIR)/std_module_path.mk

# get std.cppm module path
$(STD_MODULE_PATH_FILE): scripts/get_std_module_path.py
	@echo "Generating $(STD_MODULE_PATH_FILE)"
	@PYTHON $< > $@
	
-include $(STD_MODULE_PATH_FILE)

# compile std and std.compat
# $(info $(STD_MODULE_SRC))
$(MODULES_BUILD_DIR)/%.pcm: $(STD_MODULE_SRC)/%.cppm
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(CXXFLAGS) --precompile -c -x c++-module "$<" -o $@ \
	-Wno-reserved-module-identifier \
	-Wno-unused-command-line-argument

else

_STD_MODULE_FLAGS := $(filter-out -fmodule-file-deps,$(CXXFLAGS))

ifeq (USE_HEADER_UNITS, 1)

$(STD_MODULE_TARGET): $(CPP_SYSTEM_HEADER_TARGETS)

_STD_MODULE_FLAGS += \
	$(foreach header,$(CPP_SYSTEM_HEADERS),-fmodule-file=$(HEADERS_BUILD_DIR)/$(header).pcm) \
	-Wno-experimental-header-units
	
endif # USE_HEADER_UNITS

$(MODULES_BUILD_DIR)/%.pcm: $(SRC_DIR)/Base/%.cppm
	@echo "Compiling module std"
	@$(CXX) $(_STD_MODULE_FLAGS) --precompile -c -x c++-module $< -o $@

endif # STD_MODULE_AVAILABLE




# ============================================ Modules ===================================================

ifeq ($(findstring modules,$(MAKECMDGOALS)),modules)
-include $(CPP_MODULE_DEPENDENCIES_FILE)
-include $(STD_MODULE_PATH_FILE)
endif

modules: CXXFLAGS += -fprebuilt-module-path=$(MODULES_BUILD_DIR) -D USE_MODULES -fmodule-file-deps
modules: $(CPP_MODULE_DEPENDENCIES_FILE) $(EXTERNAL_MODULE_TARGETS) $(CPP_MODULE_TARGETS) 

# CLANGD_MODULE_TARGETS := $(subst $(MODULES_BUILD_DIR),$(MODULES_BUILD_DIR_CLANGD),$(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS) $(STD_MODULE_TARGET))
CLANGD_MODULE_TARGETS := $(subst $(MODULES_BUILD_DIR),$(MODULES_BUILD_DIR_CLANGD),$(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS))

ifneq ($(STD_MODULE_SRC), )
CLANGD_MODULE_TARGETS += $(subst $(MODULES_BUILD_DIR),$(MODULES_BUILD_DIR_CLANGD),$(MODULES_BUILD_DIR)/std.pcm $(MODULES_BUILD_DIR)/std.compat.pcm)
endif

# $(info $(CPP_MODULE_TARGETS) $(EXTERNAL_MODULE_TARGETS) $(STD_MODULE_TARGET))
# $(info $(CLANGD_MODULE_TARGETS))

modules_clangd: $(DIRS_CREATED_FILE)
# modules_clangd: modules
modules_clangd: $(CLANGD_MODULE_TARGETS)

$(MODULES_BUILD_DIR_CLANGD)/%.pcm: $(MODULES_BUILD_DIR)/%.pcm
	@rm -f $@
	@cp $< $@
# ln -sf $< $@
# @echo "mklink $(subst /,\,$@) ..\modules\$(notdir $<)"

$(_MBD)/%.pcm: $(SRC_DIR)/Core/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(SRC_DIR)/Base/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(SRC_DIR)/Engine/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(SRC_DIR)/Resources/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(SRC_DIR)/Shaders/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(SRC_FEATURE)/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@


# build_external_modules: $(EXTERNAL_MODULE_TARGETS)

$(_MBD)/%.pcm: $(EXTERNAL_MODULES_DIR)/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(EXTERNAL_MODULES_DIR)/imgui/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

$(_MBD)/%.pcm: $(EXTERNAL_MODULES_DIR)/stb/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

# #entt
# $(_MBD)/%.pcm: deps\entt\src\entt\entity\registry.hpp
# @rm -f $@
# 	@echo "Compiling module $(notdir $<)"
# 	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

# fastgltf
$(_MBD)/%.pcm: deps/fastgltf/src/%.ixx
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@

# vulkan_backend
$(_MBD)/%.pcm: deps/vulkan_backend/src/%.cppm
# @rm -f $@
	@echo "Compiling module $(notdir $<)"
	@$(CXX) $(filter-out -fmodule-file-deps,$(CXXFLAGS)) --precompile -c -x c++-module $< -o $@


# ========================== Module Objs =============================

$(MODULES_OBJS_DIR)/%.o: $(MODULES_BUILD_DIR)/%.pcm
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -Wno-unused-command-line-argument

# ====================================================================

# ============================================ Libraries ===================================================

#ImGui
$(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT): $(SRC_IMGUI)/%.cpp # imgui/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT): $(IMGUI_BACKENDS_DIR)/%.cpp # imgui/backends/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<

#Stb
$(PLATFORM_BUILD_DIR)/stb/%.$(OBJ_EXT): $(SRC_STB)/%.c # stb/
	@echo "Compiling $(notdir $<)"
	@$(CXX) -x c $(filter-out $(CPP_STD),$(CXXFLAGS)) $(-O)$@ $(-C) $<

# GLFW
ifeq ($(OS),Windows_NT)
GLFW_PLATFORM := _GLFW_WIN32
else
GLFW_PLATFORM := _GLFW_X11
endif

ifeq ($(findstring clang,$(CXX)),clang)
_CLANG_LIBFLAGS := -target x86_64-w64-mingw32
endif

$(PLATFORM_BUILD_DIR)/glfw/%.$(OBJ_EXT): $(SRC_GLFW)/%.c # glfw/
	@echo "Compiling $(notdir $<)"
	@$(CXX) -x c $(filter-out $(CPP_STD),$(CXXFLAGS)) -c $< $(-O)$@ -D$(GLFW_PLATFORM)

ifeq ($(findstring clang,$(CXX)),clang)
_CLANG_FASTGLTF := -femulated-tls
endif


# Fastgltf
$(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT): $(SRC_FASTGLTF)/%.cpp $(SRC_SIMDJSON)/simdjson.h
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fastgltf/include/ -I$(DEPS_PATH)/fastgltf/deps/simdjson $(_CLANG_FASTGLTF)
#  -DFASTGLTF_USE_STD_MODULE=1

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
	@$(CXX) -MMD -MP $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fastgltf/deps/simdjson -O3 $(_CLANG_LIBFLAGS)


# spdlog
$(PLATFORM_BUILD_DIR)/spdlog/%.$(OBJ_EXT): $(SRC_SPDLOG)/%.cpp # spdlog/
	@echo "Compiling $(notdir $<)"
	@$(CXX) -MMD -MP $(-O)$@ $(-C) $< -O3 -DSPDLOG_COMPILED_LIB -DSPDLOG_NO_EXCEPTIONS -I$(DEPS_PATH)/spdlog/include $(_CLANG_LIBFLAGS)
# -I$(DEPS_PATH)/fmt/include -DSPDLOG_FMT_EXTERNAL -DSPDLOG_FMT_EXTERNAL_HO

#fmt
$(PLATFORM_BUILD_DIR)/fmt/%.$(OBJ_EXT): $(SRC_FMT)/%.cc # fmt/
	@echo "Compiling $(notdir $<)"
	@$(CXX) -MMD -MP $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fmt/include -fmodules-ts $(_CLANG_LIBFLAGS)

# vulkan_backend
$(PLATFORM_BUILD_DIR)/%.$(OBJ_EXT): $(SRC_VULKAN_BACKEND)/%.cpp # vulkan_backend/
	@echo "Compiling $(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(-O)$@ $(-C) $<


DEPFILES =  $(OBJS:.$(OBJ_EXT)=.d) \
			$(OBJS_IMGUI:.$(OBJ_EXT)=.d) \
			$(OBJS_GLFW:.$(OBJ_EXT)=.d) \
			$(CPP_MODULE_TARGETS:.pcm=.d) \
			$(EXTERNAL_MODULE_TARGETS:.pcm=.d) \

-include $(wildcard $(DEPFILES))

# Run
run:
	@./$(TARGET_DIR)/$(TARGET)

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(TARGET_DIR)/$(TARGET) 

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

rm:
	$(RM) $(wildcard *.bmp)
