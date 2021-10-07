/* objects.h - definitions for objects.c */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#ifndef __OBJECTS_H__
#define __OBJECTS_H__

#include "streams.h"
#include <setjmp.h>

/* useful definitions */
#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

/* export keyword for library entry points */
#ifndef EXPORT
#ifdef _WINDLL
#define EXPORT __export
#else
#define EXPORT
#endif
#endif

/* heap object pointer */
typedef void *ObjectPtr;

/* macro to check result of allocating memory */
#define CheckNIL(c,x)	do { if ((x) == NULL) InsufficientMemory(c); } while (0)


/* definitions for the stop and copy collector */

typedef ObjectPtr (*CMethodHandlerPtr)(struct InterpreterContext *c,int n,ObjectPtr *p);

typedef struct {
  unsigned char *base;
  unsigned char *free;
  unsigned char *top;
} MemorySpace;

/* number of pointers in a protected pointer block */
#define PPBSize	100

typedef struct ProtectedPtrBlock {
    struct ProtectedPtrBlock *next;
    ObjectPtr *pointers[PPBSize];
    int count;
} ProtectedPtrBlock;

typedef struct InterpreterTarget {
    struct InterpreterTarget *next;
    jmp_buf target;
} InterpreterTarget;

/* limits */
#define TKNSIZE		255	/* maximum token size */
#define LSIZE		255	/* maximum line size */
#define SSIZE		20	/* break/continue stack size */

typedef struct InterpreterContext {

    /* interpreter variables */
    InterpreterTarget *interpreterTarget;
    ObjectPtr *stack;		/* stack base */
    ObjectPtr *stackTop;	/* stack top */
    ObjectPtr *sp;		/* stack pointer */
    ObjectPtr *fp;		/* frame pointer */
    ObjectPtr code;		/* code object */
    unsigned char *cbase;	/* code base */
    unsigned char *pc;		/* program counter */
    ObjectPtr env;		/* environment */
    ProtectedPtrBlock *protectedPointers;
    CMethodHandlerPtr (*findFunctionHandler)(char *name);
    void (*errorHandler)(struct InterpreterContext *c,int code,void *data);

    /* compiler context (optional) */
    void *cc;
    
    /* memory manager variables */
    MemorySpace *oldSpace;
    MemorySpace *newSpace;
    ObjectPtr nilObject;
    ObjectPtr trueObject;
    ObjectPtr falseObject;
    ObjectPtr objectObject;
    ObjectPtr classNameObject;
    ObjectPtr unboundObject;
    ObjectPtr symbols;
    
    /* environment variables */
    Stream *standardInput;
    Stream *standardOutput;
    Stream *standardError;

	/* extra pointer space */
	void *pextra1;
	void *pextra2;
	char *err;
    
} InterpreterContext;

/* continuation dispatch structure */
typedef struct CDispatch {
    void (*restore)(InterpreterContext *c);
    ObjectPtr *(*copy)(InterpreterContext *c,ObjectPtr **pfp);
} CDispatch;

/* TYPE DISPATCH */

