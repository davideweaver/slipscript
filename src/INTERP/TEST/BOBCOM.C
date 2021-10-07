/* bobcom.c - the bytecode compiler */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "execute.h"
#include "streams.h"
#include "boberr.h"

/* local constants */
#define NIL	0	/* fixup list terminator */

/* saved context structure */
typedef struct scontext {
    int oldLevel;
    unsigned char *oldcbase;
    long oldlbase;
} SCONTEXT;

/* partial value structure */
typedef struct pvalue {
    void (*fcn)(CompilerContext *,int,struct pvalue *);
    int val,val2;
} PVAL;

/* variable access function codes */
#define LOAD	1
#define STORE	2
#define PUSH	3
#define DUP	4

/* forward declarations */
static void SetupCompiler(CompilerContext *c);
static void RestoreCompiler(CompilerContext *c);
static void do_statement(CompilerContext *c);
static void do_function(CompilerContext *c);
static void define_method(CompilerContext *c);
static void define_function(CompilerContext *c);
static void do_if(CompilerContext *c);
static void do_while(CompilerContext *c);
static void do_dowhile(CompilerContext *c);
static void do_for(CompilerContext *c);
static SENTRY *addbreak(CompilerContext *c,int lbl);
static int rembreak(CompilerContext *c,SENTRY *old,int lbl);
static void do_break(CompilerContext *c);
static SENTRY *addcontinue(CompilerContext *c,int lbl);
static void remcontinue(CompilerContext *c,SENTRY *old);
static void do_continue(CompilerContext *c);
static void UnwindStack(CompilerContext *c,int levels);
static void do_block(CompilerContext *c);
static void do_local(CompilerContext *c);
static void do_return(CompilerContext *c);
static void do_test(CompilerContext *c);
static void do_expr(CompilerContext *c);
static void do_init_expr(CompilerContext *c);
static void rvalue(CompilerContext *c,PVAL *pv);
static void chklvalue(CompilerContext *c,PVAL *pv);
static void do_expr1(CompilerContext *c,PVAL *pv);
static void do_expr2(CompilerContext *c,PVAL *pv);
static void do_assignment(CompilerContext *c,PVAL *pv,int op);
static void do_expr3(CompilerContext *c,PVAL *pv);
static void do_expr4(CompilerContext *c,PVAL *pv);
static void do_expr5(CompilerContext *c,PVAL *pv);
static void do_expr6(CompilerContext *c,PVAL *pv);
static void do_expr7(CompilerContext *c,PVAL *pv);
static void do_expr8(CompilerContext *c,PVAL *pv);
static void do_expr9(CompilerContext *c,PVAL *pv);
static void do_expr10(CompilerContext *c,PVAL *pv);
static void do_expr11(CompilerContext *c,PVAL *pv);
static void do_expr12(CompilerContext *c,PVAL *pv);
static void do_expr13(CompilerContext *c,PVAL *pv);
static void do_expr14(CompilerContext *c,PVAL *pv);
static void do_preincrement(CompilerContext *c,PVAL *pv,int op);
static void do_postincrement(CompilerContext *c,PVAL *pv,int op);
static void do_expr15(CompilerContext *c,PVAL *pv);
static void do_primary(CompilerContext *c,PVAL *pv);
static void do_prop_reference(CompilerContext *c);
static void do_define(CompilerContext *c,PVAL *pv);
static void do_literal(CompilerContext *c,PVAL *pv);
static void do_literal_symbol(CompilerContext *c,PVAL *pv);
static void do_literal_list(CompilerContext *c,PVAL *pv);
static void do_literal_vector(CompilerContext *c,PVAL *pv);
static void do_literal_object(CompilerContext *c,PVAL *pv);
static ObjectPtr do_code(CompilerContext *c,ObjectPtr name);
static ObjectPtr continue_code(CompilerContext *c,SCONTEXT *sc);
static void begin_code(CompilerContext *c,SCONTEXT *sc);
static void do_call(CompilerContext *c,PVAL *pv);
static void do_send(CompilerContext *c,PVAL *pv);
static void do_index(CompilerContext *c,PVAL *pv);
static int get_id_list(CompilerContext *c,ATABLE *atable,char *term);
static void AddArgument(CompilerContext *c,ATABLE *atable,char *name);
static ATABLE *MakeArgFrame(CompilerContext *c);
static void PushArgFrame(CompilerContext *c,ATABLE *atable);
static void PushNewArgFrame(CompilerContext *c);
static void PopArgFrame(CompilerContext *c);
static void FreeArguments(CompilerContext *c);
static int FindArgument(CompilerContext *c,char *name,int *plev,int *poff);
static int ArgumentCount(CompilerContext *c);
static int addliteral(CompilerContext *c,ObjectPtr lit);
static void frequire(CompilerContext *c,int rtkn);
static void require(CompilerContext *c,int tkn,int rtkn);
static void do_lit_integer(CompilerContext *c,long n);
static void do_lit_string(CompilerContext *c,char *str);
static void do_lit_symbol(CompilerContext *c,char *pname);
static int make_lit_string(CompilerContext *c,char *str);
static int make_lit_symbol(CompilerContext *c,char *pname);
static void findvariable(CompilerContext *c,char *id,PVAL *pv);
static void code_constant(CompilerContext *c,int fcn,PVAL *);
static int load_argument(CompilerContext *c,char *name);
static void code_argument(CompilerContext *c,int fcn,PVAL *);
static void code_property(CompilerContext *c,int fcn,PVAL *);
static void code_variable(CompilerContext *c,int fcn,PVAL *);
static void code_index(CompilerContext *c,int fcn,PVAL *);
static void code_literal(CompilerContext *c,int n);
static int codeaddr(CompilerContext *c);
static void putcbyte(CompilerContext *c,int b);
static void putcword(CompilerContext *c,int w);
static void fixup(CompilerContext *c,int chn,int val);
static char *copystring(CompilerContext *c,char *str);

/* InitCompiler - initialize the compiler */
CompilerContext *InitCompiler(InterpreterContext *ic,long csize,long lsize)
{
    CompilerContext *c;
    ProtectPointer(ic,&c->literalbuf);
    CheckNIL(ic,c = (CompilerContext *)malloc(sizeof(CompilerContext)));
    CheckNIL(ic,c->codebuf = (unsigned char *)malloc((size_t)csize));
    CheckNIL(ic,c->literalbuf = (ObjectPtr *)malloc((size_t)(lsize * sizeof(ObjectPtr))));
    c->cptr = c->codebuf; c->ctop = c->codebuf + csize;
    c->lptr = 0; c->ltop = lsize;
    ic->cc = (void *)c;
    c->ic = ic;
    return c;
}

