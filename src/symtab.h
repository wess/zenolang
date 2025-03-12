#ifndef SYMTAB_H
#define SYMTAB_H

// Symbol types
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_STRUCT,
    SYMBOL_TYPE
} SymbolType;

// Symbol entry
typedef struct SymbolEntry {
    char* name;
    SymbolType type;
    struct SymbolEntry* next;
} SymbolEntry;

// Symbol table scope
typedef struct SymbolScope {
    SymbolEntry* entries;
    struct SymbolScope* parent;
} SymbolScope;

// Symbol table
typedef struct {
    SymbolScope* current_scope;
} SymbolTable;

// Initialize symbol table
SymbolTable* init_symbol_table();

// Enter a new scope
void enter_scope(SymbolTable* table);

// Leave current scope
void leave_scope(SymbolTable* table);

// Add symbol to current scope
void add_symbol(SymbolTable* table, char* name, SymbolType type);

// Look up symbol in all accessible scopes
SymbolEntry* lookup_symbol(SymbolTable* table, char* name);

// Clean up symbol table
void cleanup_symbol_table(SymbolTable* table);

#endif // SYMTAB_H