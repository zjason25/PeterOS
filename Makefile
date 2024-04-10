# Project Name
PROJECT = 143b

# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -Iinclude

# Source directory
SRC_DIR = src

# Object directory
OBJ_DIR = obj

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Build the project
$(PROJECT): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compile the source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(PROJECT)
