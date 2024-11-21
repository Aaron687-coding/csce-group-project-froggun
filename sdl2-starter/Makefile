# Now I know how to code using make :')

SRC_DIR = src
BUILD_DIR = build/debug
CC = g++
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(SRC_DIR)/*.h)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_NAME = play

INCLUDE_PATHS = -Iinclude
LIBRARY_PATHS = -Llib
COMPILER_FLAGS = -std=c++14 -Wall -O0 -g
LINKER_FLAGS = -L/opt/homebrew/Cellar/sdl2/2.30.9/lib -L/opt/homebrew/Cellar/sdl2_image/2.8.2/lib
SDL_FLAGS = -lSDL2-2.0 -lSDL2_image

# Make sure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) -c $< -o $@ # $< is the source file, $@ is the object file

# Link object files into final executable
$(BUILD_DIR)/$(OBJ_NAME): $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(LIBRARY_PATHS) $(SDL_FLAGS) -o $@

.PHONY: all clean # Let make know that "all" and "clean" are not files

all: $(BUILD_DIR)/$(OBJ_NAME) # Default target; thus, "make" is equivalent to "make all"

clean:
	rm -rf $(BUILD_DIR)/* # removes every file if you call "make clean" instead of just "make"
