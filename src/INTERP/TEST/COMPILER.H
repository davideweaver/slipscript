/* compile.h - compiler definitions */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "objects.h"
#include "streams.h"

/* token definitions */
#define T_NOTOKEN	-1
#define T_EOF		0

/* non-character tokens */
#define _TMIN		256
#define T_STRING	256
#define T_IDENTIFIER	257
#define T_NUMBER	258
#define T_FUNCTION	259
#define T_LOCAL		260
#define T_IF		261
#define T_ELSE		262
#define T_WHILE		263
#define T_RETURN	264
#define T_FOR		265
#define T_BREAK		266
#define T_CONTINUE	267
#define T_DO		268
#define T_NIL		269
#define T_LE		270	/* '<=' */
#define T_EQ		271	/* '==' */
#define T_NE		272	/* '!=' */
#define T_GE		273	/* '>=' */
#define T_SHL		274	/* '<<' */
#define T_SHR		275	/* '>>' */
#define T_AND		276	/* '&&' */
#define T_OR		277	/* '||' */
#define T_INC		278	/* '++' */
#define T_DEC		279	/* '--' */
#define T_ADDEQ		280	/* '+=' */
#define T_SUBEQ		281	/* '-=' */
#define T_MULEQ		282	/* '*=' */
#define T_DIVEQ		283	/* '/=' */
#define T_REMEQ		284	/* '%=' */
#define T_ANDEQ		285	/* '&=' */
#define T_OREQ		286	/* '|=' */
#define T_XOREQ		287	/* '^=' */
#define T_SHLEQ		288	/* '<<=' */
#define T_SHREQ		289	/* '>>=' */
#define T_DEFINE	290
#define T_SUPER		291
#define _TMAX		291

/* argument structure */
typedef struct argument {
    char *arg_name;		/* argument name */
    struct argument *arg_next;	/* next argument */
} ARGUMENT;

/* argument table structure */
typedef struct atable {
    ARGUMENT *at_arguments;	/* first argument */
    struct atable *at_next;	/* next argument table */
} ATABLE;

/* break/continue stack entry structure */
typedef struct sentry {
    int level;			/* block level */
    int label;			/* label */
} SENTRY;

/* limits */
#define TKNSIZE		255	/* maximum token size */
#define LSIZE		255	/* maximum line size */
#define SSIZE		20	/* break/continue stack size */

/* compiler context structure */
typedef struct CompilerContext {
    InterpreterContext *ic;		/* compiler - interpreter context */
    Stream *input;			/* compiler - input stream */
    int blockLevel;			/* compiler - nesting level */
    ATABLE *arguments;			/* compiler - argument frames */
    SENTRY bstack[SSIZE],*bsp;		/* compiler - break stack */
    SENTRY cstack[SSIZE],*csp;		/* compiler - continue stack */
    unsigned char *codebuf;		/* compiler - code buffer */
    unsigned char *cbase,*cptr,*ctop;	/* compiler - code buffer positions */
    ObjectPtr literalbuf;		/* compiler - literal buffer */
    long lbase,lptr,ltop;		/* compiler - literal buffer positions */
    char *errorMessage;			/* compiler - last error message */
    int t_value;			/* scanner - numeric value */
    char t_token[TKNSIZE+1];		/* scanner - token string */
    int lineNumber;			/* scanner - line number */
    int savedToken;			/* scanner - look ahead token */
    int savedChar;			/* scanner - look ahead character */
    char line[LSIZE+1];			/* scanner - last input line */
    char *linePtr;			/* scanner - line pointer */
    int atEOF;				/* scanner - input end of file flag */
} CompilerContext;

/* prototypes for bobcom.c */
CompilerContext * EXPORT InitCompiler(InterpreterContext *ic,long csize,long lsize);
void EXPORT CompileFile(CompilerContext *c,Stream *is,Stream *os);
ObjectPtr EXPORT CompileExpr(CompilerContext *c,Stream *s);
int EXPORT LoadFile(CompilerContext *c,char *fname,Stream *os);

/* prototypes for bobscn.c */
void InitScan(CompilerContext *c,Stream *s);
int token(CompilerContext *c);
void stoken(CompilerContext *c,int tkn);
char *tkn_name(int tkn);
void parse_error(CompilerContext *c,char *msg);

#endif
