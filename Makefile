CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

SRC = src/main.cpp src/Partition/Partition.cpp src/FileSystem/FileSystem.cpp src/BootableUSBCreation/BootableUSBCreation.cpp

OBJ = $(SRC:.cpp=.o)

EXEC = virtuoso

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $(EXEC) -lblkid

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
