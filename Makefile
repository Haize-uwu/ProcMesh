# Makefile for Raylib C++ Project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Include and Library paths
INCLUDE_PATHS = -I/usr/lib/include
LIB_PATHS = -L/usr/lib

# Libraries to link
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Source file and output executable
SRC = main.cpp
TARGET = mygame

# Default target
all: $(TARGET)

# Linking the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) $(LIB_PATHS) $(SRC) -o $(TARGET) $(LDLIBS)

# Clean up compiled files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean