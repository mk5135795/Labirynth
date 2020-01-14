CFLAGS=-std=c99 -Wall
DEP=network.h lab.h graphic.h map.h maze.h
BIN=server client main
OBJ=$(DEP:%.h=%.o)
LIBS=-lncursesw

.PHONY: clean all

all: $(BIN)

clean:
	rm -f *.o $(BIN)

run_%: %
	./$<

$(BIN): %: %.o $(OBJ) macro.h
	gcc -g -o $@ $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEP)
	gcc -c -o $@ $< $(CFLAGS)

#.SILENT:

