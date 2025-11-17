# Dr. Dobb's Journal Article Reference

## Citation

**Title**: Bob: A Tiny Object-Oriented Language

**Author**: David Michael Betz

**Publication**: Dr. Dobb's Journal
**Issue**: September 1994
**Volume**: "Alternative Programming Languages"
**Pages**: Featured article with complete source listings

**Online Version**: [https://jacobfilipp.com/DrDobbs/articles/DDJ/1994/9415/9415e/9415e.htm](https://jacobfilipp.com/DrDobbs/articles/DDJ/1994/9415/9415e/9415e.htm)

## Copyright Notice

**Article**: Copyright © 1994 Dr. Dobb's Journal
**Source Code**: Copyright © 1991 David Michael Betz
All rights reserved. Published for educational use.

## Article Overview

The article presents BOB as a practical implementation of a bytecode interpreter suitable for embedding in applications. Betz designed BOB for "macro language development and language experimentation," using it as a macro facility in a theatrical lighting control system.

**Key Topics Covered:**
- Bytecode compilation architecture
- Stack-based virtual machine design
- Object-oriented implementation with inheritance
- Memory management and garbage collection
- Integration patterns for embedding

**Target Audience**: Systems programmers interested in language implementation, embedded scripting, and virtual machine design.

## Source Code

The complete source code from the article is available in this directory:
- `bob.asc` - Complete listings including interpreter, compiler, and memory manager

## Historical Significance

Published during the emergence of scripting languages and just before Java popularized bytecode VMs, the article provides insight into practical VM design for embedded use cases. BOB's architecture influenced several embedded scripting implementations in the mid-1990s, including SlipScript.

## Recommended Reading Order

1. **This README** (`README.md`) - Technical documentation of BOB architecture
2. **Original Article** (link above) - Betz's design discussion and implementation details
3. **Source Code** (`bob.asc`) - Complete implementation with comments

## Related Work

**By David Michael Betz:**
- XLISP - Experimental Lisp interpreter
- XScheme - Scheme interpreter
- Various DDJ articles on language implementation

**Contemporary Technologies (1994):**
- Perl 4 - Text processing scripting
- Python 1.1 - General-purpose scripting
- Tcl/Tk 7.3 - GUI scripting
- Java in development - Bytecode VM for WWW (announced 1995)
