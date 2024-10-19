# CC := g++
# CC := ccache g++
CC := clang++
# CC := ccache clang++
TARGET := app

OPT_LEVEL := 0

INCLUDES := -Isource/Core -Isource/Base -Isource/Shaders 
CXXFLAGS := -MMD -MP $(INCLUDES) -O$(OPT_LEVEL) -DENGINE -std=c++17
LDFLAGS := -O$(OPT_LEVEL)
LIBS := spdlog 

ifeq ($(OS),Windows_NT)
	LIBS := $(LIBS) vulkan-1 glfw3 gdi32
	OBJ_DIR := build
	BUILD_DIR := build
	MKDIR := cmd /c if not exist $(BUILD_DIR) mkdir
	RM := cmd /c del /Q
	CLEAN_BUILD := $(RM) $(BUILD_DIR)
	CLEAN_OBJ := $(RM) $(OBJ_DIR)
else
	CC := g++
	LIBS := $(LIBS) vulkan fmt glfw GL m
	OBJ_DIR := build-linux
	BUILD_DIR := build-linux
	MKDIR:= mkdir -p
	RM := rm -rf
	CLEAN_BUILD := $(RM) $(BUILD_DIR)/*
	CLEAN_OBJ := $(RM) $(OBJ_DIR)/*
endif

LDFLAGS += $(foreach lib,$(LIBS),-l$(lib))


ifeq ($(findstring clang,$(CC)),clang)
	CXXFLAGS += -target x86_64-w64-mingw32
	LDFLAGS += -target x86_64-w64-mingw32 -fuse-ld=lld -pthread
endif

# Folders
SRC_MAIN := .
SRC_CORE := source/core
SRC_BASE := source/base
SRC_TEST := tests
# Tests
TST_NEURALSFD := tests/NeuralSdf
TST_IMAGEOPT := tests/ImageOptimization
TST_SLANG := tests/SlangTest
TST_FEATURE := tests/FeatureTest
TST_RADIENCE := tests/RadienceField
TST_HELLOTRIANGLE := tests/HelloTriangle
# TST_ALL := $(TST_NEURALSFD) $(TST_IMAGEOPT) $(TST_SLANG) $(TST_FEATURE) $(TST_RADIENCE)
tst_dirs := $(wildcard tests/*)
files := $(foreach dir,$(tst_dirs),$(wildcard $(dir)/*.cpp))
# TST_CPP := $(filter %.cpp, $(files) $(tst_dirs))
# TST_CPP_RAW := $(notdir $(TST_CPP))
TST_CPP_RAW := $(notdir $(files))
OBJS := \
	$(patsubst $(SRC_MAIN)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_MAIN)/*.cpp)) \
	$(patsubst $(SRC_CORE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_CORE)/*.cpp)) \
	$(patsubst $(SRC_BASE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_BASE)/*.cpp)) \
	$(patsubst $(SRC_TEST)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_TEST)/*.cpp)) \
	$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TST_CPP_RAW)) \

# $(patsubst $(TST_NEURALSFD)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_NEURALSFD)/*.cpp)) \
# $(patsubst $(TST_SLANG)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_SLANG)/*.cpp)) \
# $(patsubst $(TST_IMAGEOPT)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_IMAGEOPT)/*.cpp)) \
# $(patsubst $(TST_FEATURE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_FEATURE)/*.cpp)) \
# $(patsubst $(TST_RADIENCE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(TST_RADIENCE)/*.cpp)) \

DEPS = $(OBJS:.o=.d)

all: build_target

debug: CXXFLAGS += -g -ggdb -DAPP_DEBUG
debug: build_target

build_target: create_dirs $(BUILD_DIR)/$(TARGET)

create_dirs:
	@$(MKDIR) $(BUILD_DIR)
	@$(MKDIR) $(OBJ_DIR)

#link
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking $(patsubst $(BUILD_DIR)/%,%,$^)"
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "=== Done ==="

#compile
$(OBJ_DIR)/%.o: $(SRC_MAIN)/%.cpp # ./
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(SRC_CORE)/%.cpp # core/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(SRC_BASE)/%.cpp # base/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(SRC_TEST)/%.cpp # test/
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@

# Tests
$(OBJ_DIR)/%.o: $(TST_NEURALSFD)/%.cpp # tests/NeuralSdf
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(TST_IMAGEOPT)/%.cpp # tests/ImageOptimization
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(TST_SLANG)/%.cpp # tests/SlangTest
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(TST_FEATURE)/%.cpp # tests/FeatureTest
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(TST_RADIENCE)/%.cpp # tests/RadienceField
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
$(OBJ_DIR)/%.o: $(TST_HELLOTRIANGLE)/%.cpp # tests/HelloTriangle
	@echo "Compiling $<"
	@$(CC) $(CXXFLAGS) -c $< -o $@
# Run
run:
	@./$(BUILD_DIR)/$(TARGET)

# VLGRND_OUTPUT := --log-file="bin/mem"
VLGRND_OUTPUT := 
# VLGRND_FULL := --leak-check=full --show-leak-kinds=all
VLGRND_FULL :=

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(BUILD_DIR)/$(TARGET) 
# @valgrind ./$(BUILD_DIR)/$(TARGET)

clean:
	@$(CLEAN_BUILD)
	@$(CLEAN_OBJ)
	@echo "=== Cleaned ==="

rm:
	$(RM) $(wildcard *.bmp)


# $(RM) $(OBJ_DIR)/