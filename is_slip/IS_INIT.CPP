/* is_init.cpp - interface to Delaware Interpreter */

#include <stdio.h>
#include <string.h>

extern "C" {
	#include "../src/interp/bob.h"
}

/*#ifdef _WINDOWS
#define CALLBACK	__loadds
#else
#define CALLBACK
#endif*/

typedef struct {
  StreamIODispatch *d;
} ConsoleStream;

static int CloseConsoleStream(Stream *s)
{
    return 0;
}

static int ConsoleStreamGetC(Stream *s)
{
    return getchar();
}

static int ConsoleStreamPutC(int ch,Stream *s)
{
    return putchar(ch);
}

StreamIODispatch consoleIODispatch = {
  CloseConsoleStream,
  ConsoleStreamGetC,
  ConsoleStreamPutC
};

ConsoleStream consoleStream = { &consoleIODispatch };

void ErrorHandler(InterpreterContext *c,int code,void *data)
{
    switch (code) {
    case ecExit:
        exit(1);
    default:
		printf("\nError: %s",GetErrorText(code));
		if(code == 6) {
			printf(" - ");
			DisplayValue(c,data,(Stream *)&consoleStream);
		}
		printf("\n");
    	exit(1);
    }
}

void compile_file(char *fname)
{
    InterpreterContext *ic;
    CompilerContext *c;
    ObjectPtr val;
	
	/* create an interpreter context (heap) */
    ic = NewInterpreterContext(16384,1024);
     
    /* setup the error handler */
    ic->errorHandler = ErrorHandler;
    
    /* setup standard i/o */
    ic->standardInput = (Stream *)&consoleStream;
    ic->standardOutput = (Stream *)&consoleStream;
    ic->standardError = (Stream *)&consoleStream;
    
    /* add the library functions to the symbol table */
    EnterLibrarySymbols(ic);
    ic->findFunctionHandler = FindLibraryFunctionHandler;
    
    /* create a compiler context */
    c = InitCompiler(ic,4096,256);
    
    /* protect a pointer from the garbage collector */
    ProtectPointer(ic,&val);

	LoadFile(c, fname, ic->standardOutput);
}

extern "C" void compile_buffer(char *buffer)
{
    InterpreterContext *ic;
    CompilerContext *c;
    ObjectPtr val;
	Stream *s;
	
	/* create an interpreter context (heap) */
    ic = NewInterpreterContext(16384,1024);
     
    /* setup the error handler */
    ic->errorHandler = ErrorHandler;
    
    /* setup standard i/o */
    ic->standardInput = (Stream *)&consoleStream;
    ic->standardOutput = (Stream *)&consoleStream;
    ic->standardError = (Stream *)&consoleStream;
    
    /* add the library functions to the symbol table */
    EnterLibrarySymbols(ic);
    ic->findFunctionHandler = FindLibraryFunctionHandler;
    
    /* create a compiler context */
    c = InitCompiler(ic,4096,256);
    
    /* protect a pointer from the garbage collector */
    ProtectPointer(ic,&val);

	s = CreateStringStream((unsigned char*)buffer, strlen(buffer));
	CompileFile(c, s, ic->standardOutput);
}
