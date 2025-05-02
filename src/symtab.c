#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

// Initialize symbol table
SymbolTable* init_symbol_table() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    // Create global scope
    table->current_scope = (SymbolScope*)malloc(sizeof(SymbolScope));
    if (!table->current_scope) {
        fprintf(stderr, "Memory allocation failed\n");
        free(table);
        exit(1);
    }
    
    table->current_scope->entries = NULL;
    table->current_scope->parent = NULL;
    
    return table;
}

// Enter a new scope
void enter_scope(SymbolTable* table) {
    if (!table) return;
    
    SymbolScope* new_scope = (SymbolScope*)malloc(sizeof(SymbolScope));
    if (!new_scope) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    new_scope->entries = NULL;
    new_scope->parent = table->current_scope;
    table->current_scope = new_scope;
}

// Leave current scope
void leave_scope(SymbolTable* table) {
    if (!table || !table->current_scope || !table->current_scope->parent) {
        return; // Cannot leave global scope
    }
    
    SymbolScope* old_scope = table->current_scope;
    table->current_scope = old_scope->parent;
    
    // Clean up entries in the old scope
    SymbolEntry* entry = old_scope->entries;
    while (entry) {
        SymbolEntry* next = entry->next;
        free(entry->name);
        free(entry);
        entry = next;
    }
    
    free(old_scope);
}

// Add symbol to current scope
void add_symbol(SymbolTable* table, char* name, SymbolType type, TypeInfo* type_info) { // Added type_info parameter
    if (!table || !table->current_scope || !name) return;
    
    // Check if symbol already exists in current scope
    SymbolEntry* entry = table->current_scope->entries;
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            fprintf(stderr, "Symbol %s already defined in current scope\n", name);
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry
    SymbolEntry* new_entry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
    if (!new_entry) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    new_entry->name = strdup(name);
    new_entry->type = type;
    new_entry->type_info = type_info; // Store the type info
    new_entry->next = table->current_scope->entries;
    table->current_scope->entries = new_entry;
}

// Look up symbol in all accessible scopes
SymbolEntry* lookup_symbol(SymbolTable* table, char* name) {
    if (!table || !name) return NULL;
    
    // Search in current scope and all parent scopes
    SymbolScope* scope = table->current_scope;
    while (scope) {
        SymbolEntry* entry = scope->entries;
        while (entry) {
            if (strcmp(entry->name, name) == 0) {
                return entry;
            }
            entry = entry->next;
        }
        scope = scope->parent;
    }
    
    return NULL; // Symbol not found
}

// Clean up symbol table
void cleanup_symbol_table(SymbolTable* table) {
    if (!table) return;
    
    // Leave all scopes until we reach the global scope
    while (table->current_scope && table->current_scope->parent) {
        leave_scope(table);
    }
    
    // Clean up global scope
    if (table->current_scope) {
        SymbolEntry* entry = table->current_scope->entries;
        while (entry) {
            SymbolEntry* next = entry->next;
            free(entry->name);
            free(entry);
            entry = next;
        }
        free(table->current_scope);
    }
    
    free(table);
}
