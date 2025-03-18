#ifndef CODEGEN_UTILS_H
#define CODEGEN_UTILS_H

#include "context.h"
#include "../ast.h"

// Utility functions
void indent(CodeGenContext* ctx);
void increase_indent(CodeGenContext* ctx);
void decrease_indent(CodeGenContext* ctx);
char* get_temp_var_name(CodeGenContext* ctx);
char* get_label_name(CodeGenContext* ctx);
char* get_c_type(TypeInfo* type);

#endif // CODEGEN_UTILS_H
