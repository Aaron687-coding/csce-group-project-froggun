SRC_DIR = src
BUILD_DIR = build/debug
CC = g++
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_NAME = play
INCLUDE_PATH = -Iinclude
LIBRARY_PATH = -Llib
COMPILER_FLAGS = -std=c++14 -Wall -O0 -g
LINKER_FLAGS = -L/opt/homebrew/Cellar/sdl2/2.30.9/lib -L/opt/homebrew/Cellar/sdl2_image/2.8.2/lib


all:
	$(CC) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(SRC_FILES) -o $(BUILD_DIR)/$(OBJ_NAME)