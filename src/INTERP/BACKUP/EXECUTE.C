/* execute.c - bytecode interpreter */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include "objects.h"
#include "streams.h"
#include "execute.h"
#include "boberr.h"

/* top continuation dispatch */
static void TopRestore(InterpreterContext *c);
static ObjectPtr *TopCopy(InterpreterContext *c,ObjectPtr **pfp);
static CDispatch TopDispatch = {
    TopRestore,
    TopCopy
};

/* call continuation dispatch */
static void CallRestore(InterpreterContext *c);
static ObjectPtr *CallCopy(InterpreterContext *c,ObjectPtr **pfp);
static CDispatch CallDispatch = {
    CallRestore,
    CallCopy
};

/* call continuation offsets */
#define CallContDispatch	0
#define CallContFP		1
#define CallContEnv		2
#define CallContCode		3
#define CallContPC		4
#define CallContArgC		5
#define CallContStackEnv	6

/* frame continuation dispatch */
static void FrameRestore(InterpreterContext *c);
static ObjectPtr *FrameCopy(InterpreterContext *c,ObjectPtr **pfp);
static CDispatch FrameDispatch = {
    FrameRestore,
    FrameCopy
};

/* frame continuation offsets */
#define FrameContDispatch	0
#define FrameContFP		1
#define FrameContEnv		2
#define FrameContStackEnv	3

/* prototypes */
static ObjectPtr ExecuteFunction(InterpreterContext *c,ObjectPtr fun,int argc,va_list ap);
static void Interpret(InterpreterContext *c);
static void SendMessage(InterpreterContext *c,int n);
static void Call(InterpreterContext *c,CDispatch *d,int n);
static void Frame(InterpreterContext *c);
static ObjectPtr UnstackEnv(InterpreterContext *c,ObjectPtr env);
static void UnboundVariable(InterpreterContext *c,ObjectPtr var);
static void IndexOutOfBounds(InterpreterContext *c,ObjectPtr i);
static void NoMethod(InterpreterContext *c,ObjectPtr sel);
static void BadOpcode(InterpreterContext *c,int opcode);
static ObjectPtr ConcatenateStrings(InterpreterContext *c,ObjectPtr str1,ObjectPtr str2);

/* EnterBuiltInFunction - add a built-in function to the symbol table */
void EnterBuiltInFunction(InterpreterContext *c,char *name,CMethodHandlerPtr handler)
{
    cpush(c,InternCString(c,name));
    SetSymbolValue(top(c),NewCMethodObject(c,name,handler));
    drop(c,1);
}

/* CallFunction - call a function */
ObjectPtr CallFunction(InterpreterContext *c,ObjectPtr fun,int argc,...)
{
    ObjectPtr result;
    va_list ap;
    
    /* execute the function call */
    va_start(ap,argc);
    result = ExecuteFunction(c,fun,argc,ap);
    va_end(ap);
    
    /* return the result */
    return result;
}

/* CallFunctionByName - call a function by name */
ObjectPtr CallFunctionByName(InterpreterContext *c,char *fname,int argc,...)
{
    ObjectPtr result;
    va_list ap;
    
    /* execute the function call */
    va_start(ap,argc);
    result = ExecuteFunction(c,InternCString(c,fname),argc,ap);
    va_end(ap);
    
    /* return the result */
    return result;
}

/* ExecuteFunction - execute code */
static ObjectPtr ExecuteFunction(InterpreterContext *c,ObjectPtr fun,int argc,va_list ap)
{
    InterpreterTarget target;
    int sts,n;
    
    /* setup the interpreter target */
    target.next = c->interpreterTarget;
    c->interpreterTarget = &target;
    if ((sts = setjmp(target.target)) != itStart) {
        c->interpreterTarget = target.next;
    	switch (sts) {
    	case itReturn:
    	    return pop(c);
        case itAbort:
            return c->nilObject;
        }
    }
        
    /* reserve space for the result, function and arguments */
    check(c,argc + 2);

    /* make a place for the result */
    push(c,c->nilObject);
    
    /* push the function and arguments */
    push(c,SymbolP(fun) ? SymbolValue(fun) : fun);
    for (n = argc; --n >= 0; )
        push(c,va_arg(ap,ObjectPtr));
    va_end(ap);
    
    /* setup the call */
    Call(c,&TopDispatch,argc);
    
    /* call the interpreter */
    Interpret(c);
    return 0; /* never reached */
}
    
