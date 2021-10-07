/* bobscn.c - a lexical scanner */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include "compiler.h"
#include "streams.h"
#include "boberr.h"

/* keyword table */
static struct { char *kt_keyword; int kt_token; } ktab[] = {
{ "define",	T_DEFINE	},
{ "function",	T_FUNCTION	},
{ "local",	T_LOCAL		},
{ "if",		T_IF		},
{ "else",	T_ELSE		},
{ "while",	T_WHILE		},
{ "return",	T_RETURN	},
{ "for",	T_FOR		},
{ "break",	T_BREAK		},
{ "continue", 	T_CONTINUE	},
{ "do",		T_DO		},
{ "NULL",	T_NIL		},
{ "super",	T_SUPER		},
{ NULL,		0		}};

/* token name table */
static char *t_names[] = {
"<string>",
"<identifier>",
"<number>",
"function",
"local",
"if",
"else",
"while",
"return",
"for",
"break",
"continue",
"do",
"NULL",
"<=",
"==",
"!=",
">=",
"<<",
">>",
"&&",
"||",
"++",
"--",
"+=",
"-=",
"*=",
"/=",
"%=",
"&=",
"|=",
"^=",
"<<=",
">>=",
"define",
"function"
};

/* prototypes */
static int rtoken(CompilerContext *c);
static int getstring(CompilerContext *c);
static int getcharacter(CompilerContext *c);
static int literalch(CompilerContext *c);
static int getid(CompilerContext *c,int ch);
static int getnumber(CompilerContext *c,int ch);
static int skipspaces(CompilerContext *c);
static int isidchar(int ch);
static int getch(CompilerContext *c);

/* InitScan - initialize the scanner */
void InitScan(CompilerContext *c,Stream *s)
{
    /* remember the input stream */
    c->input = s;
    
    /* setup the line buffer */
    c->linePtr = c->line; *c->linePtr = '\0';
    c->lineNumber = 0;

    /* no lookahead yet */
    c->savedToken = T_NOTOKEN;
    c->savedChar = '\0';

    /* not eof yet */
    c->atEOF = FALSE;
}

/* token - get the next token */
int token(CompilerContext *c)
{
    int tkn;

    if ((tkn = c->savedToken) != T_NOTOKEN)
	c->savedToken = T_NOTOKEN;
    else
	tkn = rtoken(c);
    return tkn;
}

/* stoken - save a token */
void stoken(CompilerContext *c,int tkn)
{
    c->savedToken = tkn;
}

/* tkn_name - get the name of a token */
char *tkn_name(int tkn)
{
    static char tname[2];
    if (tkn == T_EOF)
	return "<eof>";
    else if (tkn >= _TMIN && tkn <= _TMAX)
	return t_names[tkn-_TMIN];
    tname[0] = tkn;
    tname[1] = '\0';
    return tname;
}

/* rtoken - read the next token */
static int rtoken(CompilerContext *c)
{
    int ch,ch2;

    /* check the next character */
    for (;;)
	switch (ch = skipspaces(c)) {
	case EOF:	return T_EOF;
	case '"':	return getstring(c);
	case '\'':	return getcharacter(c);
	case '<':	switch (ch = getch(c)) {
			case '=':
			    return T_LE;
			case '<':
			    if ((ch = getch(c)) == '=')
				return T_SHLEQ;
			    c->savedChar = ch;
			    return T_SHL;
			default:
			    c->savedChar = ch;
			    return '<';
			}
	case '=':	if ((ch = getch(c)) == '=')
			    return T_EQ;
			c->savedChar = ch;
			return '=';
	case '!':	if ((ch = getch(c)) == '=')
			    return T_NE;
			c->savedChar = ch;
			return '!';
	case '>':	switch (ch = getch(c)) {
			case '=':
			    return T_GE;
			case '>':
			    if ((ch = getch(c)) == '=')
				return T_SHREQ;
			    c->savedChar = ch;
			    return T_SHR;
			default:
			    c->savedChar = ch;
			    return '>';
			}
	case '&':	switch (ch = getch(c)) {
			case '&':
			    return T_AND;
			case '=':
			    return T_ANDEQ;
			default:
			    c->savedChar = ch;
			    return '&';
			}
	case '|':	switch (ch = getch(c)) {
			case '|':
			    return T_OR;
			case '=':
			    return T_OREQ;
			default:
			    c->savedChar = ch;
			    return '|';
			}
	case '^':	if ((ch = getch(c)) == '=')
			    return T_XOREQ;
			c->savedChar = ch;
			return '^';
	case '+':	switch (ch = getch(c)) {
			case '+':
			    return T_INC;
			case '=':
			    return T_ADDEQ;
			default:
			    c->savedChar = ch;
			    return '+';
			}
	case '-':	switch (ch = getch(c)) {
			case '-':
			    return T_DEC;
			case '=':
			    return T_SUBEQ;
			default:
			    c->savedChar = ch;
			    return '-';
			}
	case '*':	if ((ch = getch(c)) == '=')
			    return T_MULEQ;
			c->savedChar = ch;
			return '*';
	case '/':	switch (ch = getch(c)) {
			case '=':
			    return T_DIVEQ;
			case '/':
			    while ((ch = getch(c)) != EOF)
				if (ch == '\n')
				    break;
			    break;
			case '*':
			    ch = ch2 = EOF;
			    for (; (ch2 = getch(c)) != EOF; ch = ch2)
				if (ch == '*' && ch2 == '/')
				    break;
			    break;
			default:
			    c->savedChar = ch;
			    return '/';
			}
			break;
	default:	if (isdigit(ch))
			    return getnumber(c,ch);
			else if (isidchar(ch))
			    return getid(c,ch);
			else {
			    c->t_token[0] = ch;
			    c->t_token[1] = '\0';
			    return ch;
			}
	}
}

