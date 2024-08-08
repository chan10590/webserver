CC = g++
CCFLAGS = -Wall -g -O0 -std=c++11
SRC = $(wildcard ../src/*.cpp ../util/*.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))
OBJ1 = server.o
OBJ2 = client.o
TAR1 = server
TAR2 = client

server:
$(TAR1):$(OBJ) $(OBJ1)
	$(CC) $^ -o $@
%.o:%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

client:
$(TAR2):$(OBJ) $(OBJ2)
	$(CC) $^ -o $@
%.o:%.cpp
	$(CC) $(CCFLAGS) -c $< -o $@
.PHONY: cleanobj cleantar
cleanobj:
	-rm -rf $(OBJ) $(OBJ1) $(OBJ2)
cleantar:
	-rm -rf $(TAR1) $(TAR2) 
