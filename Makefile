CC := g++
# CC := ccache g++
# CC := clang++
# CC := ccache clang++
# CC := cl
TARGET := nRay

COMPILE_IMGUI := 1
STATIC_LINK := 0

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
DEPS_PATH := deps


SUBMODULE_LIBS := fastgltf spdlog


INCLUDES := -Isource/Core \
			-Isource/Base \
			-Isource/Shaders \
			-Isource/Engine \
			-Isource/Resources \
			-Ideps \
			-Ideps/fastgltf/include \
			-Ideps/spdlog/include \
			-Ideps/fmt/include \
			-Ideps/imgui \
			-Ideps/imgui/backends \
			-Ideps/entt/src \
			-Ideps/stb

CXXFLAGS := -MMD -MP $(INCLUDES) -DENGINE
LDFLAGS :=  
LIBS := 

ARFLAGS = rcs


ifeq ($(STATIC_LINK), 1)
	LDFLAGS += -static -static-libgcc -static-libstdc++
# Not needed
	CXXFLAGS += -static -static-libgcc -static-libstdc++
endif

BUILD_DIR := build
BIN_DIR := bin

ifeq ($(OS),Windows_NT)
	LDFLAGS += -fuse-ld=lld -Lbin/lib
	LIBS := $(LIBS)  glfw3 Gdi32 vulkan-1
	BUILD_PLATFORM_DIR := $(BUILD_DIR)\win
	CMAKE_BUILD_DIR := build
	TARGET_DIR := .
	CMD := cmd /c
	RM := $(CMD) del /Q
else
	CC := g++
	LDFLAGS += -lpthread -Lbin/lib-linux -lstdc++
	LIBS := $(LIBS) vulkan glfw GL m
	BUILD_PLATFORM_DIR := $(BUILD_DIR)/linux
	CMAKE_BUILD_DIR := build-linux
	TARGET_DIR := $(BUILD_DIR)/linux
	RM := rm -rf
endif

LIB_PATH := $(BUILD_PLATFORM_DIR)/lib
SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS),$(LIB_PATH)/lib$(lib).a)


# Folders
SRC_MAIN := .
SRC_CORE := source/Core
SRC_BASE := source/Base
SRC_TEST := tests
SRC_ENGINE := source/Engine
SRC_RESOURCES := source/Resources


SRC_IMGUI := deps/imgui
IMGUI_BACKENDS_DIR := deps/imgui/backends
SRC_IMGUI_BACKENDS := imgui_impl_vulkan.cpp imgui_impl_glfw.cpp
SRC_IMGUI_BACKENDS := $(foreach file,$(SRC_IMGUI_BACKENDS),$(IMGUI_BACKENDS_DIR)/$(file))

SRC_STB := deps/stb_src

LDFLAGS += $(foreach lib,$(LIBS),-l$(lib))

# # Tests
# TST_NEURALSFD := tests/NeuralSdf
# TST_IMAGEOPT := tests/ImageOptimization
# TST_SLANG := tests/SlangTest
TST_FEATURE := tests/FeatureTest
# TST_RADIENCE := tests/RadienceField
# TST_HELLOTRIANGLE := tests/HelloTriangle
# TST_WINDOW := tests/Window

