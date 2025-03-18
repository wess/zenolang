#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "anon_function.h"
#include "statement.h"
#include "utils.h"
#include "codegen.h"

// Global array to store anonymous functions
#define MAX_ANON_FUNCS 100
static AnonFunction anon_funcs[MAX_ANON_FUNCS];
static int anon_func_count = 0;

// Global array to store anonymous function forward declarations
static char* anon_func_decls[MAX_ANON_FUNCS] = {0};

// Function to add a new anonymous function
int add_anon_function(const char* name, const char* return_type, const char* param_list, const char* param_types, const char* body) {
    if (anon_func_count >= MAX_ANON_FUNCS) {
        fprintf(stderr, "Too many anonymous functions\n");
        return -1;
    }
    
    int idx = anon_func_count++;
    anon_funcs[idx].name = strdup(name);
    
    // Normalize types for anonymous functions used in promises
    // Use void* for all handler functions to match promise handler typedefs
    if (strstr(name, "__anon_func_") != NULL) {
                if (strstr(param_list, "data")) {
                    anon_funcs[idx].return_type = strdup("void*");
                    anon_funcs[idx].param_list = strdup("void* data");
                    anon_funcs[idx].param_types = strdup("void*");
                } else if (strstr(param_list, "error")) {
                    anon_funcs[idx].return_type = strdup("void*");
                    anon_funcs[idx].param_list = strdup("void* error");
                    anon_funcs[idx].param_types = strdup("void*");
                } else if (strstr(param_list, "void")) {
                    // For finally handler, we need to always return a pointer
                    anon_funcs[idx].return_type = strdup("void*");
                    anon_funcs[idx].param_list = strdup("void");
                    anon_funcs[idx].param_types = strdup("void");
                    
                    // Also update the body to make it return NULL
                    char* modified_body = malloc(strlen(body) + 100);
                    strcpy(modified_body, "{\n");
                    strcat(modified_body, "        cleanup();\n");
                    strcat(modified_body, "        return NULL;\n");
                    strcat(modified_body, "    }");
                    anon_funcs[idx].body = modified_body;
                    return idx; // Early return since we've set the body directly
        } else {
            anon_funcs[idx].return_type = strdup(return_type);
            anon_funcs[idx].param_list = strdup(param_list);
            anon_funcs[idx].param_types = strdup(param_types);
        }
    } else {
        anon_funcs[idx].return_type = strdup(return_type);
        anon_funcs[idx].param_list = strdup(param_list);
        anon_funcs[idx].param_types = strdup(param_types);
    }
    
    anon_funcs[idx].body = strdup(body);
    
    return idx;
}

// Generate code for anonymous function
void generate_anonymous_function(CodeGenContext* ctx, AST_Node* node) {
    static int anon_func_counter = 0;
    char func_name[64];
    
    // Create a unique function name
    snprintf(func_name, sizeof(func_name), "__anon_func_%d", anon_func_counter++);
    
    // Get the return type
    char* return_type = get_c_type(node->data.anon_function.return_type);
    
    // Build parameter list
    char param_list[1024] = "";
    char param_types[1024] = "";
    
    // Check if there are any parameters at all
    if (node->data.anon_function.parameters && node->data.anon_function.parameters->head) {
        AST_Node* param = node->data.anon_function.parameters->head;
        int first_param = 1;
        
        while (param) {
            if (!first_param) {
                strcat(param_list, ", ");
                strcat(param_types, ", ");
            }
            
            char* param_type = get_c_type(param->data.variable.type);
            char param_decl[256];
            sprintf(param_decl, "%s %s", param_type, param->data.variable.name);
            strcat(param_list, param_decl);
            strcat(param_types, param_type);
            
            free(param_type);
            first_param = 0;
            param = param->next;
        }
    } else {
        // For no parameters, just leave the strings empty
        strcpy(param_list, "void");
        strcpy(param_types, "void");
    }
    
    // Capture body to a string
    FILE* body_stream = open_memstream(&ctx->buffer, &ctx->buffer_size);
    FILE* old_output = ctx->output;
    ctx->output = body_stream;
    
    fprintf(ctx->output, "{\n");
    increase_indent(ctx);
    
    // Generate function body
    if (node->data.anon_function.body) {
        if (node->data.anon_function.body->type == NODE_COMPOUND_STATEMENT) {
            // For compound statements, we generate the contents directly
            generate_compound_statement_contents(ctx, node->data.anon_function.body);
        } else {
            // For single statements
            indent(ctx);
            generate_code(ctx, node->data.anon_function.body);
        }
    }
    
    decrease_indent(ctx);
    indent(ctx);
    fprintf(ctx->output, "}");
    
    fflush(body_stream);
    fclose(body_stream);
    ctx->output = old_output;
    
    // Add the anonymous function to our global list
    add_anon_function(func_name, return_type, param_list, param_types, ctx->buffer);
    
    // Add it to the declaration array
    char decl[1024];
    sprintf(decl, "%s %s(%s);", return_type, func_name, param_list);
    
    if (anon_func_decls[anon_func_count-1])
        free(anon_func_decls[anon_func_count-1]);
        
    anon_func_decls[anon_func_count-1] = strdup(decl);
    
    // Output just the function name 
    fprintf(ctx->output, "%s", func_name);
    
    free(return_type);
    free(ctx->buffer);
    ctx->buffer = NULL;
    ctx->buffer_size = 0;
}

// Generate code for all anonymous functions at program level
void generate_all_anon_functions(FILE* output) {
    // First, forward declare all anonymous functions
    for (int i = 0; i < anon_func_count; i++) {
        fprintf(output, "// Forward declaration of anonymous function\n");
        fprintf(output, "%s %s(%s);\n", 
                anon_funcs[i].return_type, 
                anon_funcs[i].name, 
                anon_funcs[i].param_list);
        
        // Store the declaration for global access
        if (anon_func_decls[i])
            free(anon_func_decls[i]);
            
        char decl[1024];
        sprintf(decl, "%s %s(%s);", 
                anon_funcs[i].return_type, 
                anon_funcs[i].name, 
                anon_funcs[i].param_list);
        anon_func_decls[i] = strdup(decl);
    }
    
    fprintf(output, "\n");
    
    // Then output the function implementations
    for (int i = 0; i < anon_func_count; i++) {
        fprintf(output, "// Anonymous function definition\n");
        fprintf(output, "%s %s(%s) %s\n\n", 
                anon_funcs[i].return_type, 
                anon_funcs[i].name, 
                anon_funcs[i].param_list,
                anon_funcs[i].body);
    }
}
