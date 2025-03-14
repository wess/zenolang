# Compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Wunused-variable -Wunused-parameter
INCLUDE_FLAGS = -I$(SRC_DIR)

# Flex and Bison
FLEX = flex
BISON = bison

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = bin
GEN_DIR = $(BUILD_DIR)/gen

# Objects
OBJS = $(OBJ_DIR)/ast.o \
       $(OBJ_DIR)/symtab.o \
       $(OBJ_DIR)/codegen.o \
       $(OBJ_DIR)/lex.yy.o \
       $(OBJ_DIR)/parser.tab.o \
       $(OBJ_DIR)/main.o \
       $(OBJ_DIR)/zeno_cli.o

# Generated sources
GEN_PARSER_C = $(GEN_DIR)/parser.tab.c
GEN_PARSER_H = $(GEN_DIR)/parser.tab.h
GEN_LEXER_C = $(GEN_DIR)/lex.yy.c

# Target
TARGET = $(BIN_DIR)/zeno

# Default target
all: dirs $(TARGET)

# Create directories
dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(GEN_DIR)

# Main target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile AST implementation
$(OBJ_DIR)/ast.o: $(SRC_DIR)/ast.c $(SRC_DIR)/ast.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Symbol Table implementation
$(OBJ_DIR)/symtab.o: $(SRC_DIR)/symtab.c $(SRC_DIR)/symtab.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Code Generation implementation
$(OBJ_DIR)/codegen.o: $(SRC_DIR)/codegen.c $(SRC_DIR)/codegen.h $(SRC_DIR)/ast.h $(SRC_DIR)/symtab.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Generate parser with Bison
$(GEN_PARSER_C) $(GEN_PARSER_H): $(SRC_DIR)/parser.y
	$(BISON) -d -o $(GEN_PARSER_C) $<

# Generate lexer with Flex
$(GEN_LEXER_C): $(SRC_DIR)/lexer.l $(GEN_PARSER_H)
	$(FLEX) -o $@ $<

# Compile parser
$(OBJ_DIR)/parser.tab.o: $(GEN_PARSER_C)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile lexer
$(OBJ_DIR)/lex.yy.o: $(GEN_LEXER_C)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -Wno-sign-compare -c -o $@ $<

# Compile main
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/ast.h $(SRC_DIR)/symtab.h $(SRC_DIR)/codegen.h $(SRC_DIR)/zeno_cli.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile CLI tool components
$(OBJ_DIR)/zeno_cli.o: $(SRC_DIR)/zeno_cli.c $(SRC_DIR)/zeno_cli.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Clean and rebuild
rebuild: clean all

# Test with example files
test: all
	@echo "Testing with examples..."
	$(TARGET) transpile examples/hello.zn examples/hello.c
	$(TARGET) transpile examples/structs.zn examples/structs.c
	$(TARGET) compile -v examples/hello.zn
	@echo "Tests completed!"

# Install Zeno CLI tool to /usr/local/bin
install: all
	@echo "Installing Zeno CLI tool..."
	cp $(TARGET) /usr/local/bin/zeno
	@echo "Installation completed!"

.PHONY: all dirs clean rebuild test install