/* Interpret - interpret bytecodes */
static void Interpret(InterpreterContext *c)
{
    ObjectPtr p2,p3,p1,*p;
    unsigned int off;
    long n;
    int i;

    /* the instruction dispatch loop */
    for (;;) {
	/*DecodeInstruction(c,c->code,c->pc - c->cbase,c->standardOutput);*/
	switch (*c->pc++) {
	case OP_CALL:
		i = *c->pc++;
		if (ObjectP(c->sp[i]))
		    SendMessage(c,i);
		Call(c,&CallDispatch,i);
		break;
	case OP_RETURN:
	case OP_UNFRAME:
		(*((CDispatch *)*c->fp)->restore)(c);
		break;
	case OP_FRAME:
		Frame(c);
		break;
	case OP_AFRAME:	/* handled by OP_CALL */
	case OP_AFRAMER:
		BadOpcode(c,c->pc[-1]);
		break;
	case OP_CLOSE:
		c->env = UnstackEnv(c,c->env);
		settop(c,NewMethodObject(c,top(c),c->env));
		break;
	case OP_EREF:
		i = *c->pc++;
		for (p1 = c->env; --i >= 0; )
		    p1 = EnvNextFrame(p1);
		i = *c->pc++;
		settop(c,EnvElement(p1,i));
		break;
	case OP_ESET:
		i = *c->pc++;
		for (p1 = c->env; --i >= 0; )
		    p1 = EnvNextFrame(p1);
		i = *c->pc++;
		SetEnvElement(p1,i,top(c));
		break;
	case OP_BRT:
		off = *c->pc++;
		off |= *c->pc++ << 8;
		if (top(c) != c->falseObject)
		    c->pc = c->cbase + off;
		break;
	case OP_BRF:
		off = *c->pc++;
		off |= *c->pc++ << 8;
		if (top(c) == c->falseObject)
		    c->pc = c->cbase + off;
		break;
	case OP_BR:
		off = *c->pc++;
		off |= *c->pc++ << 8;
		c->pc = c->cbase + off;
		break;
	case OP_T:
		settop(c,c->trueObject);
		break;
	case OP_NIL:
		settop(c,c->nilObject);
		break;
	case OP_PUSH:
		cpush(c,c->nilObject);
		break;
	case OP_NOT:
		settop(c,top(c) == c->falseObject ? c->trueObject : c->falseObject);
		break;
	case OP_NEG:
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(-UnboxNumber(top(c))));
		break;
	case OP_ADD:
		p2 = pop(c);
		if (NumberP(p2)) {
		    if (!NumberP(top(c))) TypeError(c,top(c));
		    settop(c,BoxNumber(UnboxNumber(top(c)) + UnboxNumber(p2)));
		}
		else if (StringP(p2)) {
		    if (!StringP(top(c))) TypeError(c,top(c));
		    settop(c,ConcatenateStrings(c,top(c),p2));
		}
		else
		    TypeError(c,p2);
		break;
	case OP_SUB:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) - UnboxNumber(p2)));
		break;
	case OP_MUL:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) * UnboxNumber(p2)));
		break;
	case OP_DIV:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		n = UnboxNumber(p2);
		settop(c,BoxNumber(n == 0 ? 0 : UnboxNumber(top(c)) / n));
		break;
	case OP_REM:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		n = UnboxNumber(p2);
		settop(c,BoxNumber(n == 0 ? 0 : UnboxNumber(top(c)) % n));
		break;
	case OP_INC:
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) + 1));
		break;
	case OP_DEC:
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) - 1));
		break;
	case OP_BAND:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) & UnboxNumber(p2)));
		break;
	case OP_BOR:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) | UnboxNumber(p2)));
		break;
	case OP_XOR:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) ^ UnboxNumber(p2)));
		break;
	case OP_BNOT:
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(~UnboxNumber(top(c))));
		break;
	case OP_SHL:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) << UnboxNumber(p2)));
		break;
	case OP_SHR:
		p2 = pop(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (!NumberP(top(c))) TypeError(c,top(c));
		settop(c,BoxNumber(UnboxNumber(top(c)) >> UnboxNumber(p2)));
		break;
	case OP_LT:
		p2 = pop(c);
		settop(c,CompareObjects(c,top(c),p2) < 0 ? c->trueObject : c->falseObject);
		break;
	case OP_LE:
		p2 = pop(c);
		settop(c,CompareObjects(c,top(c),p2) <= 0 ? c->trueObject : c->falseObject);
		break;
	case OP_EQ:
		p2 = pop(c);
		settop(c,Eql(top(c),p2) ? c->trueObject : c->falseObject);
		break;
	case OP_NE:
		p2 = pop(c);
		settop(c,Eql(top(c),p2) ? c->falseObject : c->trueObject);
		break;
	case OP_GE:
		p2 = pop(c);
		settop(c,CompareObjects(c,top(c),p2) >= 0 ? c->trueObject : c->falseObject);
		break;
	case OP_GT:
		p2 = pop(c);
		settop(c,CompareObjects(c,top(c),p2) > 0 ? c->trueObject : c->falseObject);
		break;
	case OP_LIT:
		off = *c->pc++;
		off |= *c->pc++ << 8;
		settop(c,CompiledCodeLiteral(c->code,off));
		break;
	case OP_GREF:
		off = *c->pc++;
		off |= *c->pc++ << 8;
		p1 = CompiledCodeLiteral(c->code,off);
		if ((p2 = SymbolValue(p1)) == c->unboundObject) {
		    UnboundVariable(c,p1);
			return;
		}
		settop(c,p2);
		break;
	case OP_GSET:
		off = *c->pc++;
		off |= *c->pc++ << 8;
		SetSymbolValue(CompiledCodeLiteral(c->code,off),top(c));
		break;
	case OP_GETP:
		p2 = pop(c);
		p1 = top(c);
		if (!ObjectP(p1)) TypeError(c,p1);
		settop(c,GetProperty(c,p1,p2));
		off -= 2;	/* my workaround for member functions */
		break;
	case OP_SETP:
		p3 = pop(c);
		p2 = pop(c);
		p1 = top(c);
		if (!ObjectP(p1)) TypeError(c,p1);
		settop(c,p3);
		SetProperty(c,p1,p2,p3);
		break;
	case OP_VREF:
		p2 = pop(c);
		p1 = top(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (StringP(p1)) {
		    if ((n = UnboxNumber(p2)) < 0 || n >= StringSize(p1))
			IndexOutOfBounds(c,p2);
		    settop(c,BoxNumber(StringElement(p1,n)));
		}
		else if (VectorP(p1)) {
		    if ((n = UnboxNumber(p2)) < 0 || n >= VectorSize(p1))
			IndexOutOfBounds(c,p2);
		    settop(c,VectorElement(p1,n));
		}
		else
		    TypeError(c,p1);
		break;
	case OP_VSET:
		p3 = pop(c);
		p2 = pop(c);
		p1 = top(c);
		if (!NumberP(p2)) TypeError(c,p2);
		if (StringP(p1)) {
		    if (!NumberP(p3)) TypeError(c,p3);
		    if ((n = UnboxNumber(p2)) < 0 || n >= StringSize(p1))
			IndexOutOfBounds(c,p2);
		    settop(c,BoxNumber((long)SetStringElement(p1,n,(int)UnboxNumber(p3))));
		}
		else if (VectorP(p1)) {
		    if ((n = UnboxNumber(p2)) < 0 || n >= VectorSize(p1))
			IndexOutOfBounds(c,p2);
		    settop(c,SetVectorElement(p1,n,p3));
		}
		else
		    TypeError(c,p1);
		break;
	case OP_DUP:
		check(c,1);
		--c->sp;
		settop(c,c->sp[1]);
		break;
	case OP_DUP2:
		check(c,2);
		c->sp -= 2;
		settop(c,c->sp[2]);
		c->sp[1] = c->sp[3];
		break;
	case OP_DROP:
		++c->sp;
		break;
	case OP_NEWOBJECT:
		settop(c,NewObject(c,top(c)));
		break;
	case OP_NEWLIST:
		n = UnboxNumber(pop(c));
		p1 = c->nilObject;
		while (--n >= 0)
		    p1 = Cons(c,pop(c),p1);
		push(c,p1);
		break;
	case OP_NEWVECTOR:
		n = UnboxNumber(pop(c));
		p1 = NewVectorObject(c,n);
		p = VectorDataAddress(p1) + n;
		while (--n >= 0)
		    *--p = pop(c);
		push(c,p1);
		break;
	default:
		BadOpcode(c,c->pc[-1]);
		break;
	    }
	}
}