/* SetupCompiler - setup the compiler context */
static void SetupCompiler(CompilerContext *c)
{
    c->cbase = c->cptr;
    c->lbase = c->lptr;
    c->bsp = &c->bstack[-1];
    c->csp = &c->cstack[-1];
    c->arguments = NULL;
    c->blockLevel = 0;
}

/* RestoreCompiler - restore the previous compiler context */
static void RestoreCompiler(CompilerContext *c)
{
    c->cptr = c->cbase;
    c->lptr = c->lbase;
    FreeArguments(c);
}

/* LoadFile - read and evaluate expressions from a file */
int LoadFile(CompilerContext *c,char *fname,Stream *os)
{
    Stream *is;
    if ((is = OpenFileStream(fname,"r")) != NULL) {
        CompileFile(c,is,os);
        CloseStream(is);
    }
    return is != NULL;
}
    
/* CompileFile - compile a file of expressions */
void CompileFile(CompilerContext *c,Stream *is,Stream *os)
{
    ObjectPtr expr;
    while ((expr = CompileExpr(c,is)) != NULL) {
	ObjectPtr val = CallFunction(c->ic,expr,0);
	if (os) {
	    /*PrintValue(c->ic,val,os);*/
	    /*StreamPutC('\n',os);*/
	}
    }
}
	
/* CompileExpr - compile a single expression */
ObjectPtr CompileExpr(CompilerContext *c,Stream *s)
{
    InterpreterContext *ic = c->ic;
    ObjectPtr code,*src,*dst;
    long size;
    int tkn;
    
    /* initialize the scanner */
    InitScan(c,s);

    /* check for end of file */
    if ((tkn = token(c)) == T_EOF)
	return NULL;
    stoken(c,tkn);
    
    /* initialize the compiler */
    SetupCompiler(c);
    
    /* make dummy function name */
    addliteral(c,NewCStringObject(ic,"(Listener)"));
    
    /* generate the argument frame */
    putcbyte(c,OP_AFRAME);
    putcbyte(c,0);
    putcbyte(c,0);
    
    /* compile the code */
    putcbyte(c,OP_PUSH);
    do_statement(c);
    putcbyte(c,OP_RETURN);

    /* make the bytecode array */
    code = NewStringObject(ic,c->cbase,c->cptr - c->cbase);
    
    /* make the compiled code object */
    size = c->lptr - c->lbase;
    code = NewCompiledCodeObject(ic,FirstLiteral + size,code);
    src = VectorDataAddress(c->literalbuf) + c->lbase;
    dst = VectorDataAddress(code) + FirstLiteral;
    while (--size >= 0)
        *dst++ = *src++;
    
    /* make a closure */
    code = NewMethodObject(ic,code,ic->nilObject);
    
    /* restore the previous compiler context */
    RestoreCompiler(c);
    
    /* return the function */
    /*DecodeProcedure(ic,code,ic->standardOutput);*/
    return code;
}

/* do_statement - compile a single statement */
static void do_statement(CompilerContext *c)
{
    int tkn;
    switch (tkn = token(c)) {
    case T_FUNCTION:	do_function(c);	break;
	case T_DEFINE:	do_function(c);  break;  /* for backwards comp. */
    case T_IF:		do_if(c);	break;
    case T_WHILE:	do_while(c);	break;
    case T_DO:		do_dowhile(c);	break;
    case T_FOR:		do_for(c);	break;
    case T_BREAK:	do_break(c);	break;
    case T_CONTINUE:	do_continue(c);	break;
    case T_RETURN:	do_return(c);	break;
    case '{':		do_block(c);	break;
    case ';':		;		break;
    default:		stoken(c,tkn);
			do_expr(c);
			frequire(c,';');  break;
    }
}

/* do_function - compile the DEFINE expression */
static void do_function(CompilerContext *c)
{
    switch (token(c)) {
    case '[':
	define_method(c);
	break;
    case T_IDENTIFIER:
	define_function(c);
	break;
    default:
	parse_error(c,"Expecting a function or a method definition");
	break;
    }
}

/* define_method - handle method definition statement */
static void define_method(CompilerContext *c)
{
    char selector[256];
    SCONTEXT sc;
    int tkn;
    
    /* get the class (in the 'self' position) */
    do_expr(c);
    putcbyte(c,OP_PUSH);
    
    /* initialize */
    begin_code(c,&sc);
    
    /* the first arguments are always 'self' and '_next' */
    AddArgument(c,c->arguments,"self");
    AddArgument(c,c->arguments,"_next");

    /* get the first part of the selector */
    frequire(c,T_IDENTIFIER);
    strcpy(selector,c->t_token);

    /* get each formal argument and each part of the message selector */
    while ((tkn = token(c)) != ']') {
	if (tkn == ':') {
	    strcat(selector,":");
	    tkn = token(c);
	}
	require(c,tkn,T_IDENTIFIER);
	AddArgument(c,c->arguments,c->t_token);
	if ((tkn = token(c)) == ']')
	    break;
	require(c,tkn,T_IDENTIFIER);
	strcat(selector,c->t_token);
    }
    
    /* compile the code */
    make_lit_string(c,selector);	/* method name is the first literal */
    cpush(c->ic,continue_code(c,&sc));
    
    /* push the selector symbol for setting the property value */
    do_lit_symbol(c,selector);
    putcbyte(c,OP_PUSH);
    
    /* push the method object */
    code_literal(c,addliteral(c,pop(c->ic)));
    putcbyte(c,OP_CLOSE);
    
     /* store the method as the value of the property */
    putcbyte(c,OP_SETP);
}

/* define_function - handle function definition statement */
static void define_function(CompilerContext *c)
{
    char name[256];
    SCONTEXT sc;
    
    /* initialize */
    strcpy(name,c->t_token);
    begin_code(c,&sc);
    make_lit_string(c,name);	/* method name is the first literal */
    
    /* get the argument list */
    frequire(c,'(');
    get_id_list(c,c->arguments,")");
    frequire(c,')');

    /* compile the code */
    code_literal(c,addliteral(c,continue_code(c,&sc)));
    putcbyte(c,OP_CLOSE);
    
    /* store the method as the value of the global symbol */
    putcbyte(c,OP_GSET);
    putcword(c,make_lit_symbol(c,name));
}

