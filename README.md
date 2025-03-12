# Zeno Language

A transpiler for Zeno, a C-based language with Elixir-inspired syntax and features.

## Language Features

- C-compatible core with modern syntax
- Type system with inference
- Pattern matching with `match` expressions
- Struct composition with `with` clause
- Spread operator (`...`) for structs and arrays
- Function guard clauses with `where`
- Pipe operator (`|>`) for function chaining
- Immutable variables with `const`

## Building the Transpiler

Prerequisites:
- GCC or compatible C compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make

To build:

```bash
make
```

This will create the `elixir-c-transpiler` binary in the `bin` directory.

## Usage

Zeno provides a single command-line tool with three main commands:

```bash
zeno transpile <input> [output]   # Convert Zeno code to C
zeno run [OPTIONS] [file]         # Transpile, compile, and run Zeno code
zeno compile [OPTIONS] [file]     # Transpile and compile Zeno code to a binary
```

Options:
- `-v, --verbose` - Enable verbose output
- `-m, --manifest PATH` - Specify manifest file (default: manifest.yaml)
- `-o, --output FILE` - Specify output file

### Project Configuration

Zeno projects can be configured using a `manifest.yaml` file:

```yaml
# Zeno project configuration
name: "my_project"
version: "0.1.0"
output:
  # Output directory for generated C files and binaries
  dir: "./build"
  # Binary name (defaults to project name if not specified)
  binary: "my_project"
source:
  # Main source file
  main: "src/main.zeno"
  # Include directories
  include:
    - "src"
    - "lib"
compiler:
  # C compiler to use
  cc: "gcc"
  # Compiler flags
  flags: "-Wall -Wextra -O2"
```

## Examples

Run the examples:

```bash
# Compile and run the transpiler on the examples
make test

# Compile the generated C code
gcc -o hello examples/hello.c
gcc -o structs examples/structs.c

# Run the compiled examples
./hello
./structs
```

## Project Structure

- `src/` - Source code for the transpiler
  - `lexer.l` - Flex lexer specification
  - `parser.y` - Bison parser specification
  - `ast.h/c` - Abstract Syntax Tree definitions
  - `symtab.h/c` - Symbol table for tracking variables
  - `codegen.h/c` - Code generation to C
  - `main.c` - Main program entry point
- `examples/` - Example programs
- `obj/` - Object files (created during build)
- `bin/` - Binary executables (created during build)

## Language Syntax Reference

### Variable Declaration

```
// Mutable variable
let x = 0;
let y: int = 42;

// Immutable variable
const PI = 3.14159;
const NAME: string = "Alice";
```

### Functions

```
// Basic function
fn add(a: int, b: int): int {
  return a + b;
}

// Arrow function for expressions
fn multiply(a: int, b: int): int => a * b;

// Function with guard clause
fn factorial(n: int): int where n > 0 {
  return n * factorial(n - 1);
}

fn factorial(n: int): int where n == 0 {
  return 1;
}
```

### Structs and Composition

```
// Basic struct
struct Point {
  x: int,
  y: int
}

// Struct composition
struct Entity {
  id: string
}

struct Timestamps {
  created_at: int,
  updated_at: int
}

struct User with Entity, Timestamps {
  name: string,
  email: string
}

// Creating instances
let user: User = {
  id: "user-123",
  name: "Alice",
  email: "alice@example.com",
  created_at: 1678234511,
  updated_at: 1678234511
};

// With spread operator
let base = {id: "user-123"};
let timestamps = {created_at: 1678234511, updated_at: 1678234511};
let user = {
  ...base,
  ...timestamps,
  name: "Alice",
  email: "alice@example.com"
};
```

### Pattern Matching

```
match value {
  0 => printf("Zero\n"),
  n: int where n > 0 => printf("Positive: %d\n", n),
  n: int where n < 0 => printf("Negative: %d\n", n),
  "hello" => printf("Greeting\n"),
  _ => printf("Something else\n")
}
```

### Pipe Operator

```
// Instead of nested function calls
let result = format(process(parse(data)));

// Use the pipe operator
let result = data |> parse |> process |> format;
```

## License

MIT