/* SendMessage - setup to send a message */
static void SendMessage(InterpreterContext *c,int n)
{
    ObjectPtr next = c->sp[n-1];
    ObjectPtr selector = c->sp[n-2];
    ObjectPtr methodBinding = NULL;
    while (next && ObjectP(next) && (methodBinding = GetLocalProperty(c,next,selector)) == NULL)
	next = ObjectClass(next);
    if (methodBinding == NULL)
	NoMethod(c,selector);
    c->sp[n-1] = c->sp[n];
    if ((c->sp[n-2] = ObjectClass(next)) == NULL) c->sp[n-2] = c->nilObject;
    c->sp[n] = Cdr(methodBinding);
}

/* TopRestore - restore a top continuation */
static void TopRestore(InterpreterContext *c)
{
    CallRestore(c);
    longjmp(c->interpreterTarget->target,itReturn);
}

/* TopCopy - copy a top continuation during garbage collection */
static ObjectPtr *TopCopy(InterpreterContext *c,ObjectPtr **pfp)
{
    return CallCopy(c,pfp);
}

/* CallRestore - restore a call continuation */
static void CallRestore(InterpreterContext *c)
{
    ObjectPtr env = c->env;
    ObjectPtr value = top(c);
    long pcoff;
    
    /* restore the stack */
    c->sp = c->fp + 1;
    
    /* restore the previous frame */
    c->fp = (ObjectPtr *)pop(c);
    c->env = pop(c);
    c->code = pop(c);
    pcoff = (long)pop(c);
    if (c->code != NULL) {
        c->cbase = StringDataAddress(CompiledCodeBytecodes(c->code));
        c->pc = c->cbase + pcoff;
    }
    drop(c,1);
    
    /* fixup moved environments */
    if (c->env && MovedEnvironmentP(c->env))
        c->env = MovedEnvForwardingAddr(c->env);

    /* reset the stack pointer */
    drop(c,StackEnvHeaderSize + VectorSize(env));
    
    /* leave the value on the stack */
    settop(c,value);
}

