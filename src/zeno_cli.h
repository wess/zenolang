#ifndef ZENO_CLI_H
#define ZENO_CLI_H

#include <stdbool.h>

// Manifest structure
typedef struct {
    char* name;
    char* version;
    
    struct {
        char* dir;
        char* binary;
    } output;
    
    struct {
        char* main;
        char** include;
        int include_count;
    } source;
    
    struct {
        char* cc;
        char* flags;
    } compiler;
} ZenoManifest;

// Command options
typedef struct {
    bool verbose;
    char* manifest_path;
    char* input_file;
    char* output_file;
    bool run_mode;
    bool compile_mode;
    bool use_llvm;    // New flag for LLVM backend
} ZenoOptions;

// Function declarations
ZenoManifest* load_manifest(const char* path, bool allow_missing);
void free_manifest(ZenoManifest* manifest);
int run_zeno_file(ZenoOptions* options, ZenoManifest* manifest);
int compile_zeno_file(ZenoOptions* options, ZenoManifest* manifest);
int init_zeno_project(const char* dir_path, bool verbose);

// Transpile function (defined in main.c)
int transpile_file(const char* input_path, const char* output_path, bool verbose, bool use_llvm);

#endif // ZENO_CLI_H