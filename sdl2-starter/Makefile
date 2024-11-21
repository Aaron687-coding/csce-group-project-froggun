SRC_DIR = src
BUILD_DIR = build/debug
CC = g++

# Explicitly list source files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/frogClass.cpp
HEADERS = $(SRC_DIR)/GameState.h $(SRC_DIR)/gameplay.h $(SRC_DIR)/GameStateManager.h
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_NAME = play

# SDL paths
SDL_INCLUDE = -I/opt/homebrew/Cellar/sdl2/2.30.9/include/SDL2 \
              -I/opt/homebrew/Cellar/sdl2_image/2.8.2/include/SDL2
SDL_LIB = -L/opt/homebrew/Cellar/sdl2/2.30.9/lib \
          -L/opt/homebrew/Cellar/sdl2_image/2.8.2/lib

# Compiler and linker flags
INCLUDE_PATHS = -Iinclude $(SDL_INCLUDE)
LIBRARY_PATHS = $(SDL_LIB)
COMPILER_FLAGS = -std=c++14 -Wall -O0 -g
# Changed SDL2-2.0 to SDL2 for macOS
LINKER_FLAGS = -lSDL2 -lSDL2_image

# Debug information
$(info Sources: $(SRCS))
$(info Objects: $(OBJS))
$(info Headers: $(HEADERS))

# Default target
.DEFAULT_GOAL := all

# Make sure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Creating build directory..."
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Link object files into final executable
$(BUILD_DIR)/$(OBJ_NAME): $(OBJS)
	@echo "Linking $@..."
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $@
	@echo "Build complete! Execute with: ./$(BUILD_DIR)/$(OBJ_NAME)"

.PHONY: all clean help

all: $(BUILD_DIR)/$(OBJ_NAME)

clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)/*
	@echo "Clean complete!"

help:
	@echo "Available targets:"
	@echo "  make       - Build the project"
	@echo "  make clean - Remove all built files"
	@echo "  make help  - Show this help message"

# Debug target
debug:
	@echo "Build Directory: $(BUILD_DIR)"
	@echo "Source Files: $(SRCS)"
	@echo "Object Files: $(OBJS)"
	@echo "Header Files: $(HEADERS)"
	@echo "Compiler Flags: $(COMPILER_FLAGS)"
	@echo "Include Paths: $(INCLUDE_PATHS)"
	@echo "Library Paths: $(LIBRARY_PATHS)"
	@echo "Linker Flags: $(LINKER_FLAGS)"
