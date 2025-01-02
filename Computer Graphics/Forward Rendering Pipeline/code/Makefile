# Define the compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++23

# Executable name
TARGET = rasterizer

# Source files
SRC = Main.cpp Camera.cpp Color.cpp Helpers.cpp Matrix4.cpp Mesh.cpp Rotation.cpp Scaling.cpp Scene.cpp tinyxml2.cpp Translation.cpp Triangle.cpp Vec3.cpp Vec4.cpp

# Header files
HEADERS = Camera.h Color.h Helpers.h Matrix4.h Mesh.h Rotation.h Scaling.h Scene.h tinyxml2.h Translation.h Triangle.h Vec3.h Vec4.h

# Object files
OBJ = $(SRC:.cpp=.o)

# Build target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Generic rule for object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJ) $(TARGET)