typedef struct TypeDispatch {
  long typeTag;
  char *typeName;
  void (*print)(ObjectPtr obj,char *buf);
  long (*size)(ObjectPtr obj);
  ObjectPtr (*copy)(InterpreterContext *c,ObjectPtr obj);
  void (*scan)(InterpreterContext *c,ObjectPtr obj);
  int (*write)(InterpreterContext *c,ObjectPtr obj,Stream *s);
  int (*read)(InterpreterContext *c,ObjectPtr obj,Stream *s);
  int (*flatten)(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
  ObjectPtr (*unflatten)(InterpreterContext *c,struct TypeDispatch *d,ObjectPtr olist,Stream *s);
} TypeDispatch,*TypeDispatchPtr;

#define BrokenHeartTypeTag	0
#define NumberTypeTag		1
#define ObjectTypeTag		2
#define ConsTypeTag		3
#define SymbolTypeTag		4
#define StringTypeTag		5
#define VectorTypeTag		6
#define MethodTypeTag		7
#define CMethodTypeTag		8
#define FileStreamTypeTag	9
#define StringStreamTypeTag	10
#define ObjectStreamTypeTag	11
#define ObjectStoreTypeTag	12
#define CursorTypeTag		13
#define LineBufferTypeTag	14
#define CompiledCodeTypeTag	15
#define EnvironmentTypeTag	16
#define StackEnvironmentTypeTag	17
#define MovedEnvironmentTypeTag	18


/* OBJECT HEADER */

typedef struct {
  TypeDispatchPtr dispatch;
} ObjectHdr;

#define PointerP(o)			(((long)o & 1) == 0)
#define ObjectDispatch(o)		(NumberP(o) ? &NumberDispatch : QuickDispatch(o))
#define QuickDispatch(o)		(((ObjectHdr *)o)->dispatch)
#define SetObjectDispatch(o,v)		(((ObjectHdr *)o)->dispatch = (v))
#define TypeCheck(o,t)			(PointerP(o) && ObjectDispatch(o) == t)
#define TypeTag(o)			(ObjectDispatch(o)->typeTag)
#define TypeName(o)			(ObjectDispatch(o)->typeName)


/* NUMBER */

#define NumberP(o)			(((long)o & 1) != 0)
#define BoxNumber(n)			((ObjectPtr)(((n) << 1) | 1))
#define UnboxNumber(o)			((long)o >> 1)

extern TypeDispatch NumberDispatch;


/* OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  ObjectPtr o_class;
  ObjectPtr properties;
} Object;

#define ObjectP(o)			TypeCheck(o,&ObjectDispatch)
#define ObjectClass(o)			(ObjectP(XObjectClass(o)) ? XObjectClass(o) : GetObjectClass(o))
#define XObjectClass(o)			(((Object *)o)->o_class)
#define SetObjectClass(o,v)		(((Object *)o)->o_class = (v))
#define ObjectProperties(o)		(((Object *)o)->properties)
#define SetObjectProperties(o,v)	(((Object *)o)->properties = (v))

extern TypeDispatch ObjectDispatch;
ObjectPtr EXPORT NewObject(InterpreterContext *c,ObjectPtr o_class);
ObjectPtr EXPORT CloneObject(InterpreterContext *c,ObjectPtr obj);
ObjectPtr EXPORT GetObjectClass(ObjectPtr obj);


/* CONS OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  ObjectPtr car;
  ObjectPtr cdr;
} ConsObject;

#define ConsP(o)			TypeCheck(o,&ConsDispatch)
#define Car(o)				(((ConsObject *)o)->car)  
#define SetCar(o,v)			(((ConsObject *)o)->car = (v))
#define Cdr(o)				(((ConsObject *)o)->cdr)  
#define SetCdr(o,v)			(((ConsObject *)o)->cdr = (v))

extern TypeDispatch ConsDispatch;
ObjectPtr EXPORT Cons(InterpreterContext *c,ObjectPtr car,ObjectPtr cdr);


/* SYMBOL OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  ObjectPtr printName;
  ObjectPtr value;
} SymbolObject;

#define SymbolP(o)			TypeCheck(o,&SymbolDispatch)
#define SymbolPrintName(o)		(((SymbolObject *)o)->printName)  
#define SetSymbolPrintName(o,v)		(((SymbolObject *)o)->printName = (v))
#define SymbolValue(o)			(((SymbolObject *)o)->value)  
#define SetSymbolValue(o,v)		(((SymbolObject *)o)->value = (v))

extern TypeDispatch SymbolDispatch;
ObjectPtr EXPORT NewSymbolObject(InterpreterContext *c,ObjectPtr printName);


/* STRING OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  long size;
/*unsigned char data[0];*/
} StringObject;

#define StringP(o)			TypeCheck(o,&StringDispatch)
#define StringSize(o)			(((StringObject *)o)->size)  
#define SetStringSize(o,v)		(((StringObject *)o)->size = (v))
#define StringDataAddress(o)		((unsigned char *)o + sizeof(StringObject))
#define StringElement(o,i)		(StringDataAddress(o)[i])  
#define SetStringElement(o,i,v)		(StringDataAddress(o)[i] = (v))  

extern TypeDispatch StringDispatch;
ObjectPtr EXPORT NewStringObject(InterpreterContext *c,unsigned char *data,long size);
ObjectPtr EXPORT NewCStringObject(InterpreterContext *c,char *str);


/* VECTOR OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  long size;
/*ObjectPtr data[0];*/
} VectorObject;

