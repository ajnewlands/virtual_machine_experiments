# Abel Assembler Specification

## Overview

`abel-asm` is a macro assembler targeting the Abel VM. It reads assembly source files and emits Abel bytecode blobs suitable for execution by `libabel`.

## Build

- C++17
- CMake (minimum 3.28)
- Links against system flex and bison for lexing/parsing
- Links against `libabel` for bytecode emission
- Uses system libboost (Boost.ProgramOptions) for command line parsing
- Unit tests use system libboost (Boost.Test) where appropriate

## Command Line Interface

Command line arguments are parsed using Boost.ProgramOptions.

| Option | Required | Description |
|--------|----------|-------------|
| `--help` | No | Print usage information and exit |
| `--input <file>` | Yes | Path to the input assembly file |

## Input Format

Source files use NASM-style syntax with the following top-level structure:

```
<constant definitions>

section .data
    <data declarations>

section .text
    <function definitions>
```

### Constants

- Defined at the top of the file, before any section, using NASM `EQU` syntax: `<NAME> EQU <value>`.
- Constants are substituted wherever their name appears as an immediate operand.
- Names are conventionally uppercase but this is not enforced.

### Sections

- `section .data` — reserved for future data segment declarations (currently unused by the VM but must be accepted by the parser).
- `section .text` — contains all function definitions.
- Both sections are optional; if omitted, the file is treated as if all content is in `section .text`.

### Functions

Defined within `section .text`:

```
function <name>(<args>):
    <variable declarations>
    <instructions>
end
```

- Declared with `function <name>(<arg1>, <arg2>, ...):` and terminated with `end`.
- The argument list defines both the arity and the named argument slots.
- An empty argument list `function <name>():` denotes arity 0.
- Arguments occupy stack slots 0..argc-1 upon entry and are referenced via `@<name>`.
- A function named `_main` is required and serves as the program entrypoint. It must have arity 0.

### Variable Declarations

- Declared with `var <name>[, <name>...]` at the top of a function body, before any instructions.
- Multiple names can be declared on one line, separated by commas.
- Each variable reserves one stack slot (u64).
- Variables are assigned slot indices sequentially after the argument slots.
- Referenced in instructions using `@<name>` notation, which resolves to the slot index.

### Instructions

| Mnemonic | Operand | Opcode | Description |
|----------|---------|--------|-------------|
| `push <imm64\|CONST>` | 64-bit integer literal or constant name | OP_PUSH | Push immediate value onto stack |
| `pop` | — | OP_POP | Pop and discard top of stack |
| `add` | — | OP_ADD | Pop two, push sum |
| `load @<name>` | slot index (u8) | OP_LOAD | Push value of slot onto stack |
| `store @<name>` | slot index (u8) | OP_STORE | Pop top into slot |
| `call <name>` | blob index (u64) | OP_CALL | Call named function |
| `ret` | — | OP_RETURN | Return from function |

### Literals

- Decimal integers: `42`, `0`, `100`
- Hex integers: `0xFF`, `0x1A`

### Comments

- Line comments begin with `;`

## Assembly Process

1. **Parse** — flex/bison tokenise and parse the source into an AST.
2. **Resolve** — Assign blob indices to functions in declaration order. Assign slot indices to args and vars per function. Resolve `@name` references to slot indices and `call <name>` references to blob indices.
3. **Emit** — Generate `Bytecode` objects (instruction stream + argc) for each function. Variable declarations emit `push 0` preamble instructions to reserve stack slots.
4. **Output** — Serialize the bytecode blobs to a binary format loadable by `libabel`.

## Errors

- Undefined variable reference (`@name` where `name` is not declared)
- Undefined function reference (`call name` where `name` is not declared)
- Undefined constant reference
- Missing `_main` function
- Duplicate function, variable, or constant names
- Variable declarations after instructions within a function
- Constant defined after a section directive

## Example: add3 program

```
; Equivalent to the hardcoded add3 program in abel's main.cpp

A EQU 4
B EQU 3
C EQU 2
D EQU 1

section .data

section .text

function add3(a, b, c, d):
    load @a
    load @b
    add
    load @c
    add
    load @d
    add
    ret
end

function _main():
    var a, b, c, d
    push A
    store @a
    push B
    store @b
    push C
    store @c
    push D
    store @d
    load @a
    load @b
    load @c
    load @d
    call add3
    pop
    ret
end
```

### Explanation

- `A EQU 4` etc. define compile-time constants. `push A` is equivalent to `push 4`.
- `section .data` is present but empty (reserved for future use).
- `section .text` contains the function definitions.

In `_main`:
- Four variables `a`, `b`, `c`, `d` are declared, reserving slots 0–3.
- Constants A–D are stored into those slots, then loaded back onto the working stack before calling `add3`.
- `call add3` slices 4 elements off the stack (because add3 has 4 parameters) into a new frame.

In `add3`:
- The parameter list `(a, b, c, d)` names the 4 argument slots (0–3) populated by the caller.
- The function loads each argument and sums them, leaving the result on the stack top for return.
