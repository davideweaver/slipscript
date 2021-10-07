/* init.cpp - interface to Delaware Interpreter */

#include <stdio.h>
#include <string.h>

extern "C" {
	#include "bob.h"
}

/* callbacks must load ds on entry */
#ifdef _WINDOWS
#define CALLBACK	__loadds
#else
#define CALLBACK
#endif

/* console stream structure */
typedef struct {
  StreamIODispatch *d;
} ConsoleStream;

/* CloseConsoleStream - console stream close handler */
static int CALLBACK CloseConsoleStream(Stream *s)
{
    return 0;
}

/* ConsoleStreamGetC - console stream getc handler */
static int CALLBACK ConsoleStreamGetC(Stream *s)
{
    return getchar();
}

/* ConsoleStreamPutC - console stream putc handler */
static int CALLBACK ConsoleStreamPutC(void *c,int ch,Stream *s)
{
    return putchar(ch);
}

/* dispatch structure for null streams */
StreamIODispatch consoleIODispatch = {
  CloseConsoleStream,
  ConsoleStreamGetC,
  ConsoleStreamPutC
};

/* console stream */
ConsoleStream consoleStream = { &consoleIODispatch };

/* ErrorHandler - error handler callback */
void CALLBACK ErrorHandler(InterpreterContext *c,int code,void *data)
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
    	//exit(1);
    }
}

void compile_file_pointer(FILE *fp)
{
    InterpreterContext *ic;
    CompilerContext *c;
    ObjectPtr val;
	Stream *is;
	
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

	is = OpenFilePointerStream(fp);
    CompileFile(c,(Stream *)is,ic->standardOutput);
    CloseStream((Stream *)is);
}

void compile_buffer(char *buffer)
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

	free( s );

    free( ic->oldSpace );
    free( ic->newSpace );
    free( ic->stack );
	free( ic->protectedPointers );
	free( ic );

	free( c->codebuf );
	free( c->literalbuf );
	free( c );
}