/* do_if - compile the IF/ELSE expression */
static void do_if(CompilerContext *c)
{
    int tkn,nxt,end;

    /* compile the test expression */
    do_test(c);

    /* skip around the 'then' clause if the expression is false */
    putcbyte(c,OP_BRF);
    nxt = codeaddr(c);
    putcword(c,NIL);

    /* compile the 'then' clause */
    do_statement(c);

    /* compile the 'else' clause */
    if ((tkn = token(c)) == T_ELSE) {
	putcbyte(c,OP_BR);
	end = codeaddr(c);
	putcword(c,NIL);
	fixup(c,nxt,codeaddr(c));
	do_statement(c);
	nxt = end;
    }
    else
	stoken(c,tkn);

    /* handle the end of the statement */
    fixup(c,nxt,codeaddr(c));
}

/* do_while - compile the WHILE expression */
static void do_while(CompilerContext *c)
{
    SENTRY *ob,*oc;
    int nxt,end;

    /* compile the test expression */
    nxt = codeaddr(c);
    do_test(c);

    /* skip around the loop body if the expression is false */
    putcbyte(c,OP_BRF);
    end = codeaddr(c);
    putcword(c,NIL);

    /* compile the loop body */
    ob = addbreak(c,end);
    oc = addcontinue(c,nxt);
    do_statement(c);
    end = rembreak(c,ob,end);
    remcontinue(c,oc);

    /* branch back to the start of the loop */
    putcbyte(c,OP_BR);
    putcword(c,nxt);

    /* handle the end of the statement */
    fixup(c,end,codeaddr(c));
}

/* do_dowhile - compile the DO/WHILE expression */
static void do_dowhile(CompilerContext *c)
{
    SENTRY *ob,*oc;
    int nxt,end=0;

    /* remember the start of the loop */
    nxt = codeaddr(c);

    /* compile the loop body */
    ob = addbreak(c,0);
    oc = addcontinue(c,nxt);
    do_statement(c);
    end = rembreak(c,ob,end);
    remcontinue(c,oc);

    /* compile the test expression */
    frequire(c,T_WHILE);
    do_test(c);
    frequire(c,';');

    /* branch to the top if the expression is true */
    putcbyte(c,OP_BRT);
    putcword(c,nxt);

    /* handle the end of the statement */
    fixup(c,end,codeaddr(c));
}

/* do_for - compile the FOR statement */
static void do_for(CompilerContext *c)
{
    int tkn,nxt,end,body,update;
    SENTRY *ob,*oc;

    /* compile the initialization expression */
    frequire(c,'(');
    if ((tkn = token(c)) != ';') {
	stoken(c,tkn);
	do_expr(c);
	frequire(c,';');
    }

    /* compile the test expression */
    nxt = codeaddr(c);
    if ((tkn = token(c)) != ';') {
	stoken(c,tkn);
	do_expr(c);
	frequire(c,';');
    }

    /* branch to the loop body if the expression is true */
    putcbyte(c,OP_BRT);
    body = codeaddr(c);
    putcword(c,NIL);

    /* branch to the end if the expression is false */
    putcbyte(c,OP_BR);
    end = codeaddr(c);
    putcword(c,NIL);

    /* compile the update expression */
    update = codeaddr(c);
    if ((tkn = token(c)) != ')') {
	stoken(c,tkn);
	do_expr(c);
	frequire(c,')');
    }

    /* branch back to the test code */
    putcbyte(c,OP_BR);
    putcword(c,nxt);

    /* compile the loop body */
    fixup(c,body,codeaddr(c));
    ob = addbreak(c,end);
    oc = addcontinue(c,update);
    do_statement(c);
    end = rembreak(c,ob,end);
    remcontinue(c,oc);

    /* branch back to the update code */
    putcbyte(c,OP_BR);
    putcword(c,update);

    /* handle the end of the statement */
    fixup(c,end,codeaddr(c));
}

/* addbreak - add a break level to the stack */
static SENTRY *addbreak(CompilerContext *c,int lbl)
{
    SENTRY *old=c->bsp;
    if (++c->bsp < &c->bstack[SSIZE]) {
	c->bsp->level = c->blockLevel;
	c->bsp->label = lbl;
    }
    else
	parse_error(c,"Too many nested loops");
    return old;
}

/* rembreak - remove a break level from the stack */
static int rembreak(CompilerContext *c,SENTRY *old,int lbl)
{
   return c->bsp > old ? (c->bsp--)->label : lbl;
}

/* do_break - compile the BREAK statement */
static void do_break(CompilerContext *c)
{
    int oldLabel;
    if (c->bsp >= c->bstack) {
	UnwindStack(c,c->blockLevel - c->bsp->level);
	putcbyte(c,OP_BR);
	oldLabel = c->bsp->label;
	c->bsp->label = codeaddr(c);
	putcword(c,oldLabel);
    }
    else
	parse_error(c,"Break outside of loop");
}

/* addcontinue - add a continue level to the stack */
static SENTRY *addcontinue(CompilerContext *c,int lbl)
{
    SENTRY *old=c->csp;
    if (++c->csp < &c->cstack[SSIZE]) {
	c->csp->level = c->blockLevel;
	c->csp->label = lbl;
    }
    else
	parse_error(c,"Too many nested loops");
    return old;
}

/* remcontinue - remove a continue level from the stack */
static void remcontinue(CompilerContext *c,SENTRY *old)
{
    c->csp = old;
}

/* do_continue - compile the CONTINUE statement */
static void do_continue(CompilerContext *c)
{
    if (c->csp >= c->cstack) {
	UnwindStack(c,c->blockLevel - c->bsp->level);
	putcbyte(c,OP_BR);
	putcword(c,c->csp->label);
    }
    else
	parse_error(c,"Continue outside of loop");
}

/* UnwindStack - pop frames off the stack to get back to a previous nesting level */
static void UnwindStack(CompilerContext *c,int levels)
{
    while (--levels >= 0)
	putcbyte(c,OP_UNFRAME);
}

