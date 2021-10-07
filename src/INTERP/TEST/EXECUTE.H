/* execute.h - interpreter definitions */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#ifndef __EXECUTE_H__
#define __EXECUTE_H__

/* export keyword for library entry points */
#ifndef EXPORT
#ifdef _WINDLL
#define EXPORT __export
#else
#define EXPORT
#endif
#endif

#include <stdarg.h>
#include <setjmp.h>
#include "objects.h"

/* opcodes */
#define OP_BRT		0x01	/* branch on true */
#define OP_BRF		0x02	/* branch on false */
#define OP_BR		0x03	/* branch unconditionally */
#define OP_T		0x04	/* load top of stack with t */
#define OP_NIL		0x05	/* load top of stack with nil */
#define OP_PUSH		0x06	/* push nil onto stack */
#define OP_NOT		0x07	/* logical negate top of stack */
#define OP_ADD		0x08	/* add two numeric expressions */
#define OP_SUB		0x09	/* subtract two numeric expressions */
#define OP_MUL		0x0A	/* multiply two numeric expressions */
#define OP_DIV		0x0B	/* divide two numeric expressions */
#define OP_REM		0x0C	/* remainder of two numeric expressions */
#define OP_BAND		0x0D	/* bitwise and of top two stack entries */
#define OP_BOR		0x0E	/* bitwise or of top two stack entries */
#define OP_XOR		0x0F	/* bitwise xor of top two stack entries */
#define OP_BNOT		0x10	/* bitwise not of top two stack entries */
#define OP_SHL		0x11	/* shift left top two stack entries */
#define OP_SHR		0x12	/* shift right top two stack entries */
#define OP_LT		0x13	/* less than */
#define OP_LE		0x14	/* less than or equal */
#define OP_EQ		0x15	/* equal */
#define OP_NE		0x16	/* not equal */
#define OP_GE		0x17	/* greater than or equal */
#define OP_GT		0x18	/* greater than */
#define OP_LIT		0x19	/* load literal */
#define OP_GREF		0x1A	/* load a global variable value */
#define OP_GSET		0x1B	/* set the value of a global variable */
#define OP_GETP		0x1C	/* get the value of an object property */
#define OP_SETP		0x1D	/* set the value of an object property and return the value */
#define OP_RETURN	0x1E	/* return from interpreter */
#define OP_CALL		0x1F	/* call a function */
#define OP_EREF		0x20	/* load an environment value */
#define OP_ESET		0x21	/* set an environment value */
#define OP_FRAME	0x22	/* push an environment frame */
#define OP_UNFRAME	0x23	/* pop an environment frame */
#define OP_VREF		0x24	/* get an element of a vector */
#define OP_VSET		0x25	/* set an element of a vector */
#define OP_NEG		0x26	/* negate top of stack */
#define OP_INC		0x27	/* increment */
#define OP_DEC		0x28	/* decrement */
#define OP_DUP		0x29	/* duplicate top element on the stack */
#define OP_DUP2		0x2A	/* duplicate top two elements on the stack */
#define OP_DROP		0x2B	/* drop the top entry from the stack */
#define OP_NEWOBJECT	0x2C	/* create a new object */
#define OP_NEWLIST	0x2D	/* create a new list */
#define OP_NEWVECTOR	0x2E	/* create a new vector */
#define OP_AFRAME	0x2F	/* create an argument frame */
#define OP_AFRAMER	0x30	/* create an argument frame with rest argument */
#define OP_CLOSE	0x31	/* create a closure */

/* text file open modes */
#ifndef TEXT_READ
#define TEXT_READ	"r"
#define TEXT_UPDATE	"r+"
#define TEXT_WRITE	"w"
#define TEXT_CUPDATE	"w+"
#endif

/* binary file open modes */
#ifndef BINARY_READ
#define BINARY_READ	"rb"
#define BINARY_UPDATE	"r+b"
#define BINARY_WRITE	"wb"
#define BINARY_CUPDATE	"w+b"
#endif

/* global functions */
void EXPORT EnterBuiltInFunction(InterpreterContext *c,char *name,CMethodHandlerPtr handler);
ObjectPtr EXPORT CallFunction(InterpreterContext *c,ObjectPtr fun,int argc,...);
ObjectPtr EXPORT CallFunctionByName(InterpreterContext *c,char *fname,int argc,...);
void EXPORT AbortInterpreter(InterpreterContext *c);
int EXPORT CompareObjects(InterpreterContext *c,ObjectPtr obj1,ObjectPtr obj2);
int EXPORT PrintValue(InterpreterContext *c,ObjectPtr val,Stream *s);
int EXPORT DisplayValue(InterpreterContext *c,ObjectPtr val,Stream *s);
void EXPORT TooManyArguments(InterpreterContext *c);
void EXPORT TooFewArguments(InterpreterContext *c);
void EXPORT TypeError(InterpreterContext *c,ObjectPtr v);

/* prototypes for bobdbg.c */
void EXPORT DecodeProcedure(InterpreterContext *c,ObjectPtr code,Stream *stream);
int EXPORT DecodeInstruction(InterpreterContext *c,ObjectPtr code,long lc,Stream *stream);

/* interpreter dispatch codes */
#define itStart		0	/* must be zero */
#define itReturn	1
#define itAbort		2

#endif
