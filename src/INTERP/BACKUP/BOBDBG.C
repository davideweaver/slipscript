/* bobdbg.c - debug routines */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <string.h>
#include "execute.h"

/* instruction output formats */
#define FMT_NONE	0
#define FMT_BYTE	1
#define FMT_2BYTE	2
#define FMT_WORD	3
#define FMT_LIT		4

typedef struct { int ot_code; char *ot_name; int ot_fmt; } OTDEF;
OTDEF otab[] = {
{	OP_BRT,		"BRT",		FMT_WORD	},
{	OP_BRF,		"BRF",		FMT_WORD	},
{	OP_BR,		"BR",		FMT_WORD	},
{	OP_T,		"T",		FMT_NONE	},
{	OP_NIL,		"NIL",		FMT_NONE	},
{	OP_PUSH,	"PUSH",		FMT_NONE	},
{	OP_NOT,		"NOT",		FMT_NONE	},
{	OP_ADD,		"ADD",		FMT_NONE	},
{	OP_SUB,		"SUB",		FMT_NONE	},
{	OP_MUL,		"MUL",		FMT_NONE	},
{	OP_DIV,		"DIV",		FMT_NONE	},
{	OP_REM,		"REM",		FMT_NONE	},
{	OP_BAND,	"BAND",		FMT_NONE	},
{	OP_BOR,		"BOR",		FMT_NONE	},
{	OP_XOR,		"XOR",		FMT_NONE	},
{	OP_BNOT,	"BNOT",		FMT_NONE	},
{	OP_SHL,		"SHL",		FMT_NONE	},
{	OP_SHR,		"SHR",		FMT_NONE	},
{	OP_LT,		"LT",		FMT_NONE	},
{	OP_LE,		"LE",		FMT_NONE	},
{	OP_EQ,		"EQ",		FMT_NONE	},
{	OP_NE,		"NE",		FMT_NONE	},
{	OP_GE,		"GE",		FMT_NONE	},
{	OP_GT,		"GT",		FMT_NONE	},
{	OP_LIT,		"LIT",		FMT_LIT		},
{	OP_GREF,	"GREF",		FMT_LIT		},
{	OP_GSET,	"GSET",		FMT_LIT		},
{	OP_GETP,	"GETP",		FMT_NONE	},
{	OP_SETP,	"SETP",		FMT_NONE	},
{	OP_RETURN,	"RETURN",	FMT_NONE	},
{	OP_CALL,	"CALL",		FMT_BYTE	},
{	OP_EREF,	"EREF",		FMT_2BYTE	},
{	OP_ESET,	"ESET",		FMT_2BYTE	},
{	OP_FRAME,	"FRAME",	FMT_BYTE	},
{	OP_UNFRAME,	"UNFRAME",	FMT_NONE	},
{	OP_VREF,	"VREF",		FMT_NONE	},
{	OP_VSET,	"VSET",		FMT_NONE	},
{	OP_NEG,		"NEG",		FMT_NONE	},
{	OP_INC,		"INC",		FMT_NONE	},
{	OP_DEC,		"DEC",		FMT_NONE	},
{	OP_DUP,		"DUP",		FMT_NONE	},
{	OP_DUP2,	"DUP2",		FMT_NONE	},
{	OP_DROP,	"DROP",		FMT_NONE	},
{	OP_NEWOBJECT,	"NEWOBJECT",	FMT_NONE	},
{	OP_NEWLIST,	"NEWLIST",	FMT_NONE	},
{	OP_NEWVECTOR,	"NEWVECTOR",	FMT_NONE	},
{	OP_AFRAME,	"AFRAME",	FMT_2BYTE	},
{	OP_AFRAME,	"AFRAMER",	FMT_2BYTE	},
{	OP_CLOSE,	"CLOSE",	FMT_NONE	},
{0,0,0}
};

/* DecodeProcedure - decode the instructions in a code object */
void DecodeProcedure(InterpreterContext *c,ObjectPtr method,Stream *stream)
{
    ObjectPtr code = MethodCode(method);
    long len,lc;
    int n;
    len = StringSize(CompiledCodeBytecodes(code));
    for (lc = 0; lc < len; lc += n)
	n = DecodeInstruction(c,code,lc,stream);
}

/* DecodeInstruction - decode a single bytecode instruction */
int DecodeInstruction(InterpreterContext *c,ObjectPtr code,long lc,Stream *stream)
{
    char buf[100];
    ObjectPtr name;
    unsigned char *cp;
    OTDEF *op;
    int n=1;

    /* get bytecode pointer for this instruction and the method name */
    cp = StringDataAddress(CompiledCodeBytecodes(code)) + lc;
    name = CompiledCodeName(code);
    
    /* show the address and opcode */
    if (StringP(name)) {
	unsigned char *data = StringDataAddress(name);
	long size = StringSize(name);
	if (size > 32) size = 32;
	strncpy(buf,(char *)data,(size_t)size);
	sprintf(&buf[size],":%04x %02x ",lc,*cp);
    }
    else
        sprintf(buf,"%08lx:%04x %02x ",code,lc,*cp);
    StreamPutS(c,buf,stream);

    /* display the operands */
    for (op = otab; op->ot_name; ++op)
	if (*cp == op->ot_code) {
	    switch (op->ot_fmt) {
	    case FMT_NONE:
		sprintf(buf,"      %s\n",op->ot_name);
		StreamPutS(c,buf,stream);
		break;
	    case FMT_BYTE:
		sprintf(buf,"%02x    %s %02x\n",cp[1],op->ot_name,cp[1]);
		StreamPutS(c,buf,stream);
		n += 1;
		break;
	    case FMT_2BYTE:
		sprintf(buf,"%02x %02x %s %02x %02x\n",cp[1],cp[2],
			op->ot_name,cp[1],cp[2]);
		StreamPutS(c,buf,stream);
		n += 2;
		break;
	    case FMT_WORD:
		sprintf(buf,"%02x %02x %s %02x%02x\n",cp[1],cp[2],
			op->ot_name,cp[2],cp[1]);
		StreamPutS(c,buf,stream);
		n += 2;
		break;
	    case FMT_LIT:
		sprintf(buf,"%02x %02x %s %02x%02x ; ",cp[1],cp[2],
			op->ot_name,cp[2],cp[1]);
		StreamPutS(c,buf,stream);
		PrintValue(c,CompiledCodeLiteral(code,cp[1]),stream);
		StreamPutC(c,'\n',stream);
		n += 2;
		break;
	    }
	    return n;
	}
    
    /* unknown opcode */
    sprintf(buf,"      <UNKNOWN>\n");
    StreamPutS(c,buf,stream);
    return 1;
}
