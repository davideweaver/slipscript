/* bob.h - Copyright (c) 1996 David Weaver */

#ifndef BOB_H_
#define BOB_H_

#include "compiler.h"
#include "execute.h"
#include "function.h"
#include "objects.h"
#include "boberr.h"


/* BOB interface prototypes */
extern "C" void compile_file_pointer(FILE *fp);
extern "C" void compile_buffer(char *buffer);
extern "C" void cleanup( InterpreterContext *ic );


#endif