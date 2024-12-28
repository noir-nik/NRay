# CC := g++
# CC := ccache g++
CC := clang++
# CC := ccache clang++
# CC := cl
TARGET := nRay

COMPILE_IMGUI := 1
STATIC_LINK := 0

USE_MODULES := 0


mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
DEPS_PATH := deps

CPP_MODULES := Lmath std_lib

# SUBMODULE_LIBS_LIST := fastgltf spdlog glfw3
# SUBMODULE_LIBS_LIST := fmt

OPT_DEBUG := -O0
OPT_RELEASE := -O2

INCLUDES := -Isource/Core \
            -Isource/Base \
            -Isource/Shaders \
            -Isource/Engine \
            -Isource/Resources \
            -Ideps \
            -Ideps/fastgltf/include \
            -Ideps/spdlog/include \
            -Ideps/glfw/include \
            -Ideps/fmt/include \
            -Ideps/imgui \
            -Ideps/imgui/backends \
            -Ideps/entt/src \
            -Ideps/stb
-O := -o
-OUT := -o
OBJ_EXT := o
LIB_EXT := a
-C := -c
CXXFLAGS := -MMD -MP $(INCLUDES) -DENGINE -std=c++20

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
	LDFLAGS += -fuse-ld=lld -Lbin/lib
	LIBS := $(LIBS) Gdi32 vulkan-1
	PLATFORM_BUILD_DIR := $(BUILD_DIR)/win
	CMAKE_BUILD_DIR := build
	TARGET_DIR := .
	CMD := cmd /c
	
	RM := $(CMD) del /Q
else
	CC := g++
	LDFLAGS += -lpthread -Lbin/lib-linux -lstdc++
	LIBS := $(LIBS) vulkan GL m
	PLATFORM_BUILD_DIR := $(BUILD_DIR)/linux
	CMAKE_BUILD_DIR := build-linux
	TARGET_DIR := $(PLATFORM_BUILD_DIR)
	RM := rm -rf
	USE_MODULES = 0
endif

LDFLAGS += $(foreach lib,$(LIBS),-l$(lib))
LIB_PATH := $(PLATFORM_BUILD_DIR)/lib
SUBMODULE_LIBS := $(foreach lib,$(SUBMODULE_LIBS_LIST),$(LIB_PATH)/lib$(lib).a)
OBJS_BUILD_DIR := $(PLATFORM_BUILD_DIR)/objs
MODULES_BUILD_DIR := $(PLATFORM_BUILD_DIR)/modules

