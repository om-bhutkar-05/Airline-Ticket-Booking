# Basic Makefile Example
# Compiler
CXX = g++
# Compiler flags (enable warnings, C++11 standard, debugging symbols)
CXXFLAGS = -Wall -Wextra -std=c++11 -g
# Include directory
INCLUDE_DIR = -Iinclude
# Linker flags (if needed)
LDFLAGS =

# Directories
SRC_DIR = src
INCLUDE_DIR_PATH = include # Path used in include statements is relative to this

# Source files
# Find all .cpp files in the src directory and its subdirectories
SRCS = $(wildcard $(SRC_DIR)/heap/*.cpp $(SRC_DIR)/booking/*.cpp main.cpp)

# Object files: replace .cpp with .o and put them in an obj directory (optional)
# OBJS = $(SRCS:.cpp=.o)
# For simplicity, let's compile directly to executable for now:

# Target executable name
TARGET = airline_booking

# Default target
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIR) $^ -o $@ $(LDFLAGS)
# $^ means all prerequisites (source files)
# $@ means the target name

# Clean up object files and the executable
clean:
	rm -f $(TARGET) *.o # Remove executable and any stray .o files

# Phony targets are rules that don't correspond to actual files
.PHONY: all clean
