#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "zeno_cli.h"

// Function prototypes
static void print_usage(const char* program_name);

// For parsing YAML
#define MAX_LINE_LENGTH 1024
#define MAX_INCLUDE_DIRS 32

// Parse command line arguments
bool parse_arguments(int argc, char** argv, ZenoOptions* options) {
    // Initialize default options
    options->verbose = false;
    options->manifest_path = "manifest.yaml";
    options->input_file = NULL;
    options->output_file = NULL;
    options->run_mode = false;
    options->compile_mode = false;
    
    // No arguments provided
    if (argc < 2) {
        print_usage(argv[0]);
        return false;
    }
    
    // Check if first argument is a command
    if (strcmp(argv[1], "run") == 0) {
        options->run_mode = true;
    } else if (strcmp(argv[1], "compile") == 0) {
        options->compile_mode = true;
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_usage(argv[0]);
        return false;
    }
    
    // Parse remaining arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            options->verbose = true;
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--manifest") == 0) {
            if (i + 1 < argc) {
                options->manifest_path = argv[++i];
            } else {
                fprintf(stderr, "Missing manifest path\n");
                return false;
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                options->output_file = argv[++i];
            } else {
                fprintf(stderr, "Missing output file\n");
                return false;
            }
        } else if (argv[i][0] != '-') {
            // Assume it's the input file
            options->input_file = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return false;
        }
    }
    
    // Validate options
    if (!options->input_file && !options->run_mode && !options->compile_mode) {
        fprintf(stderr, "No input file specified\n");
        print_usage(argv[0]);
        return false;
    }
    
    return true;
}

// Print usage information
static void print_usage(const char* program_name) {
    printf("Usage: %s COMMAND [OPTIONS] [file]\n\n", program_name);
    printf("Commands:\n");
    printf("  run           Transpile and run Zeno code\n");
    printf("  compile       Transpile and compile Zeno code to a binary\n\n");
    printf("Options:\n");
    printf("  -v, --verbose       Enable verbose output\n");
    printf("  -m, --manifest PATH Specify manifest file (default: manifest.yaml)\n");
    printf("  -o, --output FILE   Specify output file\n");
}

// Simple YAML parser for manifest
ZenoManifest* load_manifest(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        // If manifest file doesn't exist, return default values
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
        manifest->compiler.flags = strdup("-Wall -Wextra");
        return manifest;
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
    manifest->compiler.flags = strdup("-Wall -Wextra");
    
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
    
    // Generate paths
    char c_output_path[1024];
    snprintf(c_output_path, sizeof(c_output_path), 
             "%s/%s.c", manifest->output.dir, 
             options->output_file ? options->output_file : "temp");
    
    char temp_exe_path[1024];
    snprintf(temp_exe_path, sizeof(temp_exe_path), 
             "%s/%s", manifest->output.dir, 
             options->output_file ? options->output_file : "temp");
    
    // Transpile the Zeno file to C
    if (options->verbose) {
        printf("Transpiling %s to %s\n", input_file, c_output_path);
    }
    
    int ret = transpile_file(input_file, c_output_path, options->verbose);
    if (ret != 0) {
        fprintf(stderr, "Transpilation failed with code %d\n", ret);
        return ret;
    }
    
    // Compile the C file to executable
    if (options->verbose) {
        printf("Compiling %s to %s\n", c_output_path, temp_exe_path);
    }
    
    char compile_cmd[2048];
    snprintf(compile_cmd, sizeof(compile_cmd), 
             "%s %s -o %s %s", 
             manifest->compiler.cc, c_output_path, temp_exe_path, manifest->compiler.flags);
    
    ret = system(compile_cmd);
    if (ret != 0) {
        fprintf(stderr, "Compilation failed with code %d\n", ret);
        return ret;
    }
    
    // Run the executable
    if (options->verbose) {
        printf("Running %s\n", temp_exe_path);
    }
    
    ret = system(temp_exe_path);
    
    // Clean up temporary files if not in verbose mode
    if (!options->verbose) {
        remove(c_output_path);
        remove(temp_exe_path);
    }
    
    return ret;
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
    
    // Generate paths
    char c_output_path[1024];
    snprintf(c_output_path, sizeof(c_output_path), 
             "%s/%s.c", manifest->output.dir, 
             options->output_file ? options->output_file : manifest->name);
    
    char exe_path[1024];
    snprintf(exe_path, sizeof(exe_path), 
             "%s/%s", manifest->output.dir, 
             options->output_file ? options->output_file : manifest->output.binary);
    
    // Transpile the Zeno file to C
    if (options->verbose) {
        printf("Transpiling %s to %s\n", input_file, c_output_path);
    }
    
    int ret = transpile_file(input_file, c_output_path, options->verbose);
    if (ret != 0) {
        fprintf(stderr, "Transpilation failed with code %d\n", ret);
        return ret;
    }
    
    // Compile the C file to executable
    if (options->verbose) {
        printf("Compiling %s to %s\n", c_output_path, exe_path);
    }
    
    char compile_cmd[2048];
    snprintf(compile_cmd, sizeof(compile_cmd), 
             "%s %s -o %s %s", 
             manifest->compiler.cc, c_output_path, exe_path, manifest->compiler.flags);
    
    ret = system(compile_cmd);
    if (ret != 0) {
        fprintf(stderr, "Compilation failed with code %d\n", ret);
        return ret;
    }
    
    if (options->verbose) {
        printf("Successfully compiled %s\n", exe_path);
    }
    
    return 0;
}
