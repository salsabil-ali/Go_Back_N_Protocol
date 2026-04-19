# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -std=c++11 -pthread -g

# Target executable name
TARGET = network_sim

# Source files
SRCS = main.cpp frame.cpp Network.cpp Receiver.cpp sender.cpp
# Object files (automatically generated from SRCS)
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove compiled files
clean:
	del -f $(OBJS) $(TARGET)
	del -f received_output.txt

# Rule to run the program
run: all
	./$(TARGET)