/* do_block - compile the {} expression */
static void do_block(CompilerContext *c)
{
    int tkn,lcnt=0;
    
    /* handle LOCAL declarations */
    while ((tkn = token(c)) == T_LOCAL) {
	do_local(c);
	++lcnt;
    }
    
    /* compile the statements in the block */
    if (tkn != '}') {
	do {
	    stoken(c,tkn);
	    do_statement(c);
	} while ((tkn = token(c)) != '}');
    }
    else
	putcbyte(c,OP_NIL);

    /* pop the local frames */
    while (--lcnt >= 0) {
	putcbyte(c,OP_UNFRAME);
	PopArgFrame(c);
	--c->blockLevel;
    }
}

/* do_local - handle the LOCAL declaration */
static void do_local(CompilerContext *c)
{
    ATABLE *atable;
    int tkn,tcnt=0;

    /* make a new argument frame */
    atable = MakeArgFrame(c);

    /* compile each initialization expression */
    do {
	frequire(c,T_IDENTIFIER);
	AddArgument(c,atable,c->t_token);
	putcbyte(c,OP_PUSH);
	if ((tkn = token(c)) == '=')
	    do_init_expr(c);
	else {
	    stoken(c,tkn);
	    putcbyte(c,OP_NIL);
	}
	++tcnt;
    } while ((tkn = token(c)) == ',');
    require(c,tkn,';');
    
    /* establish the new frame */
    PushArgFrame(c,atable);

    /* push a new argument frame */
    putcbyte(c,OP_FRAME);
    putcbyte(c,tcnt);
    ++c->blockLevel;

    /* compile the body */
    putcbyte(c,OP_PUSH);
}

/* do_return - handle the RETURN expression */
static void do_return(CompilerContext *c)
{
    do_expr(c);
    frequire(c,';');
    UnwindStack(c,c->blockLevel);
    putcbyte(c,OP_RETURN);
}

/* do_test - compile a test expression */
static void do_test(CompilerContext *c)
{
    frequire(c,'(');
    do_expr(c);
    frequire(c,')');
}

/* do_expr - parse an expression */
static void do_expr(CompilerContext *c)
{
    PVAL pv;
    do_expr1(c,&pv);
    rvalue(c,&pv);
}

/* do_init_expr - parse an initialization expression */
static void do_init_expr(CompilerContext *c)
{
    PVAL pv;
    do_expr2(c,&pv);
    rvalue(c,&pv);
}

/* rvalue - get the rvalue of a partial expression */
static void rvalue(CompilerContext *c,PVAL *pv)
{
    if (pv->fcn) {
	(*pv->fcn)(c,LOAD,pv);
	pv->fcn = NULL;
    }
}

/* chklvalue - make sure we've got an lvalue */
static void chklvalue(CompilerContext *c,PVAL *pv)
{
    if (pv->fcn == NULL)
	parse_error(c,"Expecting an lvalue");
}

/* do_expr1 - handle the ',' operator */
static void do_expr1(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_expr2(c,pv);
    while ((tkn = token(c)) == ',') {
	rvalue(c,pv);
	do_expr1(c,pv); rvalue(c,pv);
    }
    stoken(c,tkn);
}

/* do_expr2 - handle the assignment operators */
static void do_expr2(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_expr3(c,pv);
    while ((tkn = token(c)) == '='
    ||     tkn == T_ADDEQ || tkn == T_SUBEQ
    ||     tkn == T_MULEQ || tkn == T_DIVEQ || tkn == T_REMEQ
    ||     tkn == T_ANDEQ || tkn == T_OREQ  || tkn == T_XOREQ
    ||     tkn == T_SHLEQ || tkn == T_SHLEQ) {
	chklvalue(c,pv);
	switch (tkn) {
	case '=':
	    (*pv->fcn)(c,PUSH,0);
	    do_expr(c);
	    (*pv->fcn)(c,STORE,pv);
	    break;
	case T_ADDEQ:	    do_assignment(c,pv,OP_ADD);	    break;
	case T_SUBEQ:	    do_assignment(c,pv,OP_SUB);	    break;
	case T_MULEQ:	    do_assignment(c,pv,OP_MUL);	    break;
	case T_DIVEQ:	    do_assignment(c,pv,OP_DIV);	    break;
	case T_REMEQ:	    do_assignment(c,pv,OP_REM);	    break;
	case T_ANDEQ:	    do_assignment(c,pv,OP_BAND);    break;
	case T_OREQ:	    do_assignment(c,pv,OP_BOR);	    break;
	case T_XOREQ:	    do_assignment(c,pv,OP_XOR);	    break;
	case T_SHLEQ:	    do_assignment(c,pv,OP_SHL);	    break;
	case T_SHREQ:	    do_assignment(c,pv,OP_SHR);	    break;
 	}
	pv->fcn = NULL;
    }
    stoken(c,tkn);
}

/* do_assignment - handle assignment operations */
static void do_assignment(CompilerContext *c,PVAL *pv,int op)
{
    (*pv->fcn)(c,DUP,0);
    (*pv->fcn)(c,LOAD,pv);
    putcbyte(c,OP_PUSH);
    do_expr(c);
    putcbyte(c,op);
    (*pv->fcn)(c,STORE,pv);
}

/* do_expr3 - handle the '?:' operator */
static void do_expr3(CompilerContext *c,PVAL *pv)
{
    int tkn,nxt,end;
    do_expr4(c,pv);
    while ((tkn = token(c)) == '?') {
	rvalue(c,pv);
	putcbyte(c,OP_BRF);
	nxt = codeaddr(c);
	putcword(c,NIL);
	do_expr1(c,pv); rvalue(c,pv);
	frequire(c,':');
	putcbyte(c,OP_BR);
	end = codeaddr(c);
	putcword(c,NIL);
	fixup(c,nxt,codeaddr(c));
	do_expr1(c,pv); rvalue(c,pv);
	fixup(c,end,codeaddr(c));
    }
    stoken(c,tkn);
}

/* do_expr4 - handle the '||' operator */
static void do_expr4(CompilerContext *c,PVAL *pv)
{
    int tkn,nxt,end=0;
    do_expr5(c,pv);
    while ((tkn = token(c)) == T_OR) {
	rvalue(c,pv);
	putcbyte(c,OP_BRT);
	nxt = codeaddr(c);
	putcword(c,end);
	do_expr5(c,pv); rvalue(c,pv);
	end = nxt;
    }
    fixup(c,end,codeaddr(c));
    stoken(c,tkn);
}

