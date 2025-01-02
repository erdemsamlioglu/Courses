# Compiler and compiler flags
CXX = g++
CXXFLAGS = -Wall -pthread

# Target executable
TARGET = simulator

# Source files
SRCS = main.cpp WriteOutput.c helper.c

# Object files
OBJS = main.o WriteOutput.o helper.o

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Rule to build .o files from .cpp files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to build .o files from .c files
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
