#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "zeno_cli.h"
#include "llvm_codegen/llvm_codegen.h"
#include "codegen/codegen.h"

// External variables from Flex/Bison
extern FILE* yyin;
extern int yyparse();
extern AST_Node* root;

// Transpile a Zeno source file to C or LLVM
int transpile_file(const char* input_path, const char* output_path, bool verbose, bool use_llvm) {
    if (!input_path) {
        fprintf(stderr, "Error: No input file specified\n");
        return 1;
    }
    
    if (!output_path) {
        // Default output path: input file with .c or .bc extension
        char default_output[1024];
        char *ext = use_llvm ? ".bc" : ".c";
        snprintf(default_output, sizeof(default_output), "%s%s", input_path, ext);
        output_path = default_output;
    }
    
    if (verbose) {
        printf("Transpiling %s to %s%s\n", input_path, output_path, use_llvm ? " (using LLVM)" : "");
    }
    
    // Open input file
    yyin = fopen(input_path, "r");
    if (!yyin) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_path);
        return 1;
    }
    
    // Parse the input file
    int parse_result = yyparse();
    fclose(yyin);
    
    if (parse_result != 0) {
        fprintf(stderr, "Error: Parsing failed with code %d\n", parse_result);
        return 1;
    }
    
    if (!root) {
        fprintf(stderr, "Error: No AST root node generated\n");
        return 1;
    }
    
    if (use_llvm) {
        // Use LLVM backend
        LLVMGenContext* ctx = init_llvm_codegen("zeno_module", verbose);
        if (!ctx) {
            fprintf(stderr, "Error: Failed to initialize LLVM context\n");
            return 1;
        }
        
        // Generate LLVM IR
        llvm_generate_code(ctx, root);
        
        // Write LLVM output (either IR or compiled)
        int result;
        if (strstr(output_path, ".bc") || strstr(output_path, ".ll")) {
            // Write LLVM bitcode or IR
            result = write_llvm_module(ctx, output_path);
        } else {
            // Compile to executable
            result = llvm_compile_to_executable(ctx, output_path);
        }
        
        // Clean up
        cleanup_llvm_codegen(ctx);
        return result;
    } else {
        // Use C backend
        FILE* output_file = fopen(output_path, "w");
        if (!output_file) {
            fprintf(stderr, "Error: Could not open output file %s\n", output_path);
            return 1;
        }
        
        // Create codegen context
        CodeGenContext* ctx = init_codegen(output_file);
        if (!ctx) {
            fclose(output_file);
            fprintf(stderr, "Error: Failed to initialize codegen context\n");
            return 1;
        }
        
        // Generate C code
        generate_code(ctx, root);
        
        // Clean up
        cleanup_codegen(ctx);
        fclose(output_file);
        
        if (verbose) {
            printf("Successfully transpiled to %s\n", output_path);
        }
        
        return 0;
    }
}

// For parsing YAML
#define MAX_LINE_LENGTH 1024
#define MAX_INCLUDE_DIRS 32

// Function to create a default manifest.yaml file
int init_zeno_project(const char* dir_path, bool verbose) {
    char manifest_path[1024];
    if (dir_path) {
        snprintf(manifest_path, sizeof(manifest_path), "%s/manifest.yaml", dir_path);
        
        // Create directory if it doesn't exist
        struct stat st = {0};
        if (stat(dir_path, &st) == -1) {
            #ifdef _WIN32
            mkdir(dir_path);
            #else
            mkdir(dir_path, 0755);
            #endif
        }
    } else {
        snprintf(manifest_path, sizeof(manifest_path), "manifest.yaml");
    }
    
    // Check if manifest already exists
    FILE* check = fopen(manifest_path, "r");
    if (check) {
        fclose(check);
        fprintf(stderr, "Error: manifest.yaml already exists at %s\n", manifest_path);
        return 1;
    }
    
    // Create and write the default manifest
    FILE* file = fopen(manifest_path, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not create manifest.yaml at %s\n", manifest_path);
        return 1;
    }
    
    // Write default manifest content
    fprintf(file, "# Zeno project configuration\n");
    fprintf(file, "name: \"zeno_project\"\n");
    fprintf(file, "version: \"0.1.0\"\n");
    fprintf(file, "output:\n");
    fprintf(file, "  # Output directory for generated C files and binaries\n");
    fprintf(file, "  dir: \"./build\"\n");
    fprintf(file, "  # Binary name (defaults to project name if not specified)\n");
    fprintf(file, "  binary: \"zeno_project\"\n");
    fprintf(file, "source:\n");
    fprintf(file, "  # Main source file\n");
    fprintf(file, "  main: \"src/main.zeno\"\n");
    fprintf(file, "  # Include directories\n");
    fprintf(file, "  include:\n");
    fprintf(file, "    - \"src\"\n");
    fprintf(file, "    - \"lib\"\n");
    fprintf(file, "compiler:\n");
    fprintf(file, "  # C compiler to use\n");
    fprintf(file, "  cc: \"gcc\"\n");
    fprintf(file, "  # Compiler flags\n");
    fprintf(file, "  flags: \"-Wall -Wextra -O2\"\n");
    
    fclose(file);
    
    if (verbose) {
        printf("Created manifest.yaml at %s\n", manifest_path);
    }
    
    return 0;
}

