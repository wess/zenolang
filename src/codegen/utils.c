#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// Generate indentation
void indent(CodeGenContext* ctx) {
    for (int i = 0; i < ctx->indentation; i++) {
        fprintf(ctx->output, "    ");
    }
}

// Increase indentation level
void increase_indent(CodeGenContext* ctx) {
    ctx->indentation++;
}

// Decrease indentation level
void decrease_indent(CodeGenContext* ctx) {
    if (ctx->indentation > 0) {
        ctx->indentation--;
    }
}

// Get a new temporary variable name
char* get_temp_var_name(CodeGenContext* ctx) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "__temp_%d", ctx->temp_var_count++);
    return strdup(buffer);
}

// Get a new label name
char* get_label_name(CodeGenContext* ctx) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "__label_%d", ctx->label_count++);
    return strdup(buffer);
}

// Convert our type to C type
char* get_c_type(TypeInfo* type) {
    if (!type) return strdup("void");
    
    if (strcmp(type->name, "int") == 0) {
        return strdup("int");
    } else if (strcmp(type->name, "float") == 0) {
        return strdup("float");
    } else if (strcmp(type->name, "bool") == 0) {
        return strdup("int"); // C doesn't have bool, use int
    } else if (strcmp(type->name, "string") == 0) {
        return strdup("char*");
    } else if (strcmp(type->name, "void") == 0) {
        return strdup("void");
    } else if (strcmp(type->name, "array") == 0 && type->generic_type) {
        // For arrays, we need to create a pointer to the generic type
        char* inner_type = get_c_type(type->generic_type);
        char* array_type = (char*)malloc(strlen(inner_type) + 3);
        sprintf(array_type, "%s*", inner_type);
        free(inner_type);
        return array_type;
    } else if (strcmp(type->name, "map") == 0) {
        // Placeholder for map type - use void* for now
        // TODO: Replace with actual map type (e.g., from a hashmap library)
        return strdup("void*");
    } else {
        // For custom types (structs), use struct prefix
        char* struct_type = (char*)malloc(strlen(type->name) + 8);
        sprintf(struct_type, "struct %s", type->name);
        return struct_type;
    }
}