# TST_ALL := $(TST_NEURALSFD) $(TST_IMAGEOPT) $(TST_SLANG) $(TST_FEATURE) $(TST_RADIENCE)
tst_dirs := $(wildcard tests/*)
files := $(foreach dir,$(tst_dirs),$(wildcard $(dir)/*.cpp))
TST_CPP := $(filter %.cpp, $(files) $(tst_dirs))
TST_CPP_RAW := $(notdir $(TST_CPP))
# TST_CPP_RAW := $(notdir $(files))
# $(info $(files))
# $(info $(TST_CPP))
ifeq ($(findstring clang,$(CC)),clang)
	ifeq ($(OS),Windows_NT)
	CXXFLAGS += -target x86_64-w64-mingw32
	LDFLAGS += -target x86_64-w64-mingw32
	endif
	LDFLAGS += -pthread
endif

-O := -o
-OUT := -o
OBJ_EXT := o
-C := -c
CXXFLAGS += -std=c++20
OPT_DEBUG := -O0
OPT_RELEASE := -O2

ifeq ($(findstring cl,$(CC)),cl)
ifneq ($(findstring clang,$(CC)),clang)
-O := /Fo
-OUT := /OUT:
OBJ_EXT := obj
-C := /c
CXXFLAGS += /std:c++20
OPT_DEBUG := /O0
OPT_RELEASE := /O2
endif
endif

OBJS := \
	$(patsubst $(SRC_MAIN)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_MAIN)/*.cpp)) \
	$(patsubst $(SRC_CORE)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_CORE)/*.cpp)) \
	$(patsubst $(SRC_BASE)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_BASE)/*.cpp)) \
	$(patsubst $(SRC_ENGINE)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_ENGINE)/*.cpp)) \
	$(patsubst $(SRC_RESOURCES)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_RESOURCES)/*.cpp)) \
	$(patsubst $(SRC_TEST)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_TEST)/*.cpp)) \
	$(patsubst $(TST_FEATURE)/%.cpp, $(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT), $(wildcard $(TST_FEATURE)/*.cpp)) \
	

IMGUI_OBJS := 	$(patsubst $(SRC_IMGUI)/%.cpp, $(BUILD_PLATFORM_DIR)/imgui/%.$(OBJ_EXT), $(wildcard $(SRC_IMGUI)/*.cpp)) \
				$(patsubst $(IMGUI_BACKENDS_DIR)/%.cpp, $(BUILD_PLATFORM_DIR)/imgui/%.$(OBJ_EXT), $(SRC_IMGUI_BACKENDS)) 
			

STB_OBJS := $(patsubst $(SRC_STB)/%.c, $(BUILD_PLATFORM_DIR)/stb/%.$(OBJ_EXT), $(wildcard $(SRC_STB)/*.c))

# $(info $(IMGUI_OBJS))
# $(info $(SRC_IMGUI_BACKENDS))

# $(patsubst %.cpp, $(BUILD_PLATFORM_DIR)/%.o, $(TST_CPP_RAW)) \

# $(patsubst $(TST_NEURALSFD)/%.cpp, $(BUILD_PLATFORM_DIR)/%.o, $(wildcard $(TST_NEURALSFD)/*.cpp)) \
# $(patsubst $(TST_SLANG)/%.cpp, $(BUILD_PLATFORM_DIR)/%.o, $(wildcard $(TST_SLANG)/*.cpp)) \
# $(patsubst $(TST_IMAGEOPT)/%.cpp, $(BUILD_PLATFORM_DIR)/%.o, $(wildcard $(TST_IMAGEOPT)/*.cpp)) \
# $(patsubst $(TST_RADIENCE)/%.cpp, $(BUILD_PLATFORM_DIR)/%.o, $(wildcard $(TST_RADIENCE)/*.cpp)) \


DEPFILES = $(OBJS:.o=.d) $(IMGUI_OBJS:.o=.d) $(STB_OBJS:.o=.d)

all: release

.PHONY: debug
debug: CXXFLAGS += -g -ggdb $(OPT_DEBUG) -DNRAY_DEBUG
debug: LDFLAGS  += -g -ggdb $(OPT_DEBUG)
debug: build_target

.PHONY: release
release: CXXFLAGS += $(OPT_RELEASE)
release: LDFLAGS  += $(OPT_RELEASE)
release: build_target

build_target: create_dirs $(TARGET)

create_dirs:
ifeq ($(OS),Windows_NT)
	@cmd /c if not exist $(TARGET_DIR) mkdir $(TARGET_DIR)
	@cmd /c if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@cmd /c if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cmd /c if not exist $(BUILD_PLATFORM_DIR) mkdir $(BUILD_PLATFORM_DIR)
	@cmd /c if not exist $(BUILD_PLATFORM_DIR)\imgui mkdir $(BUILD_PLATFORM_DIR)\imgui
	@cmd /c if not exist $(BUILD_PLATFORM_DIR)\stb mkdir $(BUILD_PLATFORM_DIR)\stb
else
	@mkdir -p $(TARGET_DIR) $(BIN_DIR) $(BUILD_DIR) $(BUILD_PLATFORM_DIR) $(BUILD_PLATFORM_DIR)/imgui $(BUILD_PLATFORM_DIR)/stb
endif


$(TARGET): $(SUBMODULE_LIBS)

$(TARGET): $(OBJS) $(IMGUI_OBJS) $(STB_OBJS)
	@echo "Linking $(notdir $^)"
	@$(CC) $(-OUT)$(TARGET_DIR)/$@ $^ $(LDFLAGS)
	@echo "=== Done ==="

#compile
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(SRC_MAIN)/%.cpp # ./
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(SRC_CORE)/%.cpp # core/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(SRC_BASE)/%.cpp # base/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(SRC_TEST)/%.cpp # test/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(SRC_ENGINE)/%.cpp # engine/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(SRC_RESOURCES)/%.cpp # resources/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

# Tests
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_NEURALSFD)/%.cpp # tests/NeuralSdf
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_IMAGEOPT)/%.cpp # tests/ImageOptimization
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_SLANG)/%.cpp # tests/SlangTest
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_FEATURE)/%.cpp # tests/FeatureTest
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_RADIENCE)/%.cpp # tests/RadienceField
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_HELLOTRIANGLE)/%.cpp # tests/HelloTriangle
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/%.$(OBJ_EXT): $(TST_WINDOW)/%.cpp # tests/Window
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

#ImGui
$(BUILD_PLATFORM_DIR)/imgui/%.$(OBJ_EXT): $(SRC_IMGUI)/%.cpp # imgui/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(BUILD_PLATFORM_DIR)/imgui/%.$(OBJ_EXT): $(IMGUI_BACKENDS_DIR)/%.cpp # imgui/backends/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

#Stb
$(BUILD_PLATFORM_DIR)/stb/%.$(OBJ_EXT): $(SRC_STB)/%.c # stb/
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

-include $(wildcard $(DEPFILES))

# Run
run:
	@./$(TARGET)


# VLGRND_OUTPUT := --log-file="bin/mem"
# VLGRND_FULL := --leak-check=full 
# VLGRND_FULL += --show-leak-kinds=all

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(TARGET) 
# @valgrind ./$(TARGET_DIR)/$(TARGET)

clean:
	@rm -f $(OBJS) $(DEPFILES)
	@echo "=== Cleaned ==="

cleanlib:
	@rm -f $(LIB_PATH)/libfastgltf.a
	@rm -f $(LIB_PATH)/libspdlog.a
	@rm -f $(LIB_PATH)/libfmt.a
	@rm -f $(LIB_PATH)/libimgui.a
	@echo "=== Cleaned ==="

cleanimgui:
	@rm -f $(LIB_PATH)/libimgui.a
	@rm -f $(IMGUI_OBJS) $(IMGUI_OBJS:.o=.d)
	@echo "=== Cleaned ==="

cleanstb:
	@rm -f $(STB_OBJS) $(STB_OBJS:.o=.d)
	@echo "=== Cleaned ==="

rm:
	$(RM) $(wildcard *.bmp)


# ===== Libraries ======



CMAKE_FLAGS :=  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(abspath $(LIB_PATH)) \
				-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$(abspath $(LIB_PATH))

ifeq ($(OS),Windows_NT)
CMAKE_CXX_FLAGS := -target x86_64-w64-mingw32
CMAKE_FLAGS +=  -G "MinGW Makefiles" \
				-DCMAKE_EXE_LINKER_FLAGS_INIT="-fuse-ld=lld" \
				-DCMAKE_MODULE_LINKER_FLAGS_INIT="-fuse-ld=lld" \
				-DCMAKE_SHARED_LINKER_FLAGS_INIT="-fuse-ld=lld" \
				-DCMAKE_LINKER_FLAGS_INIT="-fuse-ld=lld" \
				-DCMAKE_C_COMPILER_TARGET="x86_64-windows-gnu"


else

endif

ifeq ($(OS),Windows_NT)
$(LIB_PATH)/libfastgltf.a:
# cmake $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/build -G "MinGW Makefiles" ${CMAKE_FLAGS} -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ 
	@cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="-target x86_64-w64-mingw32 -femulated-tls" -DFASTGLTF_COMPILE_AS_CPP20=ON -DFASTGLTF_ENABLE_EXAMPLES=YES
	@cmake --build $(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR)
else
$(LIB_PATH)/libfastgltf.a:
	@cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DFASTGLTF_COMPILE_AS_CPP20=ON
	@cmake --build $(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR)
endif

$(LIB_PATH)/libfmt.a:
	@cmake --fresh $(DEPS_PATH)/fmt -B$(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" -DFMT_TEST=OFF
	@cmake --build $(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR)

$(LIB_PATH)/libspdlog.a:
	@cmake --fresh $(DEPS_PATH)/spdlog -B$(DEPS_PATH)/spdlog/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="$(CMAKE_CXX_FLAGS)" -DSPDLOG_BUILD_TESTS=OFF 
# -DSPDLOG_FMT_EXTERNAL=ON -Dfmt_DIR="$(DEPS_PATH)/fmt/$(CMAKE_BUILD_DIR)"
	@cmake --build $(DEPS_PATH)/spdlog/$(CMAKE_BUILD_DIR)

# $(LIB_PATH)/libimgui.a: $(IMGUI_OBJS)
# 	$(AR) $(ARFLAGS) $@ $^

# $(LIB_PATH)/libstb.a: $(STB_OBJS)
# 	$(AR) $(ARFLAGS) $@ $^