#define VectorP(o)			TypeCheck(o,&VectorDispatch)
#define VectorSize(o)			(((VectorObject *)o)->size)  
#define SetVectorSize(o,v)		(((VectorObject *)o)->size = (v))
#define VectorDataAddress(o)		((ObjectPtr *)((unsigned char *)o + sizeof(VectorObject))) 
#define VectorElement(o,i)		(VectorDataAddress(o)[i])
#define SetVectorElement(o,i,v)		(VectorDataAddress(o)[i] = (v))

extern TypeDispatch VectorDispatch;
ObjectPtr EXPORT NewVectorObject(InterpreterContext *c,long size);


/* COMPILED CODE OBJECT */

typedef VectorObject CompiledCodeObject;

#define CompiledCodeP(o)		TypeCheck(o,&VectorDispatch)
#define CompiledCodeLiteral(o,i)	VectorElement(o,i)
#define SetCompiledCodeLiteral(o,i,v)	SetVectorElement(o,i,v)
#define CompiledCodeBytecodes(o)	VectorElement(o,0)
#define SetCompiledCodeBytecodes(o,v)	SetVectorElement(o,0,v)
#define CompiledCodeName(o)		VectorElement(o,1)
#define CompiledCodeEnvNames(o)		VectorElement(o,2)
#define FirstLiteral			1

extern TypeDispatch CompiledCodeDispatch;
ObjectPtr EXPORT NewCompiledCodeObject(InterpreterContext *c,long size,ObjectPtr bytecodes);


/* ENVIRONMENT OBJECT */

typedef VectorObject EnvironmentObject;

#define EnvironmentP(o)			TypeCheck(o,&EnvironmentDispatch)
#define EnvElement(o,i)			VectorElement(o,i)
#define SetEnvElement(o,i,v)		SetVectorElement(o,i,v)
#define EnvNextFrame(o)			VectorElement(o,0)
#define SetEnvNextFrame(o,v)		SetVectorElement(o,0,v)
#define EnvNames(o)			VectorElement(o,1)
#define SetEnvNames(o,v)		SetVectorElement(o,1,v)
#define FirstEnvElement			2

extern TypeDispatch EnvironmentDispatch;
ObjectPtr EXPORT NewEnvironmentObject(InterpreterContext *c,long size);


/* STACK ENVIRONMENT OBJECT */

typedef VectorObject StackEnvironmentObject;

#define StackEnvironmentP(o)		TypeCheck(o,&StackEnvironmentDispatch)
#define StackEnvHeaderSize	 	((sizeof(StackEnvironmentObject) + sizeof(ObjectPtr) - 1) / sizeof(ObjectPtr))

extern TypeDispatch StackEnvironmentDispatch;


/* MOVED ENVIRONMENT OBJECT */

typedef VectorObject MovedEnvironmentObject;

#define MovedEnvironmentP(o)		TypeCheck(o,&MovedEnvironmentDispatch)
#define MovedEnvForwardingAddr(o)	EnvNextFrame(o)
#define SetMovedEnvForwardingAddr(o,v)	SetEnvNextFrame(o,v)

extern TypeDispatch MovedEnvironmentDispatch;


/* METHOD OBJECT */

typedef ConsObject MethodObject;

#define MethodP(o)			TypeCheck(o,&MethodDispatch)
#define MethodCode(o)			Car(o)  
#define SetMethodCode(o,v)		SetCar(o,v)
#define MethodEnv(o)			Cdr(o)  
#define SetMethodEnv(o,v)		SetCdr(o,v)

extern TypeDispatch MethodDispatch;
ObjectPtr EXPORT NewMethodObject(InterpreterContext *c,ObjectPtr code,ObjectPtr env);


/* C METHOD OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  char *name;
  CMethodHandlerPtr handler;
} CMethodObject;

#define CMethodP(o)			TypeCheck(o,&CMethodDispatch)
#define CMethodName(o)			(((CMethodObject *)o)->name)  
#define SetCMethodName(o,v)		(((CMethodObject *)o)->name = (v))  
#define CMethodHandler(o)		(((CMethodObject *)o)->handler)  
#define SetCMethodHandler(o,v)		(((CMethodObject *)o)->handler = (v))

extern TypeDispatch CMethodDispatch;
ObjectPtr EXPORT NewCMethodObject(InterpreterContext *c,char *name,CMethodHandlerPtr handler);


/* FOREIGN POINTER OBJECT */

typedef struct {
  TypeDispatchPtr dispatch;
  void *ptr;
} ForeignPtrObject;

