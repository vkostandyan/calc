CXX=g++
CXXFLAGS=-std=c++17 -Iinclude -Wall -Wextra -O2
TARGET=calc
SRCS=$(wildcard src/*.cpp)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
