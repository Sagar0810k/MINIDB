SHELL := cmd

CC = gcc
CFLAGS = -Wall -g

LEX = flex
YACC = bison -d

SRC_DIR = ../src
OBJ_DIR = ../obj

OBJS = \
	$(OBJ_DIR)/ast.o \
	$(OBJ_DIR)/utils.o \
	$(OBJ_DIR)/catalog.o \
	$(OBJ_DIR)/semantic.o \
	$(OBJ_DIR)/storage.o \
	$(OBJ_DIR)/executor.o \
	$(OBJ_DIR)/parser.tab.o \
	$(OBJ_DIR)/lex.yy.o \
	$(OBJ_DIR)/main.o

TARGET = minidb

all: dirs $(TARGET)

dirs:
	if not exist "..\obj" mkdir "..\obj"

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# ---- Bison ----
$(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	$(YACC) -o $(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.y

# ---- Flex ----
$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l
	$(LEX) -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/lexer.l

# ---- Compile ----
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ---- Clean (Windows-safe) ----
clean:
	if exist "..\obj" rmdir /s /q "..\obj"
	if exist "$(TARGET).exe" del "$(TARGET).exe"
	if exist "$(SRC_DIR)\lex.yy.c" del "$(SRC_DIR)\lex.yy.c"
	if exist "$(SRC_DIR)\parser.tab.c" del "$(SRC_DIR)\parser.tab.c"
	if exist "$(SRC_DIR)\parser.tab.h" del "$(SRC_DIR)\parser.tab.h"

run: all
	$(TARGET).exe
