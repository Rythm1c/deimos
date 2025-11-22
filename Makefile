# Simple Makefile for deimos
# - Compiles only changed sources (make's default behavior)
# - Places object files under build/ while preserving directory structure
# - Targets: all (default), build, clean, run

CXX ?= g++
CC ?= gcc
PKG_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
PKG_LIBS := $(shell pkg-config --libs sdl2 2>/dev/null)

# Extra include paths: project root, external imgui and glad locations
EXTRA_INCLUDES := -I. -Iexternal/imgui -Iexternal/glad

CXXFLAGS ?= -std=c++17 -O2 -g -Wall -Wextra $(PKG_CFLAGS) $(EXTRA_INCLUDES)
CFLAGS ?= -O2 -g -Wall -Wextra $(PKG_CFLAGS) $(EXTRA_INCLUDES)

# Link against system GL and helpers; GLEW removed (using glad)
LDFLAGS ?= $(PKG_LIBS) -lGL -ldl -lm

# Find all .cc and .cpp sources (exclude build/)
SRCS := $(shell find . -type f \( -name '*.cc' -o -name '*.cpp' -o -name '*.c' \) -not -path './build/*' -printf '%P\n')
# Map sources to build object paths: foo/bar.cc -> build/foo/bar.o
OBJS := $(patsubst %.cc,build/%.o,$(filter %.cc,$(SRCS))) $(patsubst %.cpp,build/%.o,$(filter %.cpp,$(SRCS))) $(patsubst %.c,build/%.o,$(filter %.c,$(SRCS)))

TARGET := model_viewer

.PHONY: all build clean run help
all: $(TARGET)

build: all

# Link
$(TARGET): $(OBJS)
	@echo Linking $@
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile rule that creates directory for the object
build/%.o: %.cc
	@mkdir -p $(dir $@)
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: %.c
	@mkdir -p $(dir $@)
	@echo Compiling C $<
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo Cleaning build artifacts
	rm -rf build/ $(TARGET)

# Run the built executable. Provide ARGS on the make command line to pass arguments.
# Example: make run ARGS="models/alien/Alien.gltf"
run: $(TARGET)
	@echo "Running $(TARGET) $(ARGS)"
	./$(TARGET) $(ARGS)

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all / build   - build $(TARGET) (default)"
	@echo "  clean         - remove build artifacts and executable"
	@echo "  run           - run $(TARGET) (use ARGS variable to pass args)"
	@echo "Environment variables you can override: CXX, CXXFLAGS, LDFLAGS"

# Avoid rebuilding if timestamp not changed (default make behavior)