/* do_expr5 - handle the '&&' operator */
static void do_expr5(CompilerContext *c,PVAL *pv)
{
    int tkn,nxt,end=0;
    do_expr6(c,pv);
    while ((tkn = token(c)) == T_AND) {
	rvalue(c,pv);
	putcbyte(c,OP_BRF);
	nxt = codeaddr(c);
	putcword(c,end);
	do_expr6(c,pv); rvalue(c,pv);
	end = nxt;
    }
    fixup(c,end,codeaddr(c));
    stoken(c,tkn);
}

/* do_expr6 - handle the '|' operator */
static void do_expr6(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_expr7(c,pv);
    while ((tkn = token(c)) == '|') {
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr7(c,pv); rvalue(c,pv);
	putcbyte(c,OP_BOR);
    }
    stoken(c,tkn);
}

/* do_expr7 - handle the '^' operator */
static void do_expr7(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_expr8(c,pv);
    while ((tkn = token(c)) == '^') {
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr8(c,pv); rvalue(c,pv);
	putcbyte(c,OP_XOR);
    }
    stoken(c,tkn);
}

/* do_expr8 - handle the '&' operator */
static void do_expr8(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_expr9(c,pv);
    while ((tkn = token(c)) == '&') {
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr9(c,pv); rvalue(c,pv);
	putcbyte(c,OP_BAND);
    }
    stoken(c,tkn);
}

/* do_expr9 - handle the '==' and '!=' operators */
static void do_expr9(CompilerContext *c,PVAL *pv)
{
    int tkn,op;
    do_expr10(c,pv);
    while ((tkn = token(c)) == T_EQ || tkn == T_NE) {
	switch (tkn) {
	case T_EQ: op = OP_EQ; break;
	case T_NE: op = OP_NE; break;
	}
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr10(c,pv); rvalue(c,pv);
	putcbyte(c,op);
    }
    stoken(c,tkn);
}

/* do_expr10 - handle the '<', '<=', '>=' and '>' operators */
static void do_expr10(CompilerContext *c,PVAL *pv)
{
    int tkn,op;
    do_expr11(c,pv);
    while ((tkn = token(c)) == '<' || tkn == T_LE || tkn == T_GE || tkn == '>') {
	switch (tkn) {
	case '<':  op = OP_LT; break;
	case T_LE: op = OP_LE; break;
	case T_GE: op = OP_GE; break;
	case '>':  op = OP_GT; break;
	}
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr11(c,pv); rvalue(c,pv);
	putcbyte(c,op);
    }
    stoken(c,tkn);
}

/* do_expr11 - handle the '<<' and '>>' operators */
static void do_expr11(CompilerContext *c,PVAL *pv)
{
    int tkn,op;
    do_expr12(c,pv);
    while ((tkn = token(c)) == T_SHL || tkn == T_SHR) {
	switch (tkn) {
	case T_SHL: op = OP_SHL; break;
	case T_SHR: op = OP_SHR; break;
	}
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr12(c,pv); rvalue(c,pv);
	putcbyte(c,op);
    }
    stoken(c,tkn);
}

/* do_expr12 - handle the '+' and '-' operators */
static void do_expr12(CompilerContext *c,PVAL *pv)
{
    int tkn,op;
    do_expr13(c,pv);
    while ((tkn = token(c)) == '+' || tkn == '-') {
	switch (tkn) {
	case '+': op = OP_ADD; break;
	case '-': op = OP_SUB; break;
	}
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr13(c,pv); rvalue(c,pv);
	putcbyte(c,op);
    }
    stoken(c,tkn);
}

/* do_expr13 - handle the '*' and '/' operators */
static void do_expr13(CompilerContext *c,PVAL *pv)
{
    int tkn,op;
    do_expr14(c,pv);
    while ((tkn = token(c)) == '*' || tkn == '/' || tkn == '%') {
	switch (tkn) {
	case '*': op = OP_MUL; break;
	case '/': op = OP_DIV; break;
	case '%': op = OP_REM; break;
	}
	rvalue(c,pv);
	putcbyte(c,OP_PUSH);
	do_expr14(c,pv); rvalue(c,pv);
	putcbyte(c,op);
    }
    stoken(c,tkn);
}

/* do_expr14 - handle unary operators */
static void do_expr14(CompilerContext *c,PVAL *pv)
{
    int tkn;
    switch (tkn = token(c)) {
    case '-':
	do_expr15(c,pv); rvalue(c,pv);
	putcbyte(c,OP_NEG);
	break;
    case '!':
	do_expr15(c,pv); rvalue(c,pv);
	putcbyte(c,OP_NOT);
	break;
    case '~':
	do_expr15(c,pv); rvalue(c,pv);
	putcbyte(c,OP_BNOT);
	break;
    case T_INC:
	do_preincrement(c,pv,OP_INC);
	break;
    case T_DEC:
	do_preincrement(c,pv,OP_DEC);
	break;
    default:
	stoken(c,tkn);
	do_expr15(c,pv);
	return;
    }
}

/* do_preincrement - handle prefix '++' and '--' */
static void do_preincrement(CompilerContext *c,PVAL *pv,int op)
{
    do_expr15(c,pv);
    chklvalue(c,pv);
    (*pv->fcn)(c,DUP,0);
    (*pv->fcn)(c,LOAD,pv);
    putcbyte(c,op);
    (*pv->fcn)(c,STORE,pv);
    pv->fcn = NULL;
}

/* do_postincrement - handle postfix '++' and '--' */
static void do_postincrement(CompilerContext *c,PVAL *pv,int op)
{
    chklvalue(c,pv);
    (*pv->fcn)(c,DUP,0);
    (*pv->fcn)(c,LOAD,pv);
    putcbyte(c,op);
    (*pv->fcn)(c,STORE,pv);
    putcbyte(c,op == OP_INC ? OP_DEC : OP_INC);
    pv->fcn = NULL;
}

/* do_expr15 - handle function calls */
static void do_expr15(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_primary(c,pv);
    while ((tkn = token(c)) == '('
    ||     tkn == '['
    ||     tkn == '.'
    ||     tkn == T_INC
    ||     tkn == T_DEC)
	switch (tkn) {
	case '(':
	    do_call(c,pv);
	    break;
	case '[':
	    do_index(c,pv);
	    break;
	case '.':
	    rvalue(c,pv);
	    putcbyte(c,OP_PUSH);
	    do_prop_reference(c);
	    pv->fcn = code_property;
	    break;
	case T_INC:
	    do_postincrement(c,pv,OP_INC);
	    break;
	case T_DEC:
	    do_postincrement(c,pv,OP_DEC);
	    break;
	}
    stoken(c,tkn);
}

