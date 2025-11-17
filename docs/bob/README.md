# BOB Interpreter

BOB (a tiny object-oriented language) is the bytecode interpreter that powers SlipScript. Created by David Michael Betz and published in Dr. Dobb's Journal, September 1994.

## Overview

BOB is a lightweight, dynamically-typed object-oriented language with C-like syntax, designed for embedding as a macro language in larger applications. It implements a hybrid compiler-interpreter architecture optimized for repeated execution.

## Architecture

### Execution Model

**Two-Phase Execution:**
1. **Compilation** - Functions compile to bytecode at definition time (once)
2. **Interpretation** - Bytecode executes through virtual machine (many times)

This avoids repeated parsing overhead while maintaining the flexibility of interpretation.

### Virtual Machine

**Stack-Based Architecture:**
- **Runtime Stack** - Holds local variables, arguments, and intermediate values
- **Program Counter (PC)** - Tracks current bytecode instruction
- **Frame Pointer (FP)** - Marks current function activation record
- **Stack Pointer (SP)** - Points to top of stack

**Components:**
```
┌─────────────────────────────────────┐
│     Source Code (C-like syntax)     │
└──────────────┬──────────────────────┘
               │ Compilation
               ▼
┌─────────────────────────────────────┐
│   Bytecode (Op codes + operands)    │
└──────────────┬──────────────────────┘
               │ Interpretation
               ▼
┌─────────────────────────────────────┐
│    Virtual Machine (Stack-based)    │
│  • Stack operations                 │
│  • Method dispatch                  │
│  • Memory management                │
└─────────────────────────────────────┘
```

### Data Types

**Core Types:**
- `DT_NIL` - Null/undefined value
- `DT_INTEGER` - Integer values
- `DT_STRING` - String data
- `DT_CLASS` - Class definitions
- `DT_OBJECT` - Object instances
- `DT_VECTOR` - Array/vector structures
- `DT_CODE` - C function pointers (native code)
- `DT_BYTECODE` - Compiled bytecode functions

### Bytecode Instructions

**Key Opcodes:**
- `OP_CALL` - Function invocation (C or bytecode)
- `OP_RETURN` - Function return
- `OP_PUSH` - Push constant to stack
- `OP_LOAD` - Load variable value
- `OP_STORE` - Store to variable
- `OP_SEND` - Method call (message send)
- `OP_BRT/BRF` - Conditional branching
- `OP_BR` - Unconditional branch
- Arithmetic: `OP_ADD`, `OP_SUB`, `OP_MUL`, `OP_DIV`, `OP_REM`
- Comparison: `OP_LT`, `OP_LE`, `OP_EQ`, `OP_NE`, `OP_GE`, `OP_GT`

### Object System

**Class-Based OOP:**
- Classes define object structure and methods
- Single inheritance with method lookup chain
- Dynamic method dispatch through class dictionaries
- Instance variables stored in object vectors

**Method Dispatch:**
```c
// Lookup method in class hierarchy
1. Check object's class dictionary
2. If not found, check superclass
3. Repeat until found or reach root
4. Invoke method with object as context
```

### Memory Management

**Compacting Garbage Collector:**
- Scans stack and heap for live references
- Compacts memory to eliminate fragmentation
- Updates all pointers after compaction
- Triggered when allocation fails

**Memory Layout:**
```
HEAP: [Used Blocks][Free Space]
                   ↑
                heap pointer
```

## Language Features

**Syntax Characteristics:**
- C-like expression syntax
- Dynamic typing (no declarations)
- First-class functions
- Object-oriented (classes, inheritance, methods)
- Lexical scoping
- Closures (functions capture environment)

**Example BOB Code:**
```bob
// Define a simple counter class
class Counter {
    var count;

    method init() {
        count = 0;
    }

    method increment() {
        count = count + 1;
    }

    method value() {
        return count;
    }
}

// Create and use counter
c = Counter.new();
c.init();
c.increment();
c.increment();
print(c.value());  // Prints: 2
```

## Integration with SlipScript

**How SlipScript Uses BOB:**

1. **Script Parsing** - SlipScript extracts `<@ ... @>` blocks from HTML
2. **Compilation** - BOB compiler converts scripts to bytecode
3. **Execution** - BOB VM interprets bytecode
4. **Output** - Results written back to HTML stream

**SlipScript Extensions:**
- CGI/HTTP environment access (`getValue()`, form data)
- HTML output functions (`write()`)
- ODBC database connectivity
- File I/O operations
- Windows-specific APIs

## Performance Characteristics

**Optimizations:**
- Compile-once, execute-many (no parse overhead)
- Stack-based VM (efficient for expressions)
- Bytecode density (compact representation)
- Native function integration (C functions callable from BOB)

**Tradeoffs:**
- Faster than pure interpretation (pre-compiled)
- Slower than native code (VM overhead)
- Good balance for embedded scripting use case

## Design Philosophy

**Goals:**
- **Lightweight** - Small footprint for embedding
- **Extensible** - Easy to add native functions
- **Familiar** - C-like syntax reduces learning curve
- **Dynamic** - No type declarations or compilation step
- **Object-Oriented** - Modern paradigm for mid-90s

**Use Cases:**
- Macro languages in applications
- Configuration scripting
- Automation tasks
- Embedded control systems
- Educational tool for VM implementation

## Historical Context

**1994 Technology Landscape:**
- Scripting languages emerging (Perl, Python, Tcl)
- OOP gaining mainstream adoption (C++, Smalltalk)
- Bytecode VMs proving effective (Pascal P-code, Java on horizon)
- Embedded scripting becoming common pattern

BOB represented a practical implementation of these ideas in a compact, teachable form factor - perfect for magazine publication and educational use.

## Source Code

The complete BOB interpreter source code from the Dr. Dobb's Journal article is included in this directory:
- `bob.asc` - Complete source listings (669 lines)

**Files Include:**
- `bobint.c` - Bytecode interpreter
- `bobcomp.c` - Compiler
- `bobmem.c` - Memory management
- `bob.h` - Header definitions
- Example programs

## Reference

**Original Article:**
- **Title**: "Bob: A Tiny Object-Oriented Language"
- **Author**: David Michael Betz
- **Publication**: Dr. Dobb's Journal, September 1994
- **Volume**: "Alternative Programming Languages"
- **Online**: [Article Link](https://jacobfilipp.com/DrDobbs/articles/DDJ/1994/9415/9415e/9415e.htm)

**Copyright:**
- BOB interpreter: Copyright © 1991 David Michael Betz
- Article: Copyright © 1994 Dr. Dobb's Journal
- Published for educational use

## Further Reading

For detailed implementation discussion, algorithm explanations, and design rationale, refer to the original Dr. Dobb's Journal article linked above.
