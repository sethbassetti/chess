#The compiler for our c++ files, g++
CXX		= g++

#Compiler flags, -Wall for warnings -g for debugging
CXX_FLAGS	= -g -Wall -std=gnu++0x

#Target build, this will be the name of the executable
TARGET = main

#These just denote which folders to store files in
BIN = bin
SRC = src
INCLUDE = include

all: $(BIN)/$(TARGET)

run: clean all
	clear
	./$(BIN)/$(TARGET)

$(BIN)/$(TARGET): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ 

clean:
	-rm $(BIN)/*