/* do_prop_reference - parse a property reference */
static void do_prop_reference(CompilerContext *c)
{
    int tkn;
    switch (tkn = token(c)) {
    case T_IDENTIFIER:
	code_literal(c,addliteral(c,InternCString(c->ic,c->t_token)));
	break;
    default:
	stoken(c,tkn);
	do_expr(c);
	break;
    }
}

/* do_primary - parse a primary expression and unary operators */
static void do_primary(CompilerContext *c,PVAL *pv)
{
    switch (token(c)) {
    //case T_FUNCTION:
 	//do_function(c,pv);
	//break;
    case '\\':
	do_literal(c,pv);
	break;
    case '(':
	do_expr1(c,pv);
	frequire(c,')');
	break;
    case '[':
	do_send(c,pv);
	break;
    case T_NUMBER:
	do_lit_integer(c,(long)c->t_value);
	pv->fcn = NULL;
	break;
    case T_STRING:
	do_lit_string(c,c->t_token);
	pv->fcn = NULL;
	break;
    case T_NIL:
	putcbyte(c,OP_NIL);
	pv->fcn = NULL;
	break;
    case T_IDENTIFIER:
	findvariable(c,c->t_token,pv);
	break;
    default:
	parse_error(c,"Expecting a primary expression");
	break;
    }
}

/* do_define - parse a regular definition */
static void do_define(CompilerContext *c,PVAL *pv)
{
    ObjectPtr name = c->ic->nilObject;
    int tkn;
    if ((tkn = token(c)) == T_STRING)
	name = NewCStringObject(c->ic,c->t_token);
    else
	stoken(c,tkn);
    code_literal(c,addliteral(c,do_code(c,name)));
    putcbyte(c,OP_CLOSE);
    pv->fcn = NULL;
}

/* do_literal - parse a literal expression */
static void do_literal(CompilerContext *c,PVAL *pv)
{
    switch (token(c)) {
    case T_IDENTIFIER:	/* symbol */
	do_literal_symbol(c,pv);
	break;
    case '(':		/* list */
	do_literal_list(c,pv);
	break;
    case '[':		/* vector */
	do_literal_vector(c,pv);
	break;
    case '{':		/* object */
	do_literal_object(c,pv);
	break;
    default:
	parse_error(c,"Expecting a symbol, list, vector or object");
	break;
    }
}

/* do_literal_symbol - parse a literal symbol */
static void do_literal_symbol(CompilerContext *c,PVAL *pv)
{
    code_literal(c,addliteral(c,InternCString(c->ic,c->t_token)));
    pv->fcn = NULL;
}

/* do_literal_list - parse a literal list */
static void do_literal_list(CompilerContext *c,PVAL *pv)
{
    long cnt = 0;
    int tkn;
    if ((tkn = token(c)) != ')') {
	stoken(c,tkn);
	do {
	    ++cnt;
	    do_init_expr(c);
	    putcbyte(c,OP_PUSH);
	} while ((tkn = token(c)) == ',');
	require(c,tkn,')');
    }
    do_lit_integer(c,cnt);
    putcbyte(c,OP_NEWLIST);
    pv->fcn = NULL;
}

/* do_literal_vector - parse a literal vector */
static void do_literal_vector(CompilerContext *c,PVAL *pv)
{
    long cnt = 0;
    int tkn;
    if ((tkn = token(c)) != ']') {
	stoken(c,tkn);
	do {
	    ++cnt;
	    do_init_expr(c);
	    putcbyte(c,OP_PUSH);
	} while ((tkn = token(c)) == ',');
	require(c,tkn,']');
    }
    do_lit_integer(c,cnt);
    putcbyte(c,OP_NEWVECTOR);
    pv->fcn = NULL;
}

/* do_literal_object - parse a literal object */
static void do_literal_object(CompilerContext *c,PVAL *pv)
{
    int tkn;
    do_expr(c);
    putcbyte(c,OP_NEWOBJECT);
    if ((tkn = token(c)) != '}') {
	stoken(c,tkn);
	do {
	    putcbyte(c,OP_DUP);
	    putcbyte(c,OP_PUSH);
	    do_prop_reference(c);
	    frequire(c,':');
	    putcbyte(c,OP_PUSH);
	    do_init_expr(c);
	    putcbyte(c,OP_SETP);
	    putcbyte(c,OP_DROP);
	} while ((tkn = token(c)) == ',');
	require(c,tkn,'}');
    }
    pv->fcn = NULL;
}

/* do_code - compile the code part of a function or method */
static ObjectPtr do_code(CompilerContext *c,ObjectPtr name)
{
    SCONTEXT sc;
    begin_code(c,&sc);
    addliteral(c,name);	/* method name is the first literal */
    frequire(c,'(');
    get_id_list(c,c->arguments,")");
    frequire(c,')');
    return continue_code(c,&sc);
}

/* begin_code - begin compiling a function or method */
static void begin_code(CompilerContext *c,SCONTEXT *sc)
{
    sc->oldLevel = c->blockLevel;
    sc->oldcbase = c->cbase;
    sc->oldlbase = c->lbase;

    /* initialize */
    PushNewArgFrame(c);
    c->blockLevel = 0;
    c->cbase = c->cptr;
    c->lbase = c->lptr;
}

/* continue_code - continue compiling a function or method */
static ObjectPtr continue_code(CompilerContext *c,SCONTEXT *sc)
{
    InterpreterContext *ic = c->ic;
    ObjectPtr code,*src,*dst;
    long size;
    
    /* generate the argument frame */
    putcbyte(c,OP_AFRAME);
    putcbyte(c,ArgumentCount(c));
    putcbyte(c,0);
    
    /* compile the code */
    putcbyte(c,OP_PUSH);
    frequire(c,'{');
    do_block(c);
    putcbyte(c,OP_RETURN);

    /* make the bytecode array */
    code = NewStringObject(ic,c->cbase,c->cptr - c->cbase);
    
    /* make the literal vector */
    size = c->lptr - c->lbase;
    code = NewCompiledCodeObject(ic,FirstLiteral + size,code);
    src = VectorDataAddress(c->literalbuf) + c->lbase;
    dst = VectorDataAddress(code) + FirstLiteral;
    while (--size >= 0)
        *dst++ = *src++;
    
    /* pop the current argument frame and buffer pointers */
    PopArgFrame(c);
    c->cptr = c->cbase; c->cbase = sc->oldcbase;
    c->lptr = c->lbase; c->lbase = sc->oldlbase;
    c->blockLevel = sc->oldLevel;
    
    /* return the function */
    return code;
}