/* CallCopy - copy a call continuation during garbage collection */
static ObjectPtr *CallCopy(InterpreterContext *c,ObjectPtr **pfp)
{
    ObjectPtr *fp = *pfp;
    ObjectPtr env = &fp[CallContStackEnv];
    ObjectPtr *data = VectorDataAddress(env);
    long count = VectorSize(env);
    fp[CallContEnv] = CopyObject(c,fp[CallContEnv]);
    if (fp[CallContCode] != NULL)
        fp[CallContCode] = CopyObject(c,fp[CallContCode]);
    for (; --count >= 0; ++data)
        *data = CopyObject(c,*data);
    *data = CopyObject(c,*data);
    *pfp = fp[CallContFP];
    return data + 1;
}

/* Call - setup to call a function */
static void Call(InterpreterContext *c,CDispatch *d,int argc)
{
    int rflag,rargc,oargc,targc,n;
    ObjectPtr method = c->sp[argc];
    unsigned char *cbase,*pc;
    ObjectPtr code,env;

#ifdef _DEBUG

	int check1, check2;

	if( (((long)method & 1) == 0) ) 
		check1 = 1;
	else
		check1 = 0;

	if( NumberP(method) )
		&NumberDispatch;
	else {
		if( (((ObjectHdr *)method)->dispatch) == &MethodDispatch )
			check2 = 1;
		else
			check2 = 0;
	}
	if (CMethodP(method)) {
		ObjectPtr val = CMethodHandler(method)(c,argc,&c->sp[argc]);
		drop(c,argc);
		settop(c,val);
		return;
	}
	else if( !(check1 && check2) )
		TypeError(c,method);

#else
    
    /* handle built-in methods */
    if (CMethodP(method)) {
	ObjectPtr val = CMethodHandler(method)(c,argc,&c->sp[argc]);
	drop(c,argc);
	settop(c,val);
	return;
    }
    
    /* otherwise, it had better be a bytecode method */
    else if (!MethodP(method))
	TypeError(c,method);

#endif

    /* get the code object */
    code = MethodCode(method);
    cbase = pc = StringDataAddress(CompiledCodeBytecodes(code));
    
    /* parse the argument frame instruction */
    rflag = *pc++ == OP_AFRAMER;
    rargc = *pc++; oargc = *pc++;
    targc = rargc + oargc;

    /* check the argument count */
    if (argc < rargc)
        TooFewArguments(c);
    else if (!rflag && argc > rargc + oargc)
        TooManyArguments(c);
        
    /* fill out the optional arguments */
    if ((n = targc - argc) > 0) {
        check(c,n);
        while (--n >= 0)
            push(c,c->nilObject);
    }
    
    /* build the rest argument */
    if (rflag) {
        ObjectPtr value = c->nilObject;
        while (--argc >= targc)
            value = Cons(c,pop(c),value);
        cpush(c,value);
        ++targc;
    }
    
    /* reserve space for the call frame */
    check(c,StackEnvHeaderSize + FirstEnvElement + 6);
    
    /* construct the environment frame */
    push(c,c->nilObject);	/* Names */
    push(c,MethodEnv(method));	/* NextFrame */
    c->sp -= StackEnvHeaderSize;
    env = (ObjectPtr)c->sp;
    SetObjectDispatch(env,&StackEnvironmentDispatch);
    SetVectorSize(env,FirstEnvElement + targc);
    
    /* push the continuation */
    push(c,(ObjectPtr)argc);
    push(c,(ObjectPtr)(c->pc - c->cbase));
    push(c,c->code);
    push(c,c->env);
    push(c,(ObjectPtr)c->fp);
    push(c,(ObjectPtr)d);
    
    /* establish the new frame */
    c->fp = c->sp;

    /* setup the new method */
    c->env = env;
    c->code = code;
    c->cbase = cbase;
    c->pc = pc;
}