ifeq ($(USE_MODULES), 1)
CXXFLAGS += -fprebuilt-module-path=$(MODULES_BUILD_DIR) -D USE_MODULES
LDFLAGS += $(MODULES_BUILD_DIR)/*.pcm
CPP_MODULE_TARGETS := $(foreach module,$(CPP_MODULES),$(MODULES_BUILD_DIR)/$(module).pcm)
endif

# $(info $(CPP_MODULE_TARGETS))

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
SRC_TEST := tests
SRC_ENGINE := source/Engine
SRC_RESOURCES := source/Resources



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



OBJS := \
	$(patsubst $(SRC_MAIN)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_MAIN)/*.cpp)) \
	$(patsubst $(SRC_CORE)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_CORE)/*.cpp)) \
	$(patsubst $(SRC_BASE)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_BASE)/*.cpp)) \
	$(patsubst $(SRC_ENGINE)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_ENGINE)/*.cpp)) \
	$(patsubst $(SRC_RESOURCES)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_RESOURCES)/*.cpp)) \
	$(patsubst $(SRC_TEST)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(SRC_TEST)/*.cpp)) \
	$(patsubst $(TST_FEATURE)/%.cpp, $(OBJS_BUILD_DIR)/%.$(OBJ_EXT), $(wildcard $(TST_FEATURE)/*.cpp)) \

# Libs
# ImGui
SRC_IMGUI := $(DEPS_PATH)/imgui
IMGUI_BACKENDS_DIR := $(DEPS_PATH)/imgui/backends
SRC_IMGUI_BACKENDS := imgui_impl_vulkan.cpp imgui_impl_glfw.cpp
SRC_IMGUI_BACKENDS := $(foreach file,$(SRC_IMGUI_BACKENDS),$(IMGUI_BACKENDS_DIR)/$(file))
OBJS_IMGUI := 	$(patsubst $(SRC_IMGUI)/%.cpp, $(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT), $(wildcard $(SRC_IMGUI)/*.cpp)) \
				$(patsubst $(IMGUI_BACKENDS_DIR)/%.cpp, $(PLATFORM_BUILD_DIR)/imgui/%.$(OBJ_EXT), $(SRC_IMGUI_BACKENDS)) 

# Stb	
SRC_STB := $(DEPS_PATH)/stb_src
OBJS_STB := $(patsubst $(SRC_STB)/%.c, $(PLATFORM_BUILD_DIR)/stb/%.$(OBJ_EXT), $(wildcard $(SRC_STB)/*.c))

# GLFW
SRC_GLFW := $(DEPS_PATH)/glfw/src
OBJS_GLFW  := $(patsubst $(DEPS_PATH)/glfw/src/%.c, $(PLATFORM_BUILD_DIR)/glfw/%.$(OBJ_EXT), $(wildcard $(SRC_GLFW)/*.c))

# fastgltf
SRC_FASTGLTF := $(DEPS_PATH)/fastgltf/src
SRC_SIMDJSON :=  $(DEPS_PATH)/fastgltf/deps/simdjson
OBJS_FASTGLTF := $(patsubst $(SRC_FASTGLTF)/%.cpp, $(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT), $(wildcard $(SRC_FASTGLTF)/*.cpp)) \
				$(patsubst $(SRC_SIMDJSON)/%.cpp, $(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT), $(wildcard $(SRC_SIMDJSON)/*.cpp))

# spdlog
SRC_SPDLOG := $(DEPS_PATH)/spdlog/src
OBJS_SPDLOG := $(patsubst $(SRC_SPDLOG)/%.cpp, $(PLATFORM_BUILD_DIR)/spdlog/%.$(OBJ_EXT), $(wildcard $(SRC_SPDLOG)/*.cpp))

#fmt
SRC_FMT := $(DEPS_PATH)/fmt/src
OBJS_FMT := $(patsubst $(SRC_FMT)/%.cc, $(PLATFORM_BUILD_DIR)/fmt/%.$(OBJ_EXT), $(wildcard $(SRC_FMT)/*.cc))


# $(info $(OBJS))
# $(info $(SRC_GLFW))
# $(info $(OBJS_GLFW))

# $(info $(SRC_FMT))
# $(info $(OBJS_FMT))

# $(patsubst %.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(TST_CPP_RAW)) \

# $(patsubst $(TST_NEURALSFD)/%.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(wildcard $(TST_NEURALSFD)/*.cpp)) \
# $(patsubst $(TST_SLANG)/%.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(wildcard $(TST_SLANG)/*.cpp)) \
# $(patsubst $(TST_IMAGEOPT)/%.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(wildcard $(TST_IMAGEOPT)/*.cpp)) \
# $(patsubst $(TST_RADIENCE)/%.cpp, $(PLATFORM_BUILD_DIR)/%.o, $(wildcard $(TST_RADIENCE)/*.cpp)) \


DEPFILES =  $(OBJS:.$(OBJ_EXT)=.d) \
			$(OBJS_IMGUI:.$(OBJ_EXT)=.d) \
			$(OBJS_STB:.$(OBJ_EXT)=.d) \
			$(OBJS_GLFW:.$(OBJ_EXT)=.d)

all: release

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

build_target: create_dirs $(TARGET)

_WINBDIR := $(subst /,\,$(PLATFORM_BUILD_DIR))
_WOBDIR := $(subst /,\,$(OBJS_BUILD_DIR))
_WMBDIR := $(subst /,\,$(MODULES_BUILD_DIR))
create_dirs:
ifeq ($(OS),Windows_NT)
	@cmd /c if not exist $(TARGET_DIR) mkdir $(TARGET_DIR)
	@cmd /c if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@cmd /c if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@cmd /c if not exist $(_WINBDIR) mkdir $(_WINBDIR)
	@cmd /c if not exist $(_WOBDIR) mkdir $(_WOBDIR)
	@cmd /c if not exist $(_WMBDIR) mkdir $(_WMBDIR)
	@cmd /c if not exist $(_WINBDIR)\imgui mkdir $(_WINBDIR)\imgui
	@cmd /c if not exist $(_WINBDIR)\stb mkdir $(_WINBDIR)\stb
	@cmd /c if not exist $(_WINBDIR)\glfw mkdir $(_WINBDIR)\glfw
	@cmd /c if not exist $(_WINBDIR)\fastgltf mkdir $(_WINBDIR)\fastgltf
	@cmd /c if not exist $(_WINBDIR)\spdlog mkdir $(_WINBDIR)\spdlog
	@cmd /c if not exist $(_WINBDIR)\fmt mkdir $(_WINBDIR)\fmt
else
	@mkdir -p \
	$(TARGET_DIR) $(BIN_DIR) \
	$(BUILD_DIR) \
	$(PLATFORM_BUILD_DIR) \
	$(OBJS_BUILD_DIR) \
	$(MODULES_BUILD_DIR) \
	$(PLATFORM_BUILD_DIR)/imgui \
	$(PLATFORM_BUILD_DIR)/stb \
	$(PLATFORM_BUILD_DIR)/glfw \
	$(PLATFORM_BUILD_DIR)/fastgltf \
	$(PLATFORM_BUILD_DIR)/spdlog \
	$(PLATFORM_BUILD_DIR)/fmt \


endif


$(TARGET): $(SUBMODULE_LIBS)
# $(TARGET): $(wildcard $(LIB_PATH)/*.lib)

# $(TARGET): $(OBJS) $(OBJS_IMGUI) $(OBJS_STB)
$(TARGET): \
	$(OBJS) \
	$(OBJS_IMGUI) \
	$(OBJS_STB) \
	$(OBJS_GLFW) \
	$(OBJS_FASTGLTF) \
	$(OBJS_SPDLOG) \
	
# $(OBJS_FMT)
# @echo "Linking $(notdir $^)"
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
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
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
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_NEURALSFD)/%.cpp # tests/NeuralSdf
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_IMAGEOPT)/%.cpp # tests/ImageOptimization
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_SLANG)/%.cpp # tests/SlangTest
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_FEATURE)/%.cpp # tests/FeatureTest
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_RADIENCE)/%.cpp # tests/RadienceField
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_HELLOTRIANGLE)/%.cpp # tests/HelloTriangle
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<
$(OBJS_BUILD_DIR)/%.$(OBJ_EXT): $(TST_WINDOW)/%.cpp # tests/Window
	@echo "Compiling $(notdir $<)"
	@$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $<



# ================ Modules ==================

# SRC_MODULES := source/Core/Lmath.cxx

ifeq ($(USE_MODULES), 1)
$(OBJS) : $(CPP_MODULE_TARGETS)
endif

$(MODULES_BUILD_DIR)/%.pcm: source/Core/%.cxx
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(CXXFLAGS) --precompile -c -x c++-module $< -o $@

$(MODULES_BUILD_DIR)/%.pcm: source/Base/%.cxx
	@echo "Compiling module $(notdir $<)"
	@$(CC) $(CXXFLAGS) --precompile -c -x c++-module $< -o $@

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
$(PLATFORM_BUILD_DIR)/glfw/%.$(OBJ_EXT): $(SRC_GLFW)/%.c # glfw/
	@echo "Compiling $(notdir $<)"
	@$(CC) -x c $(filter-out -std=c++20,$(CXXFLAGS)) -c $< $(-O)$@ -D$(GLFW_PLATFORM)

# Fastgltf
$(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT): $(SRC_FASTGLTF)/%.cpp # fastgltf/
	@echo "Compiling $(notdir $<)"
	$(CC) $(CXXFLAGS) $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fastgltf/include/ -I$(DEPS_PATH)/fastgltf/deps/simdjson

# Simdjson
$(PLATFORM_BUILD_DIR)/fastgltf/%.$(OBJ_EXT): $(SRC_SIMDJSON)/%.cpp # simdjson/
	@echo "Compiling $(notdir $<)"
	@$(CC) -MMD -MP $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fastgltf/deps/simdjson -O3

# spdlog
$(PLATFORM_BUILD_DIR)/spdlog/%.$(OBJ_EXT): $(SRC_SPDLOG)/%.cpp # spdlog/
	@echo "Compiling $(notdir $<)"
	@$(CC) -MMD -MP $(-O)$@ $(-C) $< -O3 -DSPDLOG_COMPILED_LIB -DSPDLOG_NO_EXCEPTIONS -I$(DEPS_PATH)/spdlog/include
# -I$(DEPS_PATH)/fmt/include -DSPDLOG_FMT_EXTERNAL -DSPDLOG_FMT_EXTERNAL_HO

#fmt
$(PLATFORM_BUILD_DIR)/fmt/%.$(OBJ_EXT): $(SRC_FMT)/%.cc # fmt/
	@echo "Compiling $(notdir $<)"
	@$(CC) -MMD -MP $(-O)$@ $(-C) $< -I$(DEPS_PATH)/fmt/include -fmodules-ts


-include $(wildcard $(DEPFILES))

# Run
run:
	@./$(TARGET_DIR)/$(TARGET)


# VLGRND_OUTPUT := --log-file="bin/mem"
# VLGRND_FULL := --leak-check=full 
# VLGRND_FULL += --show-leak-kinds=all

runv:
	@valgrind $(VLGRND_FULL) $(VLGRND_OUTPUT) ./$(TARGET) 
# @valgrind ./$(TARGET_DIR)/$(TARGET)

# $(info $(DEPFILES))

clean:
	@rm -f $(OBJS) $(DEPFILES) $(PLATFORM_BUILD_DIR)/$(TARGET).pdb $(wildcard $(PLATFORM_BUILD_DIR)/*.o) $(wildcard $(PLATFORM_BUILD_DIR)/*.obj)
	@echo "=== Cleaned ==="

cleanlib:
	@rm -f $(wildcard $(LIB_PATH)/*.lib) $(wildcard $(LIB_PATH)/*.a)
	@echo "=== Cleaned ==="

cleanimgui:
	@rm -f $(LIB_PATH)/libimgui.a
	@rm -f $(OBJS_IMGUI) $(OBJS_IMGUI:.o=.d)
	@echo "=== Cleaned ==="

cleanstb:
	@rm -f $(OBJS_STB) $(OBJS_STB:.o=.d)
	@echo "=== Cleaned ==="

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
$(LIB_PATH)/libfastgltf.$(LIB_EXT):
# cmake $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/build -G "MinGW Makefiles" ${CMAKE_FLAGS} -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ 
	@cmake --fresh $(DEPS_PATH)/fastgltf -B$(DEPS_PATH)/fastgltf/$(CMAKE_BUILD_DIR) ${CMAKE_FLAGS} -DCMAKE_CXX_FLAGS="-target x86_64-w64-mingw32 -femulated-tls" -DFASTGLTF_COMPILE_AS_CPP20=ON -DFASTGLTF_ENABLE_EXAMPLES=NO
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
 