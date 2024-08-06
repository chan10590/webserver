CC = g++
CCFLAGS = -Wall -std=c++11
SRC = $(wildcard *.cpp ../util/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))
TAR = client

all:$(TAR)
$(TAR):$(OBJ)
	$(CC) $^ -o $@
%.o:%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@
.PHONY: clean
clean:
	-rm -rf $(TAR) $(OBJ)