/* FrameRestore - restore a frame continuation */
static void FrameRestore(InterpreterContext *c)
{
    ObjectPtr env = c->env;
    ObjectPtr value = top(c);
    
    /* restore the stack */
    c->sp = c->fp + 1;
    
    /* restore the previous frame */
    c->fp = (ObjectPtr *)pop(c);
    c->env = pop(c);

    /* fixup moved environments */
    if (c->env && MovedEnvironmentP(c->env))
        c->env = MovedEnvForwardingAddr(c->env);

    /* reset the stack pointer */
    drop(c,StackEnvHeaderSize + VectorSize(env));
    
    /* leave the value on the stack */
    settop(c,value);
}

/* FrameCopy - copy a frame continuation during garbage collection */
static ObjectPtr *FrameCopy(InterpreterContext *c,ObjectPtr **pfp)
{
    ObjectPtr *fp = *pfp;
    ObjectPtr env = &fp[FrameContStackEnv];
    ObjectPtr *data = VectorDataAddress(env);
    long count = VectorSize(env);
    fp[FrameContEnv] = CopyObject(c,fp[FrameContEnv]);
    for (; --count >= 0; ++data)
        *data = CopyObject(c,*data);
    *pfp = fp[FrameContFP];
    return data;
}

/* Frame - push a frame on the stack */
static void Frame(InterpreterContext *c)
{
    int n = *c->pc++;
    ObjectPtr env;
    
    /* reserve space for the call frame */
    check(c,StackEnvHeaderSize + FirstEnvElement + 3);
    
    /* construct the environment frame */
    push(c,c->nilObject);	/* Names */
    push(c,c->env);		/* NextFrame */
    c->sp -= StackEnvHeaderSize;
    env = (ObjectPtr)c->sp;
    SetObjectDispatch(env,&StackEnvironmentDispatch);
    SetVectorSize(env,n + 1);
    
    /* push the continuation */
    push(c,c->env);
    push(c,(ObjectPtr)c->fp);
    push(c,(ObjectPtr)&FrameDispatch);
    
    /* establish the new frame */
    c->fp = c->sp;
    c->env = env;
}

