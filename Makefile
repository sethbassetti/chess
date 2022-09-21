#The compiler for our c++ files, g++
CXX		= g++

#Compiler flags, -Wall for warnings -g for debugging
CXX_FLAGS	= -g -Wall -std=gnu++0x -Ofast
MINGW_FLAGS 	= -Ofast --static

#Target build, this will be the name of the executable
TARGET = main

#These just denote which folders to store files in
BIN = bin
SRC = src
INCLUDE = -I include

all: clean build run


build:	$(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ -o $(BIN)/$(TARGET)
	x86_64-w64-mingw32-g++ $(MINGW_FLAGS) $(INCLUDE) $^ -o $(BIN)/main.exe


run: $(BIN)/$(TARGET)
	./$(BIN)/$(TARGET)


clean:
	-$(RM) $(BIN)/*