#define ForeignPtr(o)			(((ForeignPtrObject *)o)->ptr)  
#define SetForeignPtr(o,v)		(((ForeignPtrObject *)o)->ptr = (v))  

#define StreamP(o)			(FileStreamP(o) || StringStreamP(o) || ObjectStreamP(o))

#define FileStreamP(o)			TypeCheck(o,&FileStreamDispatch)
extern TypeDispatch FileStreamDispatch;

#define StringStreamP(o)		TypeCheck(o,&StringStreamDispatch)
extern TypeDispatch StringStreamDispatch;

#define ObjectStreamP(o)		TypeCheck(o,&ObjectStreamDispatch)
extern TypeDispatch ObjectStreamDispatch;

#define ObjectStoreP(o)			TypeCheck(o,&ObjectStoreDispatch)
extern TypeDispatch ObjectStoreDispatch;

#define CursorP(o)			TypeCheck(o,&CursorDispatch)
extern TypeDispatch CursorDispatch;

#define LineBufferP(o)			TypeCheck(o,&LineBufferDispatch)
extern TypeDispatch LineBufferDispatch;

ObjectPtr EXPORT NewForeignPtrObject(InterpreterContext *c,long type,void *ptr);


/* BROKEN HEART */

typedef struct {
  TypeDispatchPtr dispatch;
  ObjectPtr forwardingAddress;
} BrokenHeart;

#define BrokenHeartP(o)			TypeCheck(o,&BrokenHeartDispatch)
#define ForwardingAddress(o)		(((BrokenHeart *)o)->forwardingAddress)  
#define SetForwardingAddress(o,v)	(((BrokenHeart *)o)->forwardingAddress = (v))  

extern TypeDispatch BrokenHeartDispatch;


/* stack manipulation macros */
#define check(c,n)	do { if ((c)->sp - (n) < &(c)->stack[0]) StackOverflow(c); } while (0)
#define cpush(c,v)	do { if ((c)->sp > &(c)->stack[0]) push(c,v); else StackOverflow(c); } while (0)
#define push(c,v)	(*--(c)->sp = (v))
#define top(c)		(*(c)->sp)
#define settop(c,v)	(*(c)->sp = (v))
#define pop(c)		(*(c)->sp++)
#define drop(c,n)	((c)->sp += (n))

/* function table entry structure */
typedef struct {
  char *name;
  CMethodHandlerPtr handler;
} FunctionTableEntry;

/* prototypes */
InterpreterContext * EXPORT NewInterpreterContext(long size,long stackSize);
void EXPORT SetExtraInterpreterContextData( InterpreterContext *c, void *extra1, void *extra2 );
void EXPORT CollectGarbage(InterpreterContext *c);
ObjectPtr CopyObject(InterpreterContext *c,ObjectPtr obj);
void EXPORT ObjectPrint(ObjectPtr obj,char *buf);
int EXPORT FlattenObject(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
ObjectPtr EXPORT UnflattenObject(InterpreterContext *c,ObjectPtr olist,Stream *s);
ObjectPtr EXPORT GetProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag);
ObjectPtr EXPORT GetLocalProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag);
ObjectPtr EXPORT GetInheritedProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag);
void EXPORT SetProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag,ObjectPtr value);
int EXPORT Eql(ObjectPtr obj1,ObjectPtr obj2);
int EXPORT CompareStrings(ObjectPtr str1,ObjectPtr str2);
long EXPORT ListLength(ObjectPtr list);
ObjectPtr EXPORT InternCString(InterpreterContext *c,char *str);
ObjectPtr EXPORT InternSymbol(InterpreterContext *c,ObjectPtr printName);
int EXPORT PrintValue(InterpreterContext *c,ObjectPtr val,Stream *s);
void EXPORT InsufficientMemory(InterpreterContext *c);
void EXPORT StackOverflow(InterpreterContext *c);
void EXPORT ResetStack(InterpreterContext *c);
int EXPORT SaveWorkspace(InterpreterContext *c,Stream *s);
int EXPORT RestoreWorkspace(InterpreterContext *c,Stream *s);
void EXPORT CallErrorHandler(InterpreterContext *c,int errorCode,void *data);
void EXPORT ProtectPointer(InterpreterContext *c,ObjectPtr *pp);

#endif
