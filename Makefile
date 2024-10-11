CC := g++
TARGET := app

OPT_LEVEL := 0

INCLUDES := -Iinclude -IDeps -ICore -IBase -IShaders 
CXXFLAGS := -MMD -MP $(INCLUDES) -O$(OPT_LEVEL) -DENGINE
# -Wno-narrowing

LDFLAGS := -lspdlog -O$(OPT_LEVEL)

ifeq ($(OS),Windows_NT)
	LDFLAGS := $(LDFLAGS) -lvulkan-1
	OBJ_DIR := build
	BUILD_DIR := build
	MKDIR := cmd /c if not exist $(BUILD_DIR) mkdir
	CLEAN_BUILD := cmd /c del /Q $(BUILD_DIR)
	CLEAN_OBJ := cmd /c del /Q $(OBJ_DIR)
else
	LDFLAGS := $(LDFLAGS) -lvulkan -lfmt
	OBJ_DIR := build-linux
	BUILD_DIR := build-linux
	MKDIR:= mkdir -p
	CLEAN_BUILD := rm -f $(BUILD_DIR)/*
	CLEAN_OBJ := rm -f $(OBJ_DIR)/*
endif


# Folders
SRC_MAIN := .
SRC_CORE := core
SRC_BASE := base

OBJS := \
	$(patsubst $(SRC_MAIN)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_MAIN)/*.cpp)) \
	$(patsubst $(SRC_CORE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_CORE)/*.cpp)) \
	$(patsubst $(SRC_BASE)/%.cpp, $(OBJ_DIR)/%.o, $(wildcard $(SRC_BASE)/*.cpp))

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

run:
	@./$(BUILD_DIR)/$(TARGET)

# VLGRND_OUTPUT := --log-file="mem"
VLGRND_OUTPUT := 
VLGRND_FULL := --leak-check=full --show-leak-kinds=all
VLGRND_FULL :=

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(BUILD_DIR)/$(TARGET) 
# @valgrind ./$(BUILD_DIR)/$(TARGET)

clean:
	@$(CLEAN_BUILD)
	@$(CLEAN_OBJ)
	@echo "=== Cleaned ==="

rm:
	cmd /c del $(wildcard *.bmp)

# $(RM) $(OBJ_DIR)/