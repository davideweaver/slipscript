/* function.h - built-in function definitions */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

/*
	These macros depend on the c, n and p parameters of the built-in
	function declaration:
	
	ObjectPtr BIF_xxx(InterpreterContext *c,int n,ObjectPtr *p) {}
*/

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include "execute.h"

#define CheckArgCount(m)	CheckArgRange(m,m)

#define CheckArgMin(m)		do { if (n < (m)) TooFewArguments(c); } while (0)

#define CheckArgRange(mn,mx)	do { if (n < (mn)) TooFewArguments(c); else if (n > (mx)) TooManyArguments(c); } while (0)

#define CheckType(n,tp)		do { if (!tp(p[-n])) TypeError(c,p[-n]); } while (0)

#define GetArg(n)		(p[-n])

#define DebugName(name)		c->err = ( char * )name



/* prototypes */
void EXPORT EnterLibrarySymbols(InterpreterContext *c);
CMethodHandlerPtr EXPORT FindLibraryFunctionHandler(char *name);

#endif
