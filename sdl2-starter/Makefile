# Knowledge of Make: https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents
# Make is sooo fun :')

SRC_DIR = src
BUILD_DIR = build/debug
CC = g++

# Explicitly list source files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/frog/frogClass.cpp \
       $(SRC_DIR)/guns/DefaultShotgun.cpp \
       $(SRC_DIR)/turtle/turtleStruct.cpp \
       $(SRC_DIR)/turtle/turtBullet/bulletStruct.cpp \
       $(SRC_DIR)/wasp/waspStruct.cpp \
       $(SRC_DIR)/terrain/TerrainGrid.cpp \
       $(SRC_DIR)/terrain/MenuState.cpp \
       $(SRC_DIR)/terrainElem.cpp \
	   $(SRC_DIR)/healthBar.cpp \
	   $(SRC_DIR)/waterPhysics.cpp \
	   $(SRC_DIR)/hurtFlash.cpp

HEADERS = $(SRC_DIR)/GameState.h \
          $(SRC_DIR)/gameplay.h \
          $(SRC_DIR)/GameStateManager.h \
          $(SRC_DIR)/frog/frogClass.h \
          $(SRC_DIR)/guns/GunTemplate.h \
          $(SRC_DIR)/guns/DefaultShotgun.h \
          $(SRC_DIR)/turtle/turtleStruct.h \
          $(SRC_DIR)/turtle/turtBullet/bulletStruct.h \
          $(SRC_DIR)/wasp/waspStruct.h \
          $(SRC_DIR)/terrain/TerrainGrid.h \
          $(SRC_DIR)/terrain/TerrainState.h \
          $(SRC_DIR)/terrain/MenuState.h \
          $(SRC_DIR)/terrainElem.h \
		  $(SRC_DIR)/RainSystem.h \
		  $(SRC_DIR)/waterPhysics.h \
		  $(SRC_DIR)/hurtFlash.h \

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_NAME = play

# SDL paths - use system paths first, then local paths
SDL_INCLUDE = -I/opt/homebrew/include \
              -I/opt/homebrew/include/SDL2 \
              -I/opt/homebrew/Cellar/sdl2_image/2.8.2_2/include/SDL2 \
              -I/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2 \
              -I$(CURDIR)/include \
              -I$(CURDIR)/include/SDL2

# Compiler and linker flags
INCLUDE_PATHS = -I$(SRC_DIR) $(SDL_INCLUDE)
LIBRARY_PATHS = -L/opt/homebrew/lib \
                -L/opt/homebrew/Cellar/sdl2_image/2.8.2_2/lib \
                -L/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib \
                -L$(CURDIR)/lib/SDL2

COMPILER_FLAGS = -std=c++14 -Wall -O0 -g -arch arm64
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -Wl,-rpath,'/opt/homebrew/lib'

# Debug information
$(info Sources: $(SRCS))
$(info Objects: $(OBJS))
$(info Headers: $(HEADERS))

# Default target
.DEFAULT_GOAL := all

# Make sure build directory exists
create_dirs:
	@mkdir -p $(BUILD_DIR)/frog
	@mkdir -p $(BUILD_DIR)/guns
	@mkdir -p $(BUILD_DIR)/turtle/turtBullet
	@mkdir -p $(BUILD_DIR)/wasp
	@mkdir -p $(BUILD_DIR)/terrain
	@mkdir -p $(BUILD_DIR)/fonts

# Copy assets to build directory
copy_assets: create_dirs
	@echo "Copying assets..."
	@cp -r ../assets $(BUILD_DIR)/
	@echo "Copying fonts..."
	@cp -r ../fonts/* $(BUILD_DIR)/fonts/

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo "Creating build directory..."
	@mkdir -p $(@D)
	@echo "Compiling $<..."
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Link object files into final executable
$(BUILD_DIR)/$(OBJ_NAME): create_dirs $(OBJS) copy_assets
	@echo "Linking $@..."
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $@
	@echo "Build complete! Execute with: ./$(BUILD_DIR)/$(OBJ_NAME)"

.PHONY: all clean help copy_assets create_dirs

all: $(BUILD_DIR)/$(OBJ_NAME)

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
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