/* getstring - get a string */
static int getstring(CompilerContext *c)
{
    int ch,len=0;
    char *p;

    /* get the string */
    p = c->t_token;
    while ((ch = literalch(c)) != EOF && ch != '"') {
	if (++len > TKNSIZE)
	    parse_error(c,"string too long");
	*p++ = ch;
    }
    if (ch == EOF)
	c->savedChar = EOF;
    *p = '\0';
    return T_STRING;
}

/* getcharacter - get a character constant */
static int getcharacter(CompilerContext *c)
{
    c->t_value = literalch(c);
    c->t_token[0] = c->t_value;
    c->t_token[1] = '\0';
    if (getch(c) != '\'')
	parse_error(c,"Expecting a closing single quote");
    return T_NUMBER;
}

/* literalch - get a character from a literal string */
static int literalch(CompilerContext *c)
{
    int ch;
    if ((ch = getch(c)) == '\\')
	switch (ch = getch(c)) {
	case 'n':  ch = '\n'; break;
	case 'r':  ch = '\r'; break;
	case 't':  ch = '\t'; break;
	case EOF:  ch = '\\'; c->savedChar = EOF; break;
	}
    return ch;
}

/* getid - get an identifier */
static int getid(CompilerContext *c,int ch)
{
    int len=1,i;
    char *p;

    /* get the identifier */
    p = c->t_token; *p++ = ch;
    while ((ch = getch(c)) != EOF && isidchar(ch)) {
	if (++len > TKNSIZE)
	    parse_error(c,"identifier too long");
	*p++ = ch;
    }
    c->savedChar = ch;
    *p = '\0';

    /* check to see if it is a keyword */
    for (i = 0; ktab[i].kt_keyword != NULL; ++i)
	if (strcmp(ktab[i].kt_keyword,c->t_token) == 0)
	    return ktab[i].kt_token;
    return T_IDENTIFIER;
}

/* getnumber - get a number */
static int getnumber(CompilerContext *c,int ch)
{
    char *p;

    /* get the number */
    p = c->t_token; *p++ = ch; c->t_value = ch - '0';
    while ((ch = getch(c)) != EOF && isdigit(ch)) {
	c->t_value = c->t_value * 10 + ch - '0';
	*p++ = ch;
    }
    c->savedChar = ch;
    *p = '\0';
    return T_NUMBER;
}

/* skipspaces - skip leading spaces */
static skipspaces(CompilerContext *c)
{
    int ch;
    while ((ch = getch(c)) != '\0' && isspace(ch))
	;
    return ch;
}

/* isidchar - is this an identifier character */
static int isidchar(int ch)
{
    return isupper(ch)
        || islower(ch)
        || isdigit(ch)
        || ch == '_';
}

/* getch - get the next character */
static int getch(CompilerContext *c)
{
    int ch;

    /* check for a lookahead character */
    if ((ch = c->savedChar) != '\0') {
	c->savedChar = '\0';
	return (ch);
    }

    /* check for a buffered character */
    while (*c->linePtr == '\0') {

	/* check for end of file */
	if (c->atEOF)
	    return (StreamEOF);

	/* read another line */
	else {

	    /* read the line */
	    for (c->linePtr = c->line; (ch = StreamGetC(c->input)) != StreamEOF && ch != '\n'; )
		*c->linePtr++ = ch;
	    *c->linePtr++ = '\n'; *c->linePtr = '\0';
	    c->linePtr = c->line;
	    ++c->lineNumber;

	    /* check for end of file */
	    if (ch < 0)
	 	c->atEOF = TRUE;
	 }
    }

    /* return the current character */
    return (*c->linePtr++);
}

/* parse_error - report an error in the current line */
void parse_error(CompilerContext *c,char *msg)
{
    c->errorMessage = msg;
    CallErrorHandler(c->ic,ecSyntaxError,c);
}
