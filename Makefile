# Compiler flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Wunused-variable -Wunused-parameter
INCLUDE_FLAGS = -I$(SRC_DIR)

# Flex and Bison
FLEX = flex
BISON = bison

# LLVM Config - find the right llvm-config
LLVM_CONFIG ?= $(shell \
    if command -v llvm-config > /dev/null; then \
        echo llvm-config; \
    elif [ -f /opt/homebrew/opt/llvm/bin/llvm-config ]; then \
        echo /opt/homebrew/opt/llvm/bin/llvm-config; \
    elif [ -f /usr/local/opt/llvm/bin/llvm-config ]; then \
        echo /usr/local/opt/llvm/bin/llvm-config; \
    else \
        echo "Error: llvm-config not found" >&2; \
        exit 1; \
    fi)

# Export the LLVM bin directory to PATH for compile-time tools
export PATH := $(shell dirname $(shell which $(LLVM_CONFIG))):$(PATH)

LLVM_CFLAGS = $(shell $(LLVM_CONFIG) --cflags)
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags)
LLVM_LIBS = $(shell $(LLVM_CONFIG) --libs core analysis native bitwriter)

# Mac Homebrew LLVM support
ifdef CPPFLAGS
  LLVM_CFLAGS += $(CPPFLAGS)
else
  # Fallback for Mac Homebrew LLVM if environment variable not set
  ifneq ($(wildcard /opt/homebrew/opt/llvm/include),)
    LLVM_CFLAGS += -I/opt/homebrew/opt/llvm/include
  endif
endif

ifdef LDFLAGS
  LLVM_LDFLAGS += $(LDFLAGS)
else
  # Fallback for Mac Homebrew LLVM if environment variable not set
  ifneq ($(wildcard /opt/homebrew/opt/llvm/lib),)
    LLVM_LDFLAGS += -L/opt/homebrew/opt/llvm/lib
  endif
endif

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = bin
GEN_DIR = $(BUILD_DIR)/gen

# Objects
OBJS = $(OBJ_DIR)/ast.o \
       $(OBJ_DIR)/symtab.o \
       $(OBJ_DIR)/codegen/context.o \
       $(OBJ_DIR)/codegen/utils.o \
       $(OBJ_DIR)/codegen/expression.o \
       $(OBJ_DIR)/codegen/statement.o \
       $(OBJ_DIR)/codegen/declaration.o \
       $(OBJ_DIR)/codegen/anon_function.o \
       $(OBJ_DIR)/codegen/codegen.o \
       $(OBJ_DIR)/llvm_codegen/llvm_context.o \
       $(OBJ_DIR)/llvm_codegen/llvm_codegen.o \
       $(OBJ_DIR)/lex.yy.o \
       $(OBJ_DIR)/parser.tab.o \
       $(OBJ_DIR)/main.o \
       $(OBJ_DIR)/zeno_cli.o \
       $(OBJ_DIR)/socket.o \
       $(OBJ_DIR)/error_reporter.o

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
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(GEN_DIR) $(OBJ_DIR)/codegen $(OBJ_DIR)/llvm_codegen

# Main target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) -o $@ $^ $(LLVM_LDFLAGS) $(LLVM_LIBS)

# Compile AST implementation
$(OBJ_DIR)/ast.o: $(SRC_DIR)/ast.c $(SRC_DIR)/ast.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Symbol Table implementation
$(OBJ_DIR)/symtab.o: $(SRC_DIR)/symtab.c $(SRC_DIR)/symtab.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Context
$(OBJ_DIR)/codegen/context.o: $(SRC_DIR)/codegen/context.c $(SRC_DIR)/codegen/context.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Utils
$(OBJ_DIR)/codegen/utils.o: $(SRC_DIR)/codegen/utils.c $(SRC_DIR)/codegen/utils.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Expression
$(OBJ_DIR)/codegen/expression.o: $(SRC_DIR)/codegen/expression.c $(SRC_DIR)/codegen/expression.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Statement
$(OBJ_DIR)/codegen/statement.o: $(SRC_DIR)/codegen/statement.c $(SRC_DIR)/codegen/statement.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Declaration
$(OBJ_DIR)/codegen/declaration.o: $(SRC_DIR)/codegen/declaration.c $(SRC_DIR)/codegen/declaration.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Anonymous Functions
$(OBJ_DIR)/codegen/anon_function.o: $(SRC_DIR)/codegen/anon_function.c $(SRC_DIR)/codegen/anon_function.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Codegen Module: Main Codegen
$(OBJ_DIR)/codegen/codegen.o: $(SRC_DIR)/codegen/codegen.c $(SRC_DIR)/codegen/codegen.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile LLVM Codegen Module: Context
$(OBJ_DIR)/llvm_codegen/llvm_context.o: $(SRC_DIR)/llvm_codegen/llvm_context.c $(SRC_DIR)/llvm_codegen/llvm_context.h
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile LLVM Codegen Module: Main Codegen
$(OBJ_DIR)/llvm_codegen/llvm_codegen.o: $(SRC_DIR)/llvm_codegen/llvm_codegen.c $(SRC_DIR)/llvm_codegen/llvm_codegen.h
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

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
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/ast.h $(SRC_DIR)/symtab.h $(SRC_DIR)/codegen/codegen.h $(SRC_DIR)/zeno_cli.h $(SRC_DIR)/llvm_codegen/llvm_codegen.h
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile CLI tool components
$(OBJ_DIR)/zeno_cli.o: $(SRC_DIR)/zeno_cli.c $(SRC_DIR)/zeno_cli.h
	$(CC) $(CFLAGS) $(LLVM_CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

# Compile Socket wrapper
$(OBJ_DIR)/socket.o: $(SRC_DIR)/socket.c $(SRC_DIR)/socket.h
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c -o $@ $<

$(OBJ_DIR)/error_reporter.o: $(SRC_DIR)/error_reporter.c $(SRC_DIR)/error_reporter.h
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

# Test with LLVM compiler
test-llvm: all
	@echo "Testing with LLVM compiler..."
	$(TARGET) transpile --llvm examples/hello.zn examples/hello.bc
	$(TARGET) compile --llvm -v examples/hello.zn
	@echo "LLVM tests completed!"

# Install Zeno CLI tool to /usr/local/bin
install: all
	@echo "Installing Zeno CLI tool..."
	cp $(TARGET) /usr/local/bin/zeno
	@echo "Installation completed!"

.PHONY: all dirs clean rebuild test test-llvm install
