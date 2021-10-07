/* test.c - sample program using the Bob DLL */

#include <stdio.h>
#include <string.h>
#include "bob.h"

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
static int CALLBACK ConsoleStreamPutC(InterpreterContext *c, int ch,Stream *s)
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
        printf("Error: %s\n",GetErrorText(code));
    	break;
    }
}

/* main - the main routine */
void main(int argc,char **argv)
{
    InterpreterContext *ic;
    CompilerContext *c;
    char lineBuffer[256];
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
    
    /* read/eval/print loop */
    for (;;) {
        printf("\nExpr> "); fflush(stdout);
        if (gets(lineBuffer)) {
            Stream *s = CreateStringStream(lineBuffer,strlen(lineBuffer));
            if (s) {
                val = CompileExpr(c,s);
                val = CallFunction(ic,val,0);
                printf("Value: ");
                PrintValue(ic,val,ic->standardOutput);
                CloseStream(s);
            }
        }
        else
            break;
    }
}