/* do_call - compile a function call */
static void do_call(CompilerContext *c,PVAL *pv)
{
    int tkn,n=0;
    
    /* get the value of the function */
    rvalue(c,pv);

    /* compile each argument expression */
    if ((tkn = token(c)) != ')') {
	stoken(c,tkn);
	do {
	    putcbyte(c,OP_PUSH);
	    do_expr2(c,pv); rvalue(c,pv);
	    ++n;
	} while ((tkn = token(c)) == ',');
    }
    require(c,tkn,')');
    putcbyte(c,OP_CALL);
    putcbyte(c,n);

    /* we've got an rvalue now */
    pv->fcn = NULL;
}

/* do_send - compile a message sending expression */
static void do_send(CompilerContext *c,PVAL *pv)
{
    char selector[100];
    int selLiteral;
    int tkn,n=2;
    
    /* compile the receiver expression */
    if ((tkn = token(c)) == T_SUPER) {
	if (!load_argument(c,"self"))
	    parse_error(c,"Use of 'super' outside of a method");
	putcbyte(c,OP_PUSH);
	load_argument(c,"_next");
    }
    else {
	stoken(c,tkn);
	do_expr(c);
	putcbyte(c,OP_DUP);
    }

    /* generate code to push the selector */
    putcbyte(c,OP_PUSH);
    code_literal(c,selLiteral = addliteral(c,NULL));

    /* get the first part of the selector */
    frequire(c,T_IDENTIFIER);
    strcpy(selector,c->t_token);

    /* get each formal argument and each part of the message selector */
    while ((tkn = token(c)) != ']') {
	if (tkn == ':')
	    strcat(selector,":");
	else
	    stoken(c,tkn);
	putcbyte(c,OP_PUSH);
	do_expr(c);
	++n;
	if ((tkn = token(c)) == ']')
	    break;
	require(c,tkn,T_IDENTIFIER);
	strcat(selector,c->t_token);
    }
    
    /* fixup the selector literal */
    SetVectorElement(c->literalbuf,c->lbase + selLiteral,InternCString(c->ic,selector));
    
    /* send the message */
    putcbyte(c,OP_CALL);
    putcbyte(c,n);
    pv->fcn = NULL;
}

/* do_index - compile an indexing operation */
static void do_index(CompilerContext *c,PVAL *pv)
{
    rvalue(c,pv);
    putcbyte(c,OP_PUSH);
    do_expr(c);
    frequire(c,']');
    pv->fcn = code_index;
}

/* get_id_list - get a comma separated list of identifiers */
static int get_id_list(CompilerContext *c,ATABLE *atable,char *term)
{
    int tkn,cnt=0;
    tkn = token(c);
    if (!strchr(term,tkn)) {
	stoken(c,tkn);
	do {
	    frequire(c,T_IDENTIFIER);
	    AddArgument(c,atable,c->t_token);
	    ++cnt;
	} while ((tkn = token(c)) == ',');
    }
    stoken(c,tkn);
    return cnt;
}

/* AddArgument - add a formal argument */
static void AddArgument(CompilerContext *c,ATABLE *atable,char *name)
{
    ARGUMENT *arg;
    if ((arg = (ARGUMENT *)malloc(sizeof(ARGUMENT))) == NULL)
	InsufficientMemory(c->ic);
    arg->arg_name = copystring(c,name);
    arg->arg_next = atable->at_arguments;
    atable->at_arguments = arg;
}

/* MakeArgFrame - make a new argument frame */
static ATABLE *MakeArgFrame(CompilerContext *c)
{
    ATABLE *atable;
    if ((atable = (ATABLE *)malloc(sizeof(ATABLE))) == NULL)
	InsufficientMemory(c->ic);
    atable->at_arguments = NULL;
    atable->at_next = NULL;
    return (atable);
}

/* PushArgFrame - push an argument frame onto the stack */
static void PushArgFrame(CompilerContext *c,ATABLE *atable)
{
    atable->at_next = c->arguments;
    c->arguments = atable;
}

/* PushNewArgFrame - push a new argument frame onto the stack */
static void PushNewArgFrame(CompilerContext *c)
{
    PushArgFrame(c,MakeArgFrame(c));
}

/* PopArgFrame - push an argument frame off the stack */
static void PopArgFrame(CompilerContext *c)
{
    ARGUMENT *arg,*nxt;
    ATABLE *atable;
    for (arg = c->arguments->at_arguments; arg != NULL; arg = nxt) {
	nxt = arg->arg_next;
	free(arg->arg_name);
	free((char *)arg);
	arg = nxt;
    }
    atable = c->arguments->at_next;
    free((char *)c->arguments);
    c->arguments = atable;
}

/* FreeArguments - free all argument frames */
static void FreeArguments(CompilerContext *c)
{
    while (c->arguments)
	PopArgFrame(c);
}

/* FindArgument - find an argument offset */
static int FindArgument(CompilerContext *c,char *name,int *plev,int *poff)
{
    ATABLE *table;
    ARGUMENT *arg;
    int lev,off;
    lev = 0;
    for (table = c->arguments; table != NULL; table = table->at_next) {
	off = FirstEnvElement;
	for (arg = table->at_arguments; arg != NULL; arg = arg->arg_next) {
	    if (strcmp(name,arg->arg_name) == 0) {
		*plev = lev;
		*poff = off;
		return (TRUE);
	    }
	    ++off;
	}
	++lev;
    }
    return (FALSE);
}

/* ArgumentCount - get the number of arguments */
static int ArgumentCount(CompilerContext *c)
{
    ATABLE *table = c->arguments;
    ARGUMENT *arg;
    int count = 0;
    for (arg = table->at_arguments; arg != NULL; arg = arg->arg_next)
        ++count;
    return count;
}

