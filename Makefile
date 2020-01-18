CC=gcc
CFLAGS=-std=c99 -Wall
LIBS=-lncursesw

DEP_DIR=include
OBJ_DIR=obj
SRC_DIR=src

BIN=server client main
DEP=$(wildcard $(DEP_DIR)/*.h)
OBJ:=$(patsubst $(DEP_DIR)/%.h, $(OBJ_DIR)/%.o, $(DEP)) 
OBJ:=$(filter-out $(OBJ_DIR)/macro.o, $(OBJ))

.PHONY: clean all

all: $(BIN)

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN)

run_%: %
	./$<

$(BIN): %: $(OBJ_DIR)/%.o $(OBJ) $(DEP_DIR)/macro.h
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

#.SILENT:

