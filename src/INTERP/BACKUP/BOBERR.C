/* boberr.c - error strings */

#include "boberr.h"

typedef struct {
    int code;
    char *text;
} ErrString;

ErrString errStrings[] = {
{	ecExit,			"Exit"					},
{	ecInsufficientMemory,	"Insufficient memory"			},
{	ecStackOverflow,	"Stack overflow"			},
{	ecTooManyArguments,	"Too many arguments"			},
{	ecTooFewArguments,	"Too few arguments"			},
{	ecTypeError,		"Wrong type",				},
{	ecUnboundVariable,	"Unbound variable",			},
{	ecIndexOutOfBounds,	"Index out of bounds",			},
{	ecNoMethod,		"No method for this selector"		},
{	ecBadOpcode,		"Bad opcode"				},
{	ecTooManyLiterals,	"Too many literals"			},
{	ecTooMuchCode,		"Too much code"				},
{	ecStoreIntoConstant,	"Attempt to store into a constant",	},
{	ecSyntaxError,		"Syntax error"				},
{0,0}};

/* GetErrorText - get the text for an error code */
char *GetErrorText(int code)
{
    ErrString *p;
    for (p = errStrings; p->text != 0; ++p)
        if (code == p->code)
            return p->text;
    return "Unknown error";
}

