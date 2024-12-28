CC := g++
# CC := ccache g++
# CC := clang++
# CC := ccache clang++
# CC := cl
TARGET := nRay

COMPILE_IMGUI := 0
STATIC_LINK := 0

INCLUDES := -Isource/Core -Isource/Base -Isource/Shaders
CXXFLAGS := -MMD -MP $(INCLUDES) -DENGINE
LDFLAGS :=
LIBS := spdlog fmt 

ifeq ($(STATIC_LINK), 1)
	LDFLAGS += -static -static-libgcc -static-libstdc++
# Not needed
	CXXFLAGS += -static -static-libgcc -static-libstdc++
endif

BIN_DIR := bin

ifeq ($(OS),Windows_NT)
	LDFLAGS += -fuse-ld=lld
	LIBS := $(LIBS)  glfw3 Gdi32 vulkan-1
	OBJ_DIR := build
	BUILD_DIR := .
	MKDIR_BUILD := cmd /c if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	MKDIR_OBJ := cmd /c if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	MKDIR_BIN := cmd /c if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	RM := cmd /c del /Q
	CLEAN_BUILD := cmd /c if exist $(BUILD_DIR)\$(TARGET).exe $(RM) $(BUILD_DIR)\$(TARGET).exe
	CLEAN_OBJ := $(RM) $(OBJ_DIR)
else
	CC := g++
	LIBS := $(LIBS) vulkan fmt glfw GL m
	OBJ_DIR := build-linux
	BUILD_DIR := build-linux
	MKDIR_BUILD := mkdir -p $(BUILD_DIR)
	MKDIR_OBJ := mkdir -p $(OBJ_DIR)
	MKDIR_BIN := mkdir -p $(BIN_DIR)
	RM := rm -rf
	CLEAN_BUILD := $(RM) $(BUILD_DIR)/$(TARGET)
	CLEAN_OBJ := $(RM) $(OBJ_DIR)/*
	COMPILE_IMGUI := 1
endif



# Folders
SRC_MAIN := .
SRC_CORE := source/core
SRC_BASE := source/base
SRC_TEST := tests

ifeq ($(COMPILE_IMGUI), 1)
SRC_IMGUI := bin/imgui
else
LIBS := imgui $(LIBS)
endif

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
	$(patsubst $(SRC_MAIN)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_MAIN)/*.cpp)) \
	$(patsubst $(SRC_CORE)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_CORE)/*.cpp)) \
	$(patsubst $(SRC_BASE)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_BASE)/*.cpp)) \
	$(patsubst $(SRC_TEST)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_TEST)/*.cpp)) \
	$(patsubst $(TST_FEATURE)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(wildcard $(TST_FEATURE)/*.cpp)) \
	$(patsubst $(SRC_IMGUI)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_IMGUI)/*.cpp)) \
# $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TST_CPP_RAW)) \

# $(patsubst $(TST_NEURALSFD)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_NEURALSFD)/*.cpp)) \
# $(patsubst $(TST_SLANG)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_SLANG)/*.cpp)) \
# $(patsubst $(TST_IMAGEOPT)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_IMAGEOPT)/*.cpp)) \
# $(patsubst $(TST_RADIENCE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_RADIENCE)/*.cpp)) \


DEPS = $(OBJS:.o=.d)

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
	@$(MKDIR_BUILD)
	@$(MKDIR_OBJ)
	@$(MKDIR_BIN)


$(TARGET): $(OBJS)
	@echo "Linking $(patsubst $(BUILD_DIR)/%,%,$^)"
	@$(CC) $(-OUT)$@ $^ $(LDFLAGS)
	@echo "=== Done ==="

#compile
$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_MAIN)/%.cpp # ./
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_CORE)/%.cpp # core/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_BASE)/%.cpp # base/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_TEST)/%.cpp # test/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<

# Tests
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_NEURALSFD)/%.cpp # tests/NeuralSdf
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_IMAGEOPT)/%.cpp # tests/ImageOptimization
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_SLANG)/%.cpp # tests/SlangTest
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_FEATURE)/%.cpp # tests/FeatureTest
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_RADIENCE)/%.cpp # tests/RadienceField
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_HELLOTRIANGLE)/%.cpp # tests/HelloTriangle
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(TST_WINDOW)/%.cpp # tests/Window
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_IMGUI)/%.cpp # imgui/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
# Run
run:
	@./$(TARGET)


# VLGRND_OUTPUT := --log-file="bin/mem2"
# VLGRND_FULL := --leak-check=full 
# VLGRND_FULL += --show-leak-kinds=all

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(TARGET) 
# @valgrind ./$(BUILD_DIR)/$(TARGET)

clean:
	@$(CLEAN_BUILD)
	@$(CLEAN_OBJ)
	@echo "=== Cleaned ==="

rm:
	$(RM) $(wildcard *.bmp)


# $(RM) $(OBJ_DIR)/