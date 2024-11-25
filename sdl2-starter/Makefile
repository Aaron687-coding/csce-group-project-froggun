# Knowledge of Make: https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents
# Make is sooo fun :')

SRC_DIR = src
BUILD_DIR = build/debug
CC = g++

# Explicitly list source files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/frogClass.cpp $(SRC_DIR)/DefaultShotgun.cpp
HEADERS = $(SRC_DIR)/GameState.h $(SRC_DIR)/gameplay.h $(SRC_DIR)/GameStateManager.h $(SRC_DIR)/frogClass.h $(SRC_DIR)/GunTemplate.h
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_NAME = play

# SDL paths - use system paths first, then local paths
SDL_INCLUDE = -I/opt/homebrew/include \
              -I/opt/homebrew/include/SDL2 \
              -I$(CURDIR)/include \
              -I$(CURDIR)/include/SDL2
# Mac OS setup above; You can make it a windows setup by replacing /opt/homebrew/ with C:/...
# e.g.,
# SDL_INCLUDE = -IC:/SDL2/include \        # If installed system-wide in C:/SDL2
#               -IC:/SDL2/include/SDL2 \
#               -I$(CURDIR)/include \      # Local project includes (same as current)
#               -I$(CURDIR)/include/SDL2 

# Compiler and linker flags
INCLUDE_PATHS = -I$(SRC_DIR) $(SDL_INCLUDE)
LIBRARY_PATHS = -L/opt/homebrew/lib
COMPILER_FLAGS = -std=c++14 -Wall -O0 -g
LINKER_FLAGS = -lSDL2 -lSDL2_image -Wl,-rpath,/opt/homebrew/lib

# Debug information
$(info Sources: $(SRCS))
$(info Objects: $(OBJS))
$(info Headers: $(HEADERS))

# Default target
.DEFAULT_GOAL := all

# Make sure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Copy assets to build directory
copy_assets:
	@echo "Copying assets..."
	@cp -r ../assets $(BUILD_DIR)/

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "Creating build directory..."
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Link object files into final executable
$(BUILD_DIR)/$(OBJ_NAME): $(OBJS) copy_assets
	@echo "Linking $@..."
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $@
	@echo "Build complete! Execute with: ./$(BUILD_DIR)/$(OBJ_NAME)"

.PHONY: all clean help copy_assets

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
