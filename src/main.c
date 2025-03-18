#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h"
#include "symtab.h"
#include "codegen/codegen.h"
#include "zeno_cli.h"

// External variables from Flex/Bison
extern FILE* yyin;
extern int yyparse();
extern AST_Node* root;

// Transpile a Zeno file to C
int transpile_file(const char* input_path, const char* output_path, bool verbose) {
    FILE* input_file = NULL;
    FILE* output_file = stdout;
    
    // Open input file
    input_file = fopen(input_path, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_path);
        return 1;
    }
    
    // Open output file if specified
    if (output_path) {
        output_file = fopen(output_path, "w");
        if (!output_file) {
            fprintf(stderr, "Error: Cannot open output file '%s'\n", output_path);
            fclose(input_file);
            return 1;
        }
    }
    
    // Set input file for Flex
    yyin = input_file;
    
    // Parse input file
    if (verbose) {
        printf("Parsing Zeno input file...\n");
    }
    
    if (yyparse() != 0) {
        fprintf(stderr, "Error: Parsing failed\n");
        fclose(input_file);
        if (output_file != stdout) {
            fclose(output_file);
        }
        return 1;
    }
    
    // Check if AST was built successfully
    if (!root) {
        fprintf(stderr, "Error: AST generation failed\n");
        fclose(input_file);
        if (output_file != stdout) {
            fclose(output_file);
        }
        return 1;
    }
    
    // Generate code
    if (verbose) {
        printf("Generating C code...\n");
    }
    
    CodeGenContext* ctx = init_codegen(output_file);
    generate_code(ctx, root);
    cleanup_codegen(ctx);
    
    // Clean up
    free_ast(root);
    fclose(input_file);
    if (output_file != stdout) {
        fclose(output_file);
    }
    
    if (verbose) {
        printf("Zeno transpilation completed successfully!\n");
    }
    
    return 0;
}

void print_usage(const char* program_name) {
    printf("Zeno Language v0.1.0 - Usage:\n\n");
    printf("  %s COMMAND [OPTIONS] [FILE]\n\n", program_name);
    printf("Commands:\n");
    printf("  transpile <input> [output]  Convert Zeno code to C\n");
    printf("  run [OPTIONS] [file]        Transpile, compile, and run Zeno code\n");
    printf("  compile [OPTIONS] [file]    Transpile and compile Zeno code to a binary\n");
    printf("  init [directory]            Create a default manifest.yaml file\n\n");
    printf("Options:\n");
    printf("  -v, --verbose       Enable verbose output\n");
    printf("  -m, --manifest PATH Specify manifest file (default: manifest.yaml)\n");
    printf("  -o, --output FILE   Specify output file\n");
}

int main(int argc, char** argv) {
    // Check for minimum arguments
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Handle different commands
    if (strcmp(argv[1], "transpile") == 0) {
        // Transpile mode
        if (argc < 3) {
            fprintf(stderr, "Error: Missing input file for transpile command\n");
            print_usage(argv[0]);
            return 1;
        }
        
        const char* input_file = argv[2];
        const char* output_file = (argc > 3) ? argv[3] : NULL;
        
        return transpile_file(input_file, output_file, true);
    } 
    else if (strcmp(argv[1], "init") == 0) {
        // Init mode - create a default manifest.yaml
        const char* dir_path = (argc > 2) ? argv[2] : NULL;
        return init_zeno_project(dir_path, true);
    }
    else if (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "compile") == 0) {
        // CLI mode (run or compile)
        ZenoOptions options;
        options.verbose = false;
        options.manifest_path = "manifest.yaml";
        options.input_file = NULL;
        options.output_file = NULL;
        options.run_mode = (strcmp(argv[1], "run") == 0);
        options.compile_mode = (strcmp(argv[1], "compile") == 0);
        
        // Parse remaining arguments
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                options.verbose = true;
            } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--manifest") == 0) {
                if (i + 1 < argc) {
                    options.manifest_path = argv[++i];
                } else {
                    fprintf(stderr, "Missing manifest path\n");
                    return 1;
                }
            } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
                if (i + 1 < argc) {
                    options.output_file = argv[++i];
                } else {
                    fprintf(stderr, "Missing output file\n");
                    return 1;
                }
            } else if (argv[i][0] != '-') {
                // Assume it's the input file
                options.input_file = argv[i];
            } else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                print_usage(argv[0]);
                return 1;
            }
        }

        // Load manifest - don't allow missing manifest for run/compile commands
        ZenoManifest* manifest = load_manifest(options.manifest_path, false);
        if (!manifest) {
            return 1; // Error message already printed by load_manifest
        }
        
        if (options.verbose) {
            printf("Zeno Compiler v0.1.0\n");
            printf("Project: %s (%s)\n", manifest->name, manifest->version);
            printf("Output directory: %s\n", manifest->output.dir);
            printf("Binary name: %s\n", manifest->output.binary);
            printf("Compiler: %s\n", manifest->compiler.cc);
            printf("Compiler flags: %s\n", manifest->compiler.flags);
            
            if (manifest->source.main) {
                printf("Main source file: %s\n", manifest->source.main);
            }
            
            printf("Include directories:\n");
            for (int i = 0; i < manifest->source.include_count; i++) {
                printf("  - %s\n", manifest->source.include[i]);
            }
            
            printf("\n");
        }
        
        int result = 0;
        
        // Execute command
        if (options.run_mode) {
            result = run_zeno_file(&options, manifest);
        } else if (options.compile_mode) {
            result = compile_zeno_file(&options, manifest);
        }
        
        // Clean up
        free_manifest(manifest);
        
        return result;
    } 
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    } 
    else {
        // Unknown command
        fprintf(stderr, "Error: Unknown command '%s'\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
}