// Simple YAML parser for manifest
ZenoManifest* load_manifest(const char* path, bool allow_missing) {
    FILE* file = fopen(path, "r");
    if (!file) {
        if (allow_missing) {
            // If manifest file doesn't exist but it's allowed, return default values
            ZenoManifest* manifest = (ZenoManifest*)malloc(sizeof(ZenoManifest));
            manifest->name = strdup("zeno_project");
            manifest->version = strdup("0.1.0");
            manifest->output.dir = strdup("./build");
            manifest->output.binary = strdup("zeno_project");
            manifest->source.main = NULL;
            manifest->source.include = (char**)malloc(sizeof(char*));
            manifest->source.include[0] = strdup(".");
            manifest->source.include_count = 1;
            manifest->compiler.cc = strdup("gcc");
            manifest->compiler.flags = strdup("-Wall -Wextra -Wunused-parameter -Wunused-variable");
            return manifest;
        } else {
            // If manifest file doesn't exist and it's not allowed, return NULL
            fprintf(stderr, "Error: Manifest file not found at %s\n", path);
            fprintf(stderr, "Run 'zeno init' to create a default manifest\n");
            return NULL;
        }
    }
    
    ZenoManifest* manifest = (ZenoManifest*)malloc(sizeof(ZenoManifest));
    manifest->name = strdup("zeno_project");
    manifest->version = strdup("0.1.0");
    manifest->output.dir = strdup("./build");
    manifest->output.binary = strdup("zeno_project");
    manifest->source.main = NULL;
    manifest->source.include = (char**)malloc(MAX_INCLUDE_DIRS * sizeof(char*));
    manifest->source.include_count = 0;
    manifest->compiler.cc = strdup("gcc");
    manifest->compiler.flags = strdup("-Wall -Wextra -Wunused-parameter -Wunused-variable");
    
    char line[MAX_LINE_LENGTH];
    int section = 0; // 0 = root, 1 = output, 2 = source, 3 = compiler
    int in_include = 0; // Flag for inside include list
    
    while (fgets(line, sizeof(line), file)) {
        // Remove leading/trailing whitespace
        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        char* end = trimmed + strlen(trimmed) - 1;
        while (end > trimmed && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        // Skip comments and empty lines
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;
        
        // Handle sections
        if (strstr(trimmed, "output:") == trimmed) {
            section = 1;
            continue;
        } else if (strstr(trimmed, "source:") == trimmed) {
            section = 2;
            continue;
        } else if (strstr(trimmed, "compiler:") == trimmed) {
            section = 3;
            continue;
        }
        
        // Handle key-value pairs
        char* key = trimmed;
        char* value = strchr(trimmed, ':');
        
        if (value) {
            *value = '\0';
            value++;
            while (*value == ' ' || *value == '\t') value++;
            
            // Remove quotes if present
            if (value[0] == '"' && value[strlen(value) - 1] == '"') {
                value[strlen(value) - 1] = '\0';
                value++;
            }
            
            // Determine indentation level
            int indent = 0;
            char* p = line;
            while (*p == ' ' || *p == '\t') {
                indent++;
                p++;
            }
            
            // Parse key-value based on section and indentation
            if (section == 0) {
                if (strcmp(key, "name") == 0) {
                    free(manifest->name);
                    manifest->name = strdup(value);
                } else if (strcmp(key, "version") == 0) {
                    free(manifest->version);
                    manifest->version = strdup(value);
                }
            } else if (section == 1 && indent >= 2) {
                if (strcmp(key, "dir") == 0) {
                    free(manifest->output.dir);
                    manifest->output.dir = strdup(value);
                } else if (strcmp(key, "binary") == 0) {
                    free(manifest->output.binary);
                    manifest->output.binary = strdup(value);
                }
            } else if (section == 2 && indent >= 2) {
                if (strcmp(key, "main") == 0) {
                    if (manifest->source.main) free(manifest->source.main);
                    manifest->source.main = strdup(value);
                } else if (strcmp(key, "include") == 0) {
                    in_include = 1;
                    // Reset include list if it's a new definition
                    for (int i = 0; i < manifest->source.include_count; i++) {
                        free(manifest->source.include[i]);
                    }
                    manifest->source.include_count = 0;
                }
            } else if (section == 3 && indent >= 2) {
                if (strcmp(key, "cc") == 0) {
                    free(manifest->compiler.cc);
                    manifest->compiler.cc = strdup(value);
                } else if (strcmp(key, "flags") == 0) {
                    free(manifest->compiler.flags);
                    manifest->compiler.flags = strdup(value);
                }
            }
        } else if (in_include && section == 2) {
            // Handle include list entries (- entry)
            if (trimmed[0] == '-' && trimmed[1] == ' ') {
                char* entry = trimmed + 2;
                // Remove quotes if present
                if (entry[0] == '"' && entry[strlen(entry) - 1] == '"') {
                    entry[strlen(entry) - 1] = '\0';
                    entry++;
                }
                
                if (manifest->source.include_count < MAX_INCLUDE_DIRS) {
                    manifest->source.include[manifest->source.include_count++] = strdup(entry);
                }
            } else {
                in_include = 0;
            }
        }
    }
    
    fclose(file);
    
    // Use project name for binary if not specified
    if (strcmp(manifest->output.binary, "zeno_project") == 0) {
        free(manifest->output.binary);
        manifest->output.binary = strdup(manifest->name);
    }
    
    return manifest;
}

// Free manifest resources
void free_manifest(ZenoManifest* manifest) {
    if (!manifest) return;
    
    free(manifest->name);
    free(manifest->version);
    free(manifest->output.dir);
    free(manifest->output.binary);
    if (manifest->source.main) free(manifest->source.main);
    
    for (int i = 0; i < manifest->source.include_count; i++) {
        free(manifest->source.include[i]);
    }
    free(manifest->source.include);
    
    free(manifest->compiler.cc);
    free(manifest->compiler.flags);
    
    free(manifest);
}

// Ensure directory exists
static void ensure_dir_exists(const char* dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        // Directory doesn't exist, create it
        #ifdef _WIN32
        mkdir(dir);
        #else
        mkdir(dir, 0755);
        #endif
    }
}

