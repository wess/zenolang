# Zeno Language Specification Outline

## 1. Introduction

*   **Philosophy**: Zeno is a simple, LLVM-based, compiled language. It aims for simplicity and readability, drawing syntax inspiration from C and Elixir.
*   **Key Features**:
    *   Statically typed.
    *   No Object-Oriented Programming (OOP).
    *   Built-in garbage collection (mentioned in `basics.zn` comment, needs verification).
    *   Async/await support.
    *   Struct composition.

## 2. Syntax Basics

*   **Comments**:
    *   Single-line: `// comment text`
    *   Multi-line: `/* comment text */`
*   **Entry Point**: Typically `fn main(): int { ... }`. Can also be `async fn main() { ... }`.
*   **Imports**:
    *   Import C headers: `import "header.h";` (e.g., `import "stdio.h";`)
    *   Import Zeno modules: `import { identifier1, identifier2 } from "/path/to/file.zn";`
*   **Scope & Visibility**:
    *   Identifiers (variables, functions, structs) are file-private by default.
    *   Use the `public` keyword to make identifiers accessible outside the defining file (e.g., `public let x: int = 10;`).

## 3. Data Types

*   **Primitive Types**:
    *   `int`: Integer numbers.
    *   `string`: Text strings.
    *   `bool`: Boolean values (`true`, `false`).
    *   `number`: General number type (usage seen, relation to `int` needs clarification).
    *   `void`: Represents no return value for functions.
*   **Collections**:
    *   **Arrays**:
        *   Fixed-size style: `let name: type[] = [value1, value2];` (e.g., `let b: string[] = ["a", "b"];`)
        *   Generic style: `let name: array<type> = [value1, value2];` (e.g., `let arr: array<int> = [1, 2];`)
    *   **Maps (Hashes)**:
        *   Inline literal style: `let name: { key1: type1, key2: type2 } = { key1: value1, key2: value2 };` (e.g., `let f: { name: string, age: number } = { name: "Jane", age: 25 };`)
        *   Generic map type: `const name: map<key_type, value_type> = {key1: value1, key2: value2};` (e.g., `const myMap: map<string, string> = {"name": "Zeno"};`)
        *   *Note*: Map access and iteration were noted as potentially unimplemented in `maps.zn`.

## 4. Variables

*   **Declaration**:
    *   Mutable: `let name: type = value;`
    *   Immutable: `const name: type = value;`
*   **Type Annotation**: Explicit type annotation (`: type`) is generally used.

## 5. Operators

*   **Arithmetic**: `+`, `*` (others likely exist, e.g., `-`, `/`)
*   **Comparison**: `<`, `==`, `>` (others likely exist, e.g., `<=`, `>=`, `!=`)
*   **Modulo**: `%`
*   **Spread**: `...`
    *   Used to combine arrays: `let combined: type[] = [...arr1, ...arr2];`
    *   Used to combine hashes/struct literals: `let combined: { ... } = { ...hash1, ...hash2, new_field: value };`
*   **Pipe**: `|>`
    *   Chains function calls: `value |> func1 |> func2;` is equivalent to `func2(func1(value));`

## 6. Control Flow

*   **Conditional**: `if (condition) { ... } else if (condition) { ... } else { ... }`
*   **Pattern Matching**: `match value { pattern1 => expression1, pattern2 where guard => expression2, _ => default_expression }`
    *   Supports matching values, types (`n: int`), and guards (`where n > 0`).
    *   Can have block expressions (`{ ... }`) as results.
    *   `_` is the wildcard/default case.
*   **Loops**:
    *   `while (condition) { ... }`
    *   C-style `for`: `for (let i = 0; i < limit; i = i + 1) { ... }`
    *   Range-based `for`: `for i in start..end { ... }` (inclusive start, exclusive end?)
    *   Array iteration: `for item in array { ... }`
    *   Hash/Map iteration: `for {key, value} in map { ... }`

## 7. Functions

*   **Standard Definition**: `fn name(param1: type1, param2: type2): return_type { body }`
*   **Arrow Syntax (Single Expression)**: `fn name(params): return_type => expression;`
*   **Anonymous Functions (Lambdas)**:
    *   Single expression: `let name = (params): return_type => expression;`
    *   Block body: `let name = (params): return_type => { body };`
*   **Guard Clauses**: Functions can be overloaded with `where` clauses for conditional execution based on parameter values:
    ```zeno
    fn factorial(n: int): int where n > 0 { ... }
    fn factorial(n: int): int where n == 0 { ... }
    ```
*   **Async Functions**:
    *   Definition: `async fn name(params): return_type { ... }`
    *   Calling: Use `await` within another `async` function: `let result = await async_func();`
    *   Anonymous Async: `let name = async (params): return_type => { body };`
*   **Promises**: Async functions appear to return promises (or similar awaitables).
    *   Chaining: `promise.then((result) => ...).catch((error) => ...).finally(() => ...)`
    *   Waiting for multiple: `let results = await Promise.all([promise1, promise2]);`

## 8. Structs

*   **Definition**: `struct Name { field1: type1, field2: type2 }`
*   **Composition (Mixins)**: Combine multiple structs: `struct CombinedName with Struct1, Struct2 { additional_field: type }`
    *   Fields from composed structs are included.
*   **Instantiation**: Use struct literals (similar to map/hash literals): `let instance: Name = { field1: value1, ... };`
    *   Spread operator (`...`) can be used during instantiation.
*   **Field Access**: `instance.field_name`

## 9. Standard Library / Built-ins (Inferred)

*   **I/O**:
    *   `printf(format_string, args...)`: Formatted printing (requires `import "stdio.h";`).
    *   `print(value)`: Simpler printing (usage seen, definition source unclear).
*   **Async/Concurrency**:
    *   `fetch(url)`: Appears to be a built-in for network requests (used with `await`).
    *   `Promise.all([...])`: Waits for multiple promises.
*   **Array Properties**:
    *   `.length`: Gets the length of an array (e.g., `zzz.length`).