/* addliteral - add a literal to the literal vector */
static int addliteral(CompilerContext *c,ObjectPtr lit)
{
    long p;
    for (p = c->lbase; p < c->lptr; ++p)
	if (VectorElement(c->literalbuf,p) == lit)
	    return (int)(FirstLiteral + (p - c->lbase));
    if (c->lptr >= c->ltop)
	parse_error(c,"too many literals");
    SetVectorElement(c->literalbuf,p = c->lptr++,lit);
    return (int)(FirstLiteral + (p - c->lbase));
}

/* frequire - fetch a token and check it */
static void frequire(CompilerContext *c,int rtkn)
{
    require(c,token(c),rtkn);
}

/* require - check for a required token */
static void require(CompilerContext *c,int tkn,int rtkn)
{
    char msg[100],tknbuf[100];
    if (tkn != rtkn) {
	strcpy(tknbuf,tkn_name(rtkn));
	sprintf(msg,"Expecting '%s', found '%s'",tknbuf,tkn_name(tkn));
	parse_error(c,msg);
    }
}

/* do_lit_integer - compile a literal integer */
static void do_lit_integer(CompilerContext *c,long n)
{
    code_literal(c,addliteral(c,BoxNumber(n)));
}

/* do_lit_string - compile a literal string */
static void do_lit_string(CompilerContext *c,char *str)
{
    code_literal(c,make_lit_string(c,str));
}

/* do_lit_symbol - compile a literal symbol */
static void do_lit_symbol(CompilerContext *c,char *pname)
{
    code_literal(c,make_lit_symbol(c,pname));
}

/* make_lit_string - make a literal string */
static int make_lit_string(CompilerContext *c,char *str)
{
    return addliteral(c,NewCStringObject(c->ic,str));
}

/* make_lit_symbol - make a literal reference to a symbol */
static int make_lit_symbol(CompilerContext *c,char *pname)
{
    return addliteral(c,InternCString(c->ic,pname));
}

/* findvariable - find a variable */
static void findvariable(CompilerContext *c,char *id,PVAL *pv)
{    
    int lev,off;
    if (strcmp(id,"t") == 0) {
	pv->fcn = code_constant;
	pv->val = OP_T;
    }
    else if (strcmp(id,"NULL") == 0) {
	pv->fcn = code_constant;
	pv->val = OP_NIL;
    }
    else if (FindArgument(c,c->t_token,&lev,&off)) {
	pv->fcn = code_argument;
	pv->val = lev;
	pv->val2 = off;
    }
    else {
	pv->fcn = code_variable;
	pv->val = make_lit_symbol(c,c->t_token);
    }
}
    		
/* code_constant - compile a constant reference */
static void code_constant(CompilerContext *c,int fcn,PVAL *pv)
{
    switch (fcn) {
    case LOAD:	putcbyte(c,pv->val);
    		break;
    case STORE:	CallErrorHandler(c->ic,ecStoreIntoConstant,c);
    		break;
    }
}

/* load_argument - compile code to load an argument */
static int load_argument(CompilerContext *c,char *name)
{
    int lev,off;
    if (!FindArgument(c,name,&lev,&off))
	return FALSE;
    putcbyte(c,OP_EREF);
    putcbyte(c,lev);
    putcbyte(c,off);
    return TRUE;
}

/* code_argument - compile an argument (environment) reference */
static void code_argument(CompilerContext *c,int fcn,PVAL *pv)
{
    switch (fcn) {
    case LOAD:	putcbyte(c,OP_EREF);
    		putcbyte(c,pv->val);
    		putcbyte(c,pv->val2);
    		break;
    case STORE:	putcbyte(c,OP_ESET);
    		putcbyte(c,pv->val);
    		putcbyte(c,pv->val2);
    		break;
    }
}

/* code_property - compile a property reference */
static void code_property(CompilerContext *c,int fcn,PVAL *pv)
{
    switch (fcn) {
    case LOAD:	putcbyte(c,OP_GETP);
    		break;
    case STORE:	putcbyte(c,OP_SETP);
    		break;
    case PUSH:	putcbyte(c,OP_PUSH);
		break;
    case DUP:  	putcbyte(c,OP_DUP2);
	    	break;
    }
}

/* code_variable - compile a variable reference */
static void code_variable(CompilerContext *c,int fcn,PVAL *pv)
{
    switch (fcn) {
    case LOAD:	putcbyte(c,OP_GREF);
    		putcword(c,pv->val);
    		break;
    case STORE:	putcbyte(c,OP_GSET);
    		putcword(c,pv->val);
    		break;
    }
}

/* code_index - compile an indexed reference */
static void code_index(CompilerContext *c,int fcn,PVAL *pv)
{
    switch (fcn) {
    case LOAD: 	putcbyte(c,OP_VREF);
    		break;
    case STORE:	putcbyte(c,OP_VSET);
    		break;
    case PUSH:	putcbyte(c,OP_PUSH);
		break;
    case DUP:  	putcbyte(c,OP_DUP2);
	    	break;
    }
}

/* code_literal - compile a literal reference */
static void code_literal(CompilerContext *c,int n)
{
    putcbyte(c,OP_LIT);
    putcword(c,n);
}

/* codeaddr - get the current code address (actually, offset) */
static int codeaddr(CompilerContext *c)
{
    return (c->cptr - c->cbase);
}

/* putcbyte - put a code byte into the code buffer */
static void putcbyte(CompilerContext *c,int b)
{
    if (c->cptr >= c->ctop)
	CallErrorHandler(c->ic,ecTooMuchCode,c);
    *c->cptr++ = b;
}

/* putcword - put a code word into the code buffer */
static void putcword(CompilerContext *c,int w)
{
    putcbyte(c,w);
    putcbyte(c,w >> 8);
}

/* fixup - fixup a reference chain */
static void fixup(CompilerContext *c,int chn,int val)
{
    int hval,nxt;
    for (hval = val >> 8; chn != NIL; chn = nxt) {
	nxt = (c->cbase[chn] & 0xFF) | (c->cbase[chn+1] << 8);
	c->cbase[chn] = val;
	c->cbase[chn+1] = hval;
    }
}

/* copystring - make a copy of a string */
static char *copystring(CompilerContext *c,char *str)
{
    char *new;
    if ((new = malloc(strlen(str)+1)) == NULL)
	InsufficientMemory(c->ic);
    strcpy(new,str);
    return (new);
}