/* UnstackEnv - unstack the environment */
static ObjectPtr UnstackEnv(InterpreterContext *c,ObjectPtr env)
{
    ObjectPtr new,*src,*dst;
    long size;

    /* initialize */
    check(c,3);
    push(c,c->nilObject);
    push(c,c->nilObject);

    /* copy each stack environment frame to the heap */
    while (StackEnvironmentP(env)) {

	/* allocate a new frame */
	push(c,env);
	size = VectorSize(env);
	new = NewEnvironmentObject(c,size);
	env = pop(c);
	
	/* copy the data */
	src = VectorDataAddress(env);
	dst = VectorDataAddress(new);
	while (--size >= 0)
	    *dst++ = *src++;

	/* link the new frame into the new environment */
	if (top(c) == c->nilObject)
	    c->sp[1] = new;
	else
	    SetEnvNextFrame(top(c),new);
	settop(c,new);
	
	/* get next frame */
	push(c,EnvNextFrame(env));
	
	/* store the forwarding address */
	SetObjectDispatch(env,&MovedEnvironmentDispatch);
	SetMovedEnvForwardingAddr(env,new);
	
	/* move ahead to the next frame */
	env = pop(c);
    }

    /* link the first heap frame into the new environment */
    if (top(c) == c->nilObject)
	c->sp[1] = env;
    else
	SetEnvNextFrame(top(c),env);
    drop(c,1);

    /* return the new environment */
    return pop(c);
}

void TypeError(InterpreterContext *c,ObjectPtr v)
{
    CallErrorHandler(c,ecTypeError,v);
}

void TooManyArguments(InterpreterContext *c)
{
    CallErrorHandler(c,ecTooManyArguments,0);
}

void TooFewArguments(InterpreterContext *c)
{
    CallErrorHandler(c,ecTooFewArguments,0);
}

static void UnboundVariable(InterpreterContext *c,ObjectPtr var)
{
    CallErrorHandler(c,ecUnboundVariable,var);
}

static void IndexOutOfBounds(InterpreterContext *c,ObjectPtr i)
{
    CallErrorHandler(c,ecIndexOutOfBounds,i);
}

static void NoMethod(InterpreterContext *c,ObjectPtr sel)
{
    CallErrorHandler(c,ecNoMethod,sel);
}

static void BadOpcode(InterpreterContext *c,int opcode)
{
    CallErrorHandler(c,ecBadOpcode,(void *)opcode);
}

/* CompareObjects - compare two objects */
int CompareObjects(InterpreterContext *c,ObjectPtr obj1,ObjectPtr obj2)
{
    if (NumberP(obj1)) {
	long diff;
	if (!NumberP(obj2))
	    TypeError(c,obj2);
	diff = UnboxNumber(obj1) - UnboxNumber(obj2);
	return diff < 0 ? -1 : diff == 0 ? 0 : 1;
    }
    else if (StringP(obj1)) {
	if (!StringP(obj2))
	    TypeError(c,obj2);
	return CompareStrings(obj1,obj2);
    }
    TypeError(c,obj1);
    return 0; /* never reached */
}

/* ConcatenateStrings - concatenate two strings */
static ObjectPtr ConcatenateStrings(InterpreterContext *c,ObjectPtr str1,ObjectPtr str2)
{
    unsigned char *src,*dst;
    long len1 = StringSize(str1);
    long len2 = StringSize(str2);
    long len = len1 + len2;
    ObjectPtr new;
    check(c,2);
    push(c,str1);
    push(c,str2);
    new = NewStringObject(c,NULL,len);
    dst = StringDataAddress(new);
    src = StringDataAddress(c->sp[1]);
    while (--len1 >= 0)
	*dst++ = *src++;
    src = StringDataAddress(top(c));
    while (--len2 >= 0)
	*dst++ = *src++;
    drop(c,2);
    return new;
}

/* AbortInterpreter - abort the interpreter */
void AbortInterpreter(InterpreterContext *c)
{
    longjmp(c->interpreterTarget->target,itAbort);
}

