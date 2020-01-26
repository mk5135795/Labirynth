CC=gcc
CFLAGS=-std=c99 -Wall
LIBS=-lncursesw

BIN_DIR=bin
DEP_DIR=include
SRC_DIR=src
OBJ_DIR=obj

BIN=server client main
DEP=$(wildcard $(DEP_DIR)/*.h)
OBJ:=$(patsubst $(DEP_DIR)/%.h, $(OBJ_DIR)/%.o, $(DEP)) 
OBJ:=$(filter-out $(OBJ_DIR)/macro.o, $(OBJ))

BIN_FILES:=$(addprefix $(BIN_DIR)/, $(BIN))

.PHONY: clean all run_%

all: $(BIN_FILES)

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_FILES)

run_%: $(BIN_DIR)/%
	./$<

$(BIN_FILES): $(BIN_DIR)/%: $(OBJ_DIR)/%.o $(OBJ) $(DEP_DIR)/macro.h
	$(CC) -g -o $@ $^ $(CFLAGS) $(LIBS)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

#.SILENT:

