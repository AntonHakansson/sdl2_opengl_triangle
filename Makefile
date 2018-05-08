CC := g++
CC_FLAGS := -Wall -g -msse4.1 -std=c++11
LIB := -lSOIL -lSDL2 -lGLEW -lGL -lopenal -lGLU -pthread

INC_DIR := .
SRC_DIR := .

EXEC := triangle

make:
	$(CC) $(CC_FLAGS) $(SRC_DIR)/main.cpp $(LIB) -I $(INC_DIR) -o $(EXEC)