// Run Zeno file
int run_zeno_file(ZenoOptions* options, ZenoManifest* manifest) {
    const char* input_file = options->input_file;
    
    // If input_file not provided, try to use manifest.source.main
    if (!input_file) {
        if (!manifest->source.main) {
            fprintf(stderr, "No input file specified and no main file in manifest\n");
            return 1;
        }
        input_file = manifest->source.main;
    }
    
    // Create output directory if it doesn't exist
    ensure_dir_exists(manifest->output.dir);
    
    char output_path[1024];
    int result;
    
    if (options->use_llvm) {
        // Using LLVM backend, we compile directly to executable
        snprintf(output_path, sizeof(output_path), 
                 "%s/%s", manifest->output.dir, 
                 options->output_file ? options->output_file : "temp");
                 
        if (options->verbose) {
            printf("Compiling %s to %s using LLVM\n", input_file, output_path);
        }
        
        // Compile with LLVM
        result = transpile_file(input_file, output_path, options->verbose, true);
        if (result != 0) {
            fprintf(stderr, "LLVM compilation failed with code %d\n", result);
            return result;
        }
    } else {
        // Using C backend, we need to transpile to C and then compile
        char c_output_path[1024];
        snprintf(c_output_path, sizeof(c_output_path), 
                 "%s/%s.c", manifest->output.dir, 
                 options->output_file ? options->output_file : "temp");
                 
        snprintf(output_path, sizeof(output_path), 
                 "%s/%s", manifest->output.dir,
                 options->output_file ? options->output_file : "temp");
        
        // Transpile Zeno to C
        if (options->verbose) {
            printf("Transpiling %s to %s\n", input_file, c_output_path);
        }
        
        result = transpile_file(input_file, c_output_path, options->verbose, false);
        if (result != 0) {
            fprintf(stderr, "Transpilation failed with code %d\n", result);
            return result;
        }
        
        // Compile C to executable
        if (options->verbose) {
            printf("Compiling %s to %s\n", c_output_path, output_path);
        }
        
        char compile_cmd[2048];
        snprintf(compile_cmd, sizeof(compile_cmd), 
                 "%s %s -o %s %s", 
                 manifest->compiler.cc, c_output_path, output_path, manifest->compiler.flags);
        
        result = system(compile_cmd);
        if (result != 0) {
            fprintf(stderr, "C compilation failed with code %d\n", result);
            return result;
        }
    }
    
    // Run the executable
    if (options->verbose) {
        printf("Running %s\n", output_path);
    }
    
    result = system(output_path);
    
    // Clean up temporary files if not in verbose mode
    if (!options->verbose) {
        remove(output_path);
        
        if (!options->use_llvm) {
            char c_output_path[1024];
            snprintf(c_output_path, sizeof(c_output_path), 
                    "%s/%s.c", manifest->output.dir, 
                    options->output_file ? options->output_file : "temp");
            remove(c_output_path);
        }
    }
    
    return result;
}

