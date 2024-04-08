# C Compiler

Compilation - 20364

## Introduction

This project is a compiler for a custom programming language implemented in C called CPL. The compiler consists of multiple components, including a lexer, parser, and quad code generator. It translates source code written in CPL into intermediate language Quad.

## File Structure

The project consists of the following source files:

- cpq.c: The main file responsible for orchestrating the compilation process. It interacts with the lexer, parser, and quad code generator.
- quad1.c and quad2.c: Files containing the implementation of the quad code generator, including functions for generating quadruple instructions and managing the quadruple table.
- lexer.l and parser.y: Files containing the lexer and parser definitions using Flex and Bison, respectively.
- quad1.h and quad2.h: Header files, which contain function prototypes and structure definitions used throughout the project.
- makefile: Automates the compilation process.

## Compilation and Execution

To compile the project using the Makefile, simply navigate to the project directory in your terminal and run the following command:

```bash
make
```
This will compile all source files and generate the executable file named cpq.

To execute the compiler, use the following command:

```bash
./cpq <input_file>.c
```

## Input

The input is a single file written in CPL (.ou).

## Output

After successful execution, the compiler generates a single file containing the generated Quad code (.qud).

## Error Handling

The compiler performs error checking during lexical analysis, parsing, and code generation stages. Errors, if encountered, are reported to the console with details about the file, line number, and nature of the error.