// Compile Zeno file to a binary
int compile_zeno_file(ZenoOptions* options, ZenoManifest* manifest) {
    const char* input_file = options->input_file;
    
    // If input_file not provided, try to use manifest.source.main
    if (!input_file) {
        if (!manifest->source.main) {
            fprintf(stderr, "No input file specified and no main file in manifest\n");
            return 1;
        }
        input_file = manifest->source.main;
    }
    
    // Create output directory if it doesn't exist
    ensure_dir_exists(manifest->output.dir);
    
    char output_path[1024];
    int result;
    
    snprintf(output_path, sizeof(output_path), 
             "%s/%s", manifest->output.dir, 
             options->output_file ? options->output_file : manifest->output.binary);
    
    if (options->use_llvm) {
        // Using LLVM backend, we compile directly to executable
        if (options->verbose) {
            printf("Compiling %s to %s using LLVM\n", input_file, output_path);
        }
        
        // Compile with LLVM
        result = transpile_file(input_file, output_path, options->verbose, true);
        if (result != 0) {
            fprintf(stderr, "LLVM compilation failed with code %d\n", result);
            return result;
        }
    } else {
        // Using C backend, we need to transpile to C and then compile
        char c_output_path[1024];
        snprintf(c_output_path, sizeof(c_output_path), 
                 "%s/%s.c", manifest->output.dir, 
                 options->output_file ? options->output_file : manifest->name);
        
        // Transpile Zeno to C
        if (options->verbose) {
            printf("Transpiling %s to %s\n", input_file, c_output_path);
        }
        
        result = transpile_file(input_file, c_output_path, options->verbose, false);
        if (result != 0) {
            fprintf(stderr, "Transpilation failed with code %d\n", result);
            return result;
        }
        
        // Compile C to executable
        if (options->verbose) {
            printf("Compiling %s to %s\n", c_output_path, output_path);
        }
        
        char compile_cmd[2048];
        snprintf(compile_cmd, sizeof(compile_cmd), 
                 "%s %s -o %s %s", 
                 manifest->compiler.cc, c_output_path, output_path, manifest->compiler.flags);
        
        result = system(compile_cmd);
        if (result != 0) {
            fprintf(stderr, "C compilation failed with code %d\n", result);
            return result;
        }
    }
    
    if (options->verbose) {
        printf("Successfully compiled %s\n", output_path);
    }
    
    return 0;
}
