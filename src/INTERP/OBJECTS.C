/* objects.c - object manipulation routines */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objects.h"
#include "boberr.h"

/* round a size up to a multiple of the size of a long */
#define RoundToLong(x)	(((x) + 3) & ~3)

/* prototypes */
static long ObjectSize(ObjectPtr obj);
static void ScanObject(InterpreterContext *c,ObjectPtr obj);
static ObjectPtr ReadString(InterpreterContext *c,Stream *s);
static int WriteString(InterpreterContext *c,ObjectPtr obj,long tag,Stream *s);
static int PrintStringValue(InterpreterContext *c,ObjectPtr val,Stream *s);
static int PrintListValue(InterpreterContext *c,ObjectPtr val,Stream *s);
static int DisplayStringValue(InterpreterContext *c,ObjectPtr val,Stream *s);
static void DefaultObjectPrint(ObjectPtr obj,char *buf);
static void InitObjectMemory(InterpreterContext *c,long size,long stackSize);
static void AllocateMemorySpaces(InterpreterContext *c,long size,long stackSize);
static void FreeMemorySpaces(InterpreterContext *c);
static void InternUsefulSymbols(InterpreterContext *c);
static MemorySpace *NewMemorySpace(long size);
static ObjectPtr AllocateObjectMemory(InterpreterContext *c,long size);
static ObjectPtr CopyPropertyList(InterpreterContext *c,ObjectPtr plist);
static int WriteLong(InterpreterContext *c,long n,Stream *s);
static int ReadLong(long *pn,Stream *s);
static int WritePointer(InterpreterContext *c,ObjectPtr p,Stream *s);
static int ReadPointer(InterpreterContext *c,ObjectPtr *pp,Stream *s);
static CMethodHandlerPtr DefaultFindFunctionHandler(char *name);

InterpreterContext *NewInterpreterContext(long size,long stackSize)
{
    InterpreterContext *c = malloc(sizeof(InterpreterContext));
    if (c) {
	c->cc = NULL;
	c->nilObject = NULL;
	c->trueObject = NULL;
	c->falseObject = NULL;
	c->objectObject = NULL;
	c->classNameObject = NULL;
	c->unboundObject = NULL;
	c->symbols = NULL;
	c->interpreterTarget = NULL;
	c->protectedPointers = NULL;
	c->findFunctionHandler = DefaultFindFunctionHandler;
	c->errorHandler = NULL;
	c->standardInput = NullInputStream;
	c->standardOutput = NullOutputStream;
	c->standardError = NullOutputStream;
	InitObjectMemory(c,size,stackSize);
    }
    return c;
}

void SetExtraInterpreterContextData( InterpreterContext *c, void *extra1, void *extra2 )
{
	c->pextra1 = extra1;
	c->pextra2 = extra2;
}

void ObjectPrint(ObjectPtr obj,char *buf)
{
    ObjectDispatch(obj)->print(obj,buf);
}

static long ObjectSize(ObjectPtr obj)
{
    return ObjectDispatch(obj)->size(obj);
}

ObjectPtr CopyObject(InterpreterContext *c,ObjectPtr obj)
{
    return ObjectDispatch(obj)->copy(c,obj);
}

static void ScanObject(InterpreterContext *c,ObjectPtr obj)
{
    QuickDispatch(obj)->scan(c,obj);
}


/* WARNING:  this must be in the same order as the xxxTypeTag values!!! */
static TypeDispatch *tagToDispatch[] = {
  &BrokenHeartDispatch,
  &NumberDispatch,
  &ObjectDispatch,
  &ConsDispatch,
  &SymbolDispatch,
  &StringDispatch,
  &VectorDispatch,
  &MethodDispatch,
  &CMethodDispatch,
  &FileStreamDispatch,
  &StringStreamDispatch,
  &ObjectStreamDispatch,
  &ObjectStoreDispatch,
  &CursorDispatch,
  &LineBufferDispatch,
  &CompiledCodeDispatch,
  &EnvironmentDispatch,
  &StackEnvironmentDispatch,
  &MovedEnvironmentDispatch
};

/*** NUMBER ***/

static void NumPrint(ObjectPtr obj,char *buf);
static long NumSize(ObjectPtr obj);
static ObjectPtr NumCopy(InterpreterContext *c,ObjectPtr obj);
static void NumScan(InterpreterContext *c,ObjectPtr obj);
static int NumWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int NumRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int NumFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr NumUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch NumberDispatch = {
    NumberTypeTag,
    "Number",
    NumPrint,
    NumSize,
    NumCopy,
    NumScan,
    NumWrite,
    NumRead,
    NumFlatten,
    NumUnflatten
};

static void NumPrint(ObjectPtr obj,char *buf)
{
    sprintf(buf,"%ld",UnboxNumber(obj));
}

static long NumSize(ObjectPtr obj)
{
    return 0;
}

static ObjectPtr NumCopy(InterpreterContext *c,ObjectPtr obj)
{
    return obj;
}

static void NumScan(InterpreterContext *c,ObjectPtr obj)
{
    /* should never get here! */
}

static int NumWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    /* should never get here! */
    return FALSE;
}

static int NumRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    /* should never get here! */
    return FALSE;
}

static int NumFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return WriteLong(c,NumberTypeTag,s)
    &&     WriteLong(c,UnboxNumber(obj),s);
}

static ObjectPtr NumUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    long n;
    if (!ReadLong(&n,s))
	return NULL;
    return BoxNumber(n);
}


/*** OBJECT ***/

static long ObjSize(ObjectPtr obj);
static ObjectPtr ObjCopy(InterpreterContext *c,ObjectPtr obj);
static void ObjScan(InterpreterContext *c,ObjectPtr obj);
static int ObjWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int ObjRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int ObjFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr ObjUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch ObjectDispatch = {
    ObjectTypeTag,
    "Object",
    DefaultObjectPrint,
    ObjSize,
    ObjCopy,
    ObjScan,
    ObjWrite,
    ObjRead,
    ObjFlatten,
    ObjUnflatten
};

ObjectPtr NewObject(InterpreterContext *c,ObjectPtr class)
{
    ObjectPtr new;
    cpush(c,class);
    new = AllocateObjectMemory(c,sizeof(Object));
    SetObjectDispatch(new,&ObjectDispatch);
    SetObjectClass(new,pop(c));
    SetObjectProperties(new,c->nilObject);
    return new;
}

ObjectPtr CloneObject(InterpreterContext *c,ObjectPtr obj)
{
    check(c,2);
    push(c,obj);
    push(c,NewObject(c,XObjectClass(obj)));
    SetObjectDispatch(top(c),ObjectDispatch(c->sp[1]));
    SetObjectProperties(top(c),CopyPropertyList(c,ObjectProperties(c->sp[1])));
    obj = pop(c);
    drop(c,1);
    return obj;
}

ObjectPtr GetObjectClass(ObjectPtr obj)
{
    ObjectPtr class = XObjectClass(obj);
    if (SymbolP(class))
	class = SymbolValue(class);
    return ObjectP(class) ? class : NULL;
}

static long ObjSize(ObjectPtr obj)
{
    return sizeof(Object);
}

static ObjectPtr ObjCopy(InterpreterContext *c,ObjectPtr obj)
{
    ObjectPtr newObj = (ObjectPtr)c->newSpace->free;
    long size = ObjectSize(obj);
    memcpy(newObj,obj,(size_t)size);
    c->newSpace->free += size;
    SetObjectDispatch(obj,&BrokenHeartDispatch);
    SetForwardingAddress(obj,newObj);
    return newObj;
}

static void ObjScan(InterpreterContext *c,ObjectPtr obj)
{
    SetObjectClass(obj,CopyObject(c,XObjectClass(obj)));
    SetObjectProperties(obj,CopyObject(c,ObjectProperties(obj)));
}

static int ObjWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    return WriteLong(c,ObjectTypeTag,s)
    &&     WritePointer(c,XObjectClass(obj),s)
    &&     WritePointer(c,ObjectProperties(obj),s);
}

static int ObjRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    ObjectPtr class,properties;
    if (!ReadPointer(c,&class,s)
    ||  !ReadPointer(c,&properties,s))
	return FALSE;
    SetObjectClass(obj,class);
    SetObjectProperties(obj,properties);
    return TRUE;
}

static int ObjFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    ObjectPtr class = XObjectClass(obj);
    ObjectPtr p = ObjectProperties(obj);
    long size = ListLength(p);

    /* class is an object, find its class name */
    if (ObjectP(class)) {
	ObjectPtr className = GetProperty(c,class,c->classNameObject);
	if (!SymbolP(className)
	||  !WriteString(c,SymbolPrintName(className),ObjectTypeTag,s))
	    return FALSE;
    }

    /* class is a symbol, write it as the class name */
    else if (SymbolP(class)) {
	if (!WriteString(c,SymbolPrintName(class),ObjectTypeTag,s))
	    return FALSE;
    }
    
    /* class is NULL, write an empty string */
    else if (class == c->nilObject) {
	if (!WriteLong(c,ObjectTypeTag,s) || !WriteLong(c,0,s))
	    return FALSE;
    }
    
    /* class is something else, error! */
    else
	return FALSE;
	
    /* write the property/value pairs */
    if (!WriteLong(c,size,s))
	return FALSE;
    for (; --size >= 0; p = Cdr(p))
	if (!FlattenObject(c,Car(Car(p)),olist,s)
	||  !FlattenObject(c,Cdr(Car(p)),olist,s))
	    return FALSE;
	    
    /* return successfully */
    return TRUE;
}

static ObjectPtr ObjUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    ObjectPtr class,obj;
    long size;
    
    /* get the class name */
    obj = ReadString(c,s);
    if (!obj) return NULL;
    obj = InternSymbol(c,obj);
    
    /* get the class itself (or the symbol if the class is undefined) */
    class = SymbolValue(obj);
    if (!ObjectP(class))
	class = obj;
    
    /* create the object */
    check(c,3);
    push(c,NewObject(c,class));

    /* read the property/value pairs */
    push(c,c->nilObject);
    if (!ReadLong(&size,s))
	return NULL;
    while (--size >= 0) {
	if ((obj = UnflattenObject(c,olist,s)) == NULL)
	    { drop(c,2); return NULL; }
	push(c,obj);
	if ((obj = UnflattenObject(c,olist,s)) == NULL)
	    { drop(c,3); return NULL; }
	settop(c,Cons(c,top(c),obj));
	obj = Cons(c,pop(c),c->nilObject);
	if (top(c) == c->nilObject)
	    SetObjectProperties(c->sp[1],obj);
	else
	    SetCdr(top(c),obj);
	settop(c,obj);
    }
    drop(c,1);
    
    /* return successfully */
    return pop(c);
}


/*** CONS ***/

static long ConsSize(ObjectPtr obj);
static void ConsScan(InterpreterContext *c,ObjectPtr obj);
static int ConsWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int ConsRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int ConsFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr ConsUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch ConsDispatch = {
    ConsTypeTag,
    "Cons",
    DefaultObjectPrint,
    ConsSize,
    ObjCopy,
    ConsScan,
    ConsWrite,
    ConsRead,
    ConsFlatten,
    ConsUnflatten
};

ObjectPtr Cons(InterpreterContext *c,ObjectPtr car,ObjectPtr cdr)
{
    ObjectPtr new;
    check(c,2);
    push(c,cdr);
    push(c,car);
    new = AllocateObjectMemory(c,sizeof(ConsObject));
    SetObjectDispatch(new,&ConsDispatch);
    SetCar(new,pop(c));
    SetCdr(new,pop(c));
    return new;
}

static long ConsSize(ObjectPtr obj)
{
    return sizeof(ConsObject);
}

static void ConsScan(InterpreterContext *c,ObjectPtr obj)
{
    SetCar(obj,CopyObject(c,Car(obj)));
    SetCdr(obj,CopyObject(c,Cdr(obj)));
}

static int ConsWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    return WriteLong(c,TypeTag(obj),s)
    &&     WritePointer(c,Car(obj),s)
    &&     WritePointer(c,Cdr(obj),s);
}

static int ConsRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    ObjectPtr car,cdr;
    if (!ReadPointer(c,&car,s)
    ||  !ReadPointer(c,&cdr,s))
	return FALSE;
    SetCar(obj,car);
    SetCdr(obj,cdr);
    return TRUE;
}

static int ConsFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return WriteLong(c,TypeTag(obj),s)
    &&     FlattenObject(c,Car(obj),olist,s)
    &&     FlattenObject(c,Cdr(obj),olist,s);
}

static ObjectPtr ConsUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    ObjectPtr obj;
    if ((obj = UnflattenObject(c,olist,s)) == NULL)
	return NULL;
    cpush(c,obj);
    if ((obj = UnflattenObject(c,olist,s)) == NULL)
    	{ drop(c,1); return NULL; }
    return Cons(c,pop(c),obj);
}

static ObjectPtr ConsLikeObjectUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    ObjectPtr obj;
    if ((obj = UnflattenObject(c,olist,s)) == NULL)
	return NULL;
    cpush(c,obj);
    if ((obj = UnflattenObject(c,olist,s)) == NULL)
    	{ drop(c,1); return NULL; }
    obj = Cons(c,pop(c),obj);
    SetObjectDispatch(obj,d);
    return obj;
}


/*** SYMBOL OBJECT ***/

static void SymPrint(ObjectPtr obj,char *buf);
static long SymSize(ObjectPtr obj);
static void SymScan(InterpreterContext *c,ObjectPtr obj);
static int SymWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int SymRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int SymFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr SymUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch SymbolDispatch = {
    SymbolTypeTag,
    "Symbol",
    SymPrint,
    SymSize,
    ObjCopy,
    SymScan,
    SymWrite,
    SymRead,
    SymFlatten,
    SymUnflatten
};

ObjectPtr NewSymbolObject(InterpreterContext *c,ObjectPtr printName)
{
    ObjectPtr new;
    cpush(c,printName);
    new = AllocateObjectMemory(c,sizeof(SymbolObject));
    SetObjectDispatch(new,&SymbolDispatch);
    SetSymbolPrintName(new,pop(c));
    SetSymbolValue(new,c->unboundObject);
    return new;
}

static void SymPrint(ObjectPtr obj,char *buf)
{
    ObjectPtr name = SymbolPrintName(obj);
    long size = StringSize(name);
    char *p = (char *)StringDataAddress(name);
    while (--size >= 0)
	*buf++ = *p++;
    *buf = '\0';
}

static long SymSize(ObjectPtr obj)
{
    return sizeof(SymbolObject);
}

static void SymScan(InterpreterContext *c,ObjectPtr obj)
{
    SetSymbolPrintName(obj,CopyObject(c,SymbolPrintName(obj)));
    SetSymbolValue(obj,CopyObject(c,SymbolValue(obj)));
}

static int SymWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    return WriteLong(c,SymbolTypeTag,s)
    &&     WritePointer(c,SymbolPrintName(obj),s)
    &&     WritePointer(c,SymbolValue(obj),s);
}

static int SymRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    ObjectPtr printName,value;
    if (!ReadPointer(c,&printName,s)
    ||  !ReadPointer(c,&value,s))
	return FALSE;
    SetSymbolPrintName(obj,printName);
    SetSymbolValue(obj,value);
    return TRUE;
}

static int SymFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return WriteString(c,SymbolPrintName(obj),SymbolTypeTag,s);
}

static ObjectPtr SymUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    ObjectPtr obj = ReadString(c,s);
    return obj ? InternSymbol(c,obj) : NULL;
}


/*** STRING OBJECT ***/

static void StrPrint(ObjectPtr obj,char *buf);
static long StrSize(ObjectPtr obj);
static void StrScan(InterpreterContext *c,ObjectPtr obj);
static int StrWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int StrRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int StrFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr StrUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch StringDispatch = {
    StringTypeTag,
    "String",
    StrPrint,
    StrSize,
    ObjCopy,
    StrScan,
    StrWrite,
    StrRead,
    StrFlatten,
    StrUnflatten
};

ObjectPtr NewStringObject(InterpreterContext *c,unsigned char *data,long size)
{
    long allocSize = sizeof(StringObject) + RoundToLong(size + 1); /* space for zero terminator */
    ObjectPtr new = AllocateObjectMemory(c,allocSize);
    unsigned char *p = StringDataAddress(new);
    SetObjectDispatch(new,&StringDispatch);
    SetStringSize(new,size);
    p[size] = '\0'; /* in case we need to use it as a C string */
    
    if (data)
	memcpy(StringDataAddress(new),data,(size_t)size);
    else {
	for (; --size >= 0; )
	    *p++ = '\0';
    }
    return new;
}

ObjectPtr NewCStringObject(InterpreterContext *c,char *str)
{
    return NewStringObject(c,(unsigned char *)str,strlen(str));
}

static void StrPrint(ObjectPtr obj,char *buf)
{
    long size = StringSize(obj);
    char *p = (char *)StringDataAddress(obj);
    *buf++ = '"';
    while (--size >= 0)
	*buf++ = *p++;
    *buf++ = '"';
    *buf = '\0';
}

static long StrSize(ObjectPtr obj)
{
    return sizeof(StringObject) + RoundToLong(StringSize(obj) + 1);
}

static void StrScan(InterpreterContext *c,ObjectPtr obj)
{
}

static int StrWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    return WriteString(c,obj,StringTypeTag,s);
}

static int StrRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    unsigned char *p = StringDataAddress(obj);
    long size;
    int ch;
    if (!ReadLong(&size,s))
	return FALSE;
    SetStringSize(obj,size);
    while (--size >= 0) {
	if ((ch = StreamGetC(s)) == StreamEOF)
	    return FALSE;
	*p++ = ch;
    }
    return TRUE;
}

static int StrFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return WriteString(c,obj,StringTypeTag,s);
}

static ObjectPtr StrUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    return ReadString(c,s);
}

static ObjectPtr ReadString(InterpreterContext *c,Stream *s)
{
    unsigned char *p;
    ObjectPtr obj;
    long size;
    int ch;
    if (!ReadLong(&size,s))
	return NULL;
    obj = NewStringObject(c,NULL,size);
    p = StringDataAddress(obj);
    while (--size >= 0) {
	if ((ch = StreamGetC(s)) == StreamEOF)
	    return c->nilObject;
	*p++ = ch;
    }
    return obj;
}

static int WriteString(InterpreterContext *c,ObjectPtr obj,long tag,Stream *s)
{
    unsigned char *p = StringDataAddress(obj);
    long size = StringSize(obj);
    if (!WriteLong(c,tag,s) || !WriteLong(c,size,s))
	return FALSE;
    for (; --size >= 0; ++p)
	if (StreamPutC(c,*p,s) == StreamEOF)
	    return FALSE;
    return TRUE;
}


/*** VECTOR OBJECT ***/

static long VecSize(ObjectPtr obj);
static void VecScan(InterpreterContext *c,ObjectPtr obj);
static int VecWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int VecRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int VecFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr VecUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch VectorDispatch = {
    VectorTypeTag,
    "Vector",
    DefaultObjectPrint,
    VecSize,
    ObjCopy,
    VecScan,
    VecWrite,
    VecRead,
    VecFlatten,
    VecUnflatten
};

ObjectPtr NewVectorObject(InterpreterContext *c,long size)
{
    long allocSize = sizeof(VectorObject) + size * sizeof(ObjectPtr);
    ObjectPtr new = AllocateObjectMemory(c,allocSize);
    ObjectPtr *p;
    SetObjectDispatch(new,&VectorDispatch);
    SetVectorSize(new,size);
    for (p = VectorDataAddress(new); --size >= 0; )
	*p++ = c->nilObject;
    return new;
}

static long VecSize(ObjectPtr obj)
{
    return sizeof(VectorObject) + VectorSize(obj) * sizeof(ObjectPtr);
}

static void VecScan(InterpreterContext *c,ObjectPtr obj)
{
    long i;
    for (i = 0; i < VectorSize(obj); ++i)
    	SetVectorElement(obj,i,CopyObject(c,VectorElement(obj,i)));
}

static int VecWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    long i,size = VectorSize(obj);
    if (!WriteLong(c,VectorTypeTag,s) || !WriteLong(c,size,s))
	return FALSE;
    for (i = 0; i < size; ++i)
	if (!WritePointer(c,VectorElement(obj,i),s))
	    return FALSE;
    return TRUE;
}

static int VecRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    ObjectPtr value;
    long i,size;
    if (!ReadLong(&size,s))
	return FALSE;
    SetVectorSize(obj,size);
    for (i = 0; i < size; ++i) {
	if (!ReadPointer(c,&value,s))
	    return FALSE;
	SetVectorElement(obj,i,value);
    }
    return TRUE;
}

static int VecFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    ObjectPtr *p = VectorDataAddress(obj);
    long size = VectorSize(obj);
    if (!WriteLong(c,VectorTypeTag,s) || !WriteLong(c,size,s))
	return FALSE;
    for (; --size >= 0; ++p)
	if (!FlattenObject(c,*p,olist,s))
	    return FALSE;
    return TRUE;
}

static ObjectPtr VecUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    ObjectPtr obj;
    long size,i;
    if (!ReadLong(&size,s))
	return NULL;
    cpush(c,NewVectorObject(c,size));
    for (i = 0; i < size; ++i) {
	if ((obj = UnflattenObject(c,olist,s)) == NULL)
	    { drop(c,1); return NULL; }
	SetVectorElement(top(c),i,obj);
    }
    return pop(c);
}


/*** COMPILED CODE OBJECT ***/

static void CompiledCodePrint(ObjectPtr obj,char *buf);

TypeDispatch CompiledCodeDispatch = {
    CompiledCodeTypeTag,
    "CompiledCode",
    CompiledCodePrint,
    VecSize,
    ObjCopy,
    VecScan,
    VecWrite,
    VecRead,
    VecFlatten,
    VecUnflatten
};

static void CompiledCodePrint(ObjectPtr obj,char *buf)
{
    ObjectPtr name = CompiledCodeName(obj);
    if (StringP(name)) {
	char *tag = TypeName(obj);
	unsigned char *data = StringDataAddress(name);
	long size = StringSize(name);
	if (size > 245) size = 245; /* 256 character buffer */
	*buf++ = '<';
	while (*tag) *buf++ = *tag++;
	*buf++ = ' ';
	while (--size >= 0) *buf++ = *data++;
	*buf++ = '>';
	*buf = '\0';
    }
    else
	DefaultObjectPrint(obj,buf);
}

ObjectPtr NewCompiledCodeObject(InterpreterContext *c,long size,ObjectPtr bytecodes)
{
    ObjectPtr code;
    cpush(c,bytecodes);
    code = NewVectorObject(c,size);
    SetObjectDispatch(code,&CompiledCodeDispatch);
    SetCompiledCodeBytecodes(code,pop(c));
    return code;
}


/*** ENVIRONMENT OBJECT ***/

TypeDispatch EnvironmentDispatch = {
    EnvironmentTypeTag,
    "Environment",
    DefaultObjectPrint,
    VecSize,
    ObjCopy,
    VecScan,
    VecWrite,
    VecRead,
    VecFlatten,
    VecUnflatten
};

ObjectPtr NewEnvironmentObject(InterpreterContext *c,long size)
{
    ObjectPtr env = NewVectorObject(c,size);
    SetObjectDispatch(env,&EnvironmentDispatch);
    return env;
}


/*** STACK ENVIRONMENT OBJECT ***/

static ObjectPtr StackEnvironmentCopy(InterpreterContext *c,ObjectPtr obj);

TypeDispatch StackEnvironmentDispatch = {
    StackEnvironmentTypeTag,
    "StackEnvironment",
    DefaultObjectPrint,
    VecSize,
    StackEnvironmentCopy,
    VecScan,
    VecWrite,
    VecRead,
    VecFlatten,
    VecUnflatten
};

static ObjectPtr StackEnvironmentCopy(InterpreterContext *c,ObjectPtr obj)
{
    return obj;
}

/*** MOVED ENVIRONMENT OBJECT ***/

TypeDispatch MovedEnvironmentDispatch = {
    MovedEnvironmentTypeTag,
    "MovedEnvironment",
    DefaultObjectPrint,
    VecSize,
    StackEnvironmentCopy,
    VecScan,
    VecWrite,
    VecRead,
    VecFlatten,
    VecUnflatten
};

/*** METHOD OBJECT ***/

static void MethodPrint(ObjectPtr obj,char *buf);

TypeDispatch MethodDispatch = {
    MethodTypeTag,
    "Method",
    MethodPrint,
    ConsSize,
    ObjCopy,
    ConsScan,
    ConsWrite,
    ConsRead,
    ConsFlatten,
    ConsUnflatten
};

ObjectPtr NewMethodObject(InterpreterContext *c,ObjectPtr code,ObjectPtr env)
{
    ObjectPtr method = Cons(c,code,env);
    SetObjectDispatch(method,&MethodDispatch);
    return method;
}

static void MethodPrint(ObjectPtr obj,char *buf)
{
    ObjectPtr name = CompiledCodeName(MethodCode(obj));
    if (StringP(name)) {
	char *tag = TypeName(obj);
	unsigned char *data = StringDataAddress(name);
	long size = StringSize(name);
	if (size > 245) size = 245; /* 256 character buffer */
	*buf++ = '<';
	while (*tag) *buf++ = *tag++;
	*buf++ = ' ';
	while (--size >= 0) *buf++ = *data++;
	*buf++ = '>';
	*buf = '\0';
    }
    else
	DefaultObjectPrint(obj,buf);
}


/*** C METHOD OBJECT ***/

static void CMethodPrint(ObjectPtr obj,char *buf);
static long CMethodSize(ObjectPtr obj);
static void CMethodScan(InterpreterContext *c,ObjectPtr obj);
static int CMethodWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int CMethodRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int CMethodFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr CMethodUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch CMethodDispatch = {
    CMethodTypeTag,
    "CMethod",
    CMethodPrint,
    CMethodSize,
    ObjCopy,
    CMethodScan,
    CMethodWrite,
    CMethodRead,
    CMethodFlatten,
    CMethodUnflatten
};

ObjectPtr NewCMethodObject(InterpreterContext *c,char *name,CMethodHandlerPtr handler)
{
    ObjectPtr new;
    new = AllocateObjectMemory(c,sizeof(CMethodObject));
    SetObjectDispatch(new,&CMethodDispatch);
    SetCMethodName(new,name);
    SetCMethodHandler(new,handler);
    return new;
}

static void CMethodPrint(ObjectPtr obj,char *buf)
{
    char *tag = TypeName(obj);
    char *name = CMethodName(obj);
    *buf++ = '<';
    while (*tag) *buf++ = *tag++;
    *buf++ = ' ';
    while (*name) *buf++ = *name++;
    *buf++ = '>';
    *buf = '\0';
}

static long CMethodSize(ObjectPtr obj)
{
    return sizeof(CMethodObject);
}

static void CMethodScan(InterpreterContext *c,ObjectPtr obj)
{
}

static int CMethodWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    char *p = CMethodName(obj);
    if (!WriteLong(c,CMethodTypeTag,s))
	return FALSE;
    for (; *p != '\0'; ++p)
	if (StreamPutC(c,*p,s) == StreamEOF)
	    return FALSE;
    return StreamPutC(c,'\0',s) != StreamEOF;
}

static int CMethodRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    char name[256],*p = name;
    CMethodHandlerPtr handler;
    int ch;
    while ((ch = StreamGetC(s)) != '\0') {
	if (ch == StreamEOF)
	    return FALSE;
	*p++ = ch;
    }
    *p = '\0';
    if ((handler = (*c->findFunctionHandler)(name)) == NULL)
        return FALSE;
    SetCMethodName(obj,name);
    SetCMethodHandler(obj,handler);
    return TRUE;
}

static int CMethodFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return CMethodWrite(c,obj,s);
}

static ObjectPtr CMethodUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    return NULL;
}


/*** FOREIGN POINTER OBJECT ***/

static long ForeignPtrSize(ObjectPtr obj);
static void ForeignPtrScan(InterpreterContext *c,ObjectPtr obj);
static int ForeignPtrWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int ForeignPtrRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int ForeignPtrFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr ForeignPtrUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

#define ForeignPtrDispatchTail	DefaultObjectPrint,ForeignPtrSize,ObjCopy,ForeignPtrScan,ForeignPtrWrite,ForeignPtrRead,ForeignPtrFlatten,ForeignPtrUnflatten

TypeDispatch FileStreamDispatch = {
    FileStreamTypeTag,
    "FileStream",
    ForeignPtrDispatchTail
};

TypeDispatch StringStreamDispatch = {
    StringStreamTypeTag,
    "StringStream",
    ForeignPtrDispatchTail
};

TypeDispatch ObjectStreamDispatch = {
    ObjectStreamTypeTag,
    "ObjectStream",
    ForeignPtrDispatchTail
};

TypeDispatch ObjectStoreDispatch = {
    ObjectStoreTypeTag,
    "ObjectStore",
    ForeignPtrDispatchTail
};

TypeDispatch CursorDispatch = {
    CursorTypeTag,
    "Cursor",
    ForeignPtrDispatchTail
};

TypeDispatch LineBufferDispatch = {
    LineBufferTypeTag,
    "LineBuffer",
    ForeignPtrDispatchTail
};

static long ForeignPtrSize(ObjectPtr obj)
{
    return sizeof(ForeignPtrObject);
}

static void ForeignPtrScan(InterpreterContext *c,ObjectPtr obj)
{
}

static int ForeignPtrWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    if (!WriteLong(c,TypeTag(obj),s))
	return FALSE;
    return TRUE;
}

static int ForeignPtrRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    SetForeignPtr(obj,NULL);
    return TRUE;
}

static int ForeignPtrFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return FALSE;
}

static ObjectPtr ForeignPtrUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    return NULL;
}


/*** BROKEN HEART ***/

static long BrokenHeartSize(ObjectPtr obj);
static ObjectPtr BrokenHeartCopy(InterpreterContext *c,ObjectPtr obj);
static void BrokenHeartScan(InterpreterContext *c,ObjectPtr obj);
static int BrokenHeartWrite(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int BrokenHeartRead(InterpreterContext *c,ObjectPtr obj,Stream *s);
static int BrokenHeartFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s);
static ObjectPtr BrokenHeartUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s);

TypeDispatch BrokenHeartDispatch = {
    BrokenHeartTypeTag,
    "BrokenHeart",
    DefaultObjectPrint,
    BrokenHeartSize,
    BrokenHeartCopy,
    BrokenHeartScan,
    BrokenHeartWrite,
    BrokenHeartRead,
    BrokenHeartFlatten,
    BrokenHeartUnflatten
};

static long BrokenHeartSize(ObjectPtr obj)
{
    return sizeof(BrokenHeart);
}

static ObjectPtr BrokenHeartCopy(InterpreterContext *c,ObjectPtr obj)
{
    return ForwardingAddress(obj);
}

static void BrokenHeartScan(InterpreterContext *c,ObjectPtr obj)
{
    /* should never get here! */
}

static int BrokenHeartWrite(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    /* should never get here! */
    return FALSE;
}

static int BrokenHeartRead(InterpreterContext *c,ObjectPtr obj,Stream *s)
{
    /* should never get here! */
    return FALSE;
}

static int BrokenHeartFlatten(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    /* should never get here! */
    return FALSE;
}

static ObjectPtr BrokenHeartUnflatten(InterpreterContext *c,TypeDispatch *d,ObjectPtr olist,Stream *s)
{
    /* should never get here! */
    return 0;
}

ObjectPtr NewForeignPtrObject(InterpreterContext *c,long type,void *ptr)
{
    ObjectPtr new;
    new = AllocateObjectMemory(c,sizeof(ForeignPtrObject));
    SetObjectDispatch(new,tagToDispatch[(int)type]);
    SetForeignPtr(new,ptr);
    return new;
}

int FlattenObject(InterpreterContext *c,ObjectPtr obj,ObjectPtr olist,Stream *s)
{
    return ObjectDispatch(obj)->flatten(c,obj,olist,s);
}

ObjectPtr UnflattenObject(InterpreterContext *c,ObjectPtr olist,Stream *s)
{
    TypeDispatch *d;
    long typeTag;
    ReadLong(&typeTag,s);
    d = tagToDispatch[(int)typeTag];
    return d->unflatten(c,d,olist,s);
}


/* miscellaneous functions */

ObjectPtr GetProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag)
{
    ObjectPtr p;
    if ((p = GetLocalProperty(c,obj,tag)) != NULL)
	return Cdr(p);
    else if ((p = GetInheritedProperty(c,obj,tag)) != NULL)
	return Cdr(p);
    return c->nilObject;
}

ObjectPtr GetLocalProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag)
{
    ObjectPtr p;
    for (p = ObjectProperties(obj); p != c->nilObject; p = Cdr(p))
	if (Eql(Car(Car(p)),tag))
	    return Car(p);
    return NULL;
}

ObjectPtr GetInheritedProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag)
{
    ObjectPtr class,p;
    for (class = ObjectClass(obj); class && class != c->nilObject; class = ObjectClass(class))
	if ((p = GetLocalProperty(c,class,tag)) != NULL)
	    return p;
    return NULL;
}

void SetProperty(InterpreterContext *c,ObjectPtr obj,ObjectPtr tag,ObjectPtr value)
{
    ObjectPtr last = c->nilObject,p;
    for (p = ObjectProperties(obj); p != c->nilObject; last = p, p = Cdr(p))
	if (Eql(Car(Car(p)),tag)) {
	    SetCdr(Car(p),value);
	    return;
	}
    check(c,2);
    push(c,obj);
    push(c,last);
    p = Cons(c,Cons(c,tag,value),c->nilObject);
    if (top(c) != c->nilObject) SetCdr(top(c),p);
    else SetObjectProperties(c->sp[1],p);
    drop(c,2);
}

/* Eql - compare two objects for equality */
int Eql(ObjectPtr obj1,ObjectPtr obj2)
{
    if (NumberP(obj1))
	return NumberP(obj2) && UnboxNumber(obj1) == UnboxNumber(obj2);
    else if (StringP(obj1))
	return StringP(obj2) && CompareStrings(obj1,obj2) == 0;
    else
    	return obj1 == obj2;
}

/* CompareStrings - compare two strings */
int CompareStrings(ObjectPtr str1,ObjectPtr str2)
{
    unsigned char *p1 = StringDataAddress(str1);
    long len1 = StringSize(str1);
    unsigned char *p2 = StringDataAddress(str2);
    long len2 = StringSize(str2);
    while (len1 > 0 && len2 > 0 && *p1++ == *p2++)
	--len1, --len2;
    if (len1 == 0) return len2 == 0 ? 0 : -1;
    if (len2 == 0) return 1;
    return *--p1 - *--p2 < 0 ? -1 : *p1 == *p2 ? 0 : 1;
}

long ListLength(ObjectPtr list)
{
    long len = 0;
    while (ConsP(list)) {
	list = Cdr(list);
	++len;
    }
    return len;
}

ObjectPtr CopyPropertyList(InterpreterContext *c,ObjectPtr plist)
{
    ObjectPtr new;
    check(c,3);
    push(c,c->nilObject);
    push(c,c->nilObject);
    push(c,plist);
    for (; top(c) != c->nilObject; settop(c,Cdr(top(c)))) {
	new = Cons(c,Cons(c,Car(Car(top(c))),Cdr(Car(top(c)))),c->nilObject);
	if (c->sp[1] == c->nilObject)
	    c->sp[1] = c->sp[2] = new;
	else {
	    SetCdr(c->sp[1],new);
	    c->sp[1] = new;
	}
    }
    drop(c,2);
    return pop(c);
}

ObjectPtr InternCString(InterpreterContext *c,char *str)
{
    return InternSymbol(c,NewStringObject(c,(unsigned char *)str,strlen(str)));
}

ObjectPtr InternSymbol(InterpreterContext *c,ObjectPtr printName)
{
    unsigned char *p1,*p2;
    ObjectPtr sym,p;
    long cnt;
    for (p = c->symbols; p != c->nilObject; p = Cdr(p)) {
	sym = Car(p);
	p1 = StringDataAddress(printName);
	p2 = StringDataAddress(SymbolPrintName(sym));
	cnt = StringSize(printName);
	if (cnt == StringSize(SymbolPrintName(sym))) {
	    while (cnt > 0) {
		if (*p1++ != *p2++)
		    break;
		--cnt;
	    }
	    if (cnt == 0)
		return sym;
	}
    }
    sym = NewSymbolObject(c,printName);
    c->symbols = Cons(c,sym,c->symbols);
    return Car(c->symbols);
}

/* PrintValue - print a value */
int PrintValue(InterpreterContext *c,ObjectPtr val,Stream *s)
{
    if (StringP(val))
	return PrintStringValue(c,val,s);
    else if (ConsP(val))
	return PrintListValue(c,val,s);
    else {
	char buf[256],*p;
	ObjectPrint(val,buf);
	for (p = buf; *p != '\0'; )
	    if (StreamPutC(c,*p++,s) == StreamEOF)
		return StreamEOF;
	return 0;
    }
}

/* PrintStringValue - print a string value */
static int PrintStringValue(InterpreterContext *c,ObjectPtr val,Stream *s)
{
    unsigned char *p = StringDataAddress(val);
    long size = StringSize(val);
    if (StreamPutC(c,'"',s) == StreamEOF)
	return StreamEOF;
    while (--size >= 0)
	if (StreamPutC(c,*p++,s) == StreamEOF)
	    return StreamEOF;
    return StreamPutC(c,'"',s) == StreamEOF ? StreamEOF : 0;
}

/* PrintListValue - print a list value */
static int PrintListValue(InterpreterContext *c,ObjectPtr val,Stream *s)
{
    check(c,1);
    if (StreamPutC(c,'\\',s) == StreamEOF
    ||  StreamPutC(c,'(',s) == StreamEOF)
	return StreamEOF;
    while (ConsP(val)) {
	push(c,Cdr(val));
	if (PrintValue(c,Car(val),s) == StreamEOF
	||  (ConsP(top(c)) && (StreamPutC(c,',',s) == StreamEOF
			||  StreamPutC(c,' ',s) == StreamEOF))) {
	    drop(c,1);
	    return StreamEOF;
	}
	val = pop(c);
    }
    if (val != c->nilObject) {
	if (StreamPutC(c,' ',s) == StreamEOF
	||  StreamPutC(c,'.',s) == StreamEOF
	||  StreamPutC(c,' ',s) == StreamEOF
	||  PrintValue(c,val,s) == StreamEOF)
	    return StreamEOF;
    }
    if (StreamPutC(c,')',s) == StreamEOF)
	return StreamEOF;
    return 0;
}

/* DisplayValue - display a value */
int DisplayValue(InterpreterContext *c,ObjectPtr val,Stream *s)
{
    return StringP(val) ? DisplayStringValue(c,val,s) : PrintValue(c,val,s);
}

/* DisplayStringValue - display a string value */
static int DisplayStringValue(InterpreterContext *c,ObjectPtr val,Stream *s)
{
    unsigned char *p = StringDataAddress(val);
    long size = StringSize(val);
    while (--size >= 0)
	if (StreamPutC(c,*p++,(Stream *)s) == StreamEOF)
	    return StreamEOF;
    return 0;
}

static void DefaultObjectPrint(ObjectPtr obj,char *buf)
{
    char *tag = TypeName(obj);
    *buf++ = '<';
    while (*tag) *buf++ = *tag++;
    *buf++ = ' ';
    sprintf(buf,"%ld",(long)obj);
    strcat(buf,">");
}

static void AllocateMemorySpaces(InterpreterContext *c,long size,long stackSize)
{
    /* make the memory spaces */
    CheckNIL(c,c->oldSpace = NewMemorySpace(size));
    CheckNIL(c,c->newSpace = NewMemorySpace(size));

    /* make and initialize the stack */
    CheckNIL(c,c->stack = (ObjectPtr *)malloc((size_t)(stackSize * sizeof(ObjectPtr))));
    c->stackTop = c->stack + stackSize;
    ResetStack(c);
}

void FreeMemorySpaces(InterpreterContext *c)
{
    free(c->oldSpace);
    free(c->newSpace);
    free(c->stack);
}
       
static void InitObjectMemory(InterpreterContext *c,long size,long stackSize)
{
    /* first allocate the memory */
    AllocateMemorySpaces(c,size,stackSize);
    
    /* make the NULL symbol */
    c->nilObject = NewSymbolObject(c,NewCStringObject(c,"NULL"));
    SetSymbolValue(c->nilObject,c->nilObject);

    /* fixup the initial environment */
    c->env = c->nilObject;
    
    /* initialize the symbol table */
    c->symbols = c->nilObject;
        
    /* make the *unbound* object */
    c->unboundObject = NewSymbolObject(c,NewCStringObject(c,"*unbound*"));
    SetSymbolValue(c->unboundObject,c->unboundObject);

    /* make the true and false symbols */
    c->trueObject = InternCString(c,"t");
    SetSymbolValue(c->trueObject,c->trueObject);
    c->falseObject = c->nilObject;
        
    /* make other useful symbols */
    InternUsefulSymbols(c);

    /* make the root of the object heirarchy */
    c->objectObject = NewObject(c,c->nilObject);
    cpush(c,InternCString(c,"Object"));
    SetProperty(c,c->objectObject,c->classNameObject,top(c));
    SetSymbolValue(pop(c),c->objectObject);
}

static void InternUsefulSymbols(InterpreterContext *c)
{
    c->classNameObject = InternCString(c,"className");
}

static MemorySpace *NewMemorySpace(long size)
{
    MemorySpace *space;
    if ((space = (MemorySpace *)malloc((size_t)(sizeof(MemorySpace) + size))) != NULL) {
	space->base = (unsigned char *)space + sizeof(MemorySpace);
	space->free = space->base;
	space->top = space->base + size;
    }
    return space;
}

static ObjectPtr AllocateObjectMemory(InterpreterContext *c,long size)
{
    ObjectPtr p;
    if (c->newSpace->free + size > c->newSpace->top) {
	CollectGarbage(c);
	if (c->newSpace->free + size > c->newSpace->top)
	    InsufficientMemory(c);
    }
    p = (ObjectPtr)c->newSpace->free;
    c->newSpace->free += size;
    return p;
}

void CollectGarbage(InterpreterContext *c)
{
    ProtectedPtrBlock *ppb;
    ObjectPtr obj,*fp,*p;
    unsigned char *scan;
    MemorySpace *ms;
    
    /* reverse the memory spaces */
    ms = c->oldSpace;
    c->oldSpace = c->newSpace;
    c->newSpace = ms;

    /* reset the new space pointers */
    c->newSpace->free = scan = c->newSpace->base;
    
    /* copy the root objects */
    c->nilObject = CopyObject(c,c->nilObject);
    c->trueObject = CopyObject(c,c->trueObject);
    c->falseObject = CopyObject(c,c->falseObject);
    c->unboundObject = CopyObject(c,c->unboundObject);
    c->objectObject = CopyObject(c,c->objectObject);
    c->classNameObject = CopyObject(c,c->classNameObject);
    c->symbols = CopyObject(c,c->symbols);

    /* copy protected pointers */
    for (ppb = c->protectedPointers; ppb != NULL; ppb = ppb->next) {
        ObjectPtr **pp = ppb->pointers;
        int count = ppb->count;
        for (; --count >= 0; ++pp)
            **pp = CopyObject(c,**pp);
    }
    
    /* copy the stack */
    for (fp = c->fp, p = c->sp; p < c->stackTop; )
	if (p >= fp)
	    p = (*((CDispatch *)*fp)->copy)(c,&fp);
	else {
	    *p = CopyObject(c,*p);
	    ++p;
	}
	
    /* copy the current code object */
    if (c->code != NULL)
	c->code = CopyObject(c,c->code);
    
    /* copy the current environment */
    if (c->env != NULL)
        c->env = CopyObject(c,c->env);
        
    /* scan and copy until all accessible objects have been copied */
    while (scan < c->newSpace->free) {
	obj = (ObjectPtr)scan;
	scan += ObjectSize(obj);
	ScanObject(c,obj);
    }
	
    /* fixup cbase and pc */
    if (c->code != NULL) {
	long pcoff = c->pc - c->cbase;
	c->cbase = StringDataAddress(CompiledCodeBytecodes(c->code));
	c->pc = c->cbase + pcoff;
    }
}

int SaveWorkspace(InterpreterContext *c,Stream *s)
{
    unsigned char *next;
    ObjectPtr obj;
    
    /* first compact memory */
    CollectGarbage(c);
    
    /* write the file header */
    WriteLong(c,(long)(c->newSpace->top - c->newSpace->base),s);
    WriteLong(c,(long)(c->stackTop - c->stack),s);
    WriteLong(c,(long)(c->newSpace->free - c->newSpace->base),s);
    
    /* write the root objects */
    WritePointer(c,c->nilObject,s);
    WritePointer(c,c->symbols,s);
    WritePointer(c,c->trueObject,s);
    WritePointer(c,c->falseObject,s);
    WritePointer(c,c->objectObject,s);
    WritePointer(c,c->unboundObject,s);
    
    /* write the heap */
    for (next = c->newSpace->base; next < c->newSpace->free; next += ObjectSize(obj)) {
	obj = (ObjectPtr)next;
	if (!ObjectDispatch(obj)->write(c,obj,s))
	    return FALSE;
    }
    return TRUE;
}

int RestoreWorkspace(InterpreterContext *c,Stream *s)
{
    long size,stackSize,tmp;
    unsigned char *free;
    TypeDispatch *d;
    ObjectPtr obj;
    
    /* free the old memory spaces */
    FreeMemorySpaces(c);
    
    /* read the file header */
    ReadLong(&size,s);
    ReadLong(&stackSize,s);
    ReadLong(&tmp,s);
    
    /* create the new memory spaces */
    AllocateMemorySpaces(c,size,stackSize);
    
    /* compute first free address after the load */
    free = c->newSpace->base + tmp;
    
    /* read the root objects */
    ReadPointer(c,&c->nilObject,s);
    ReadPointer(c,&c->symbols,s);
    ReadPointer(c,&c->trueObject,s);
    ReadPointer(c,&c->falseObject,s);
    ReadPointer(c,&c->objectObject,s);
    ReadPointer(c,&c->unboundObject,s);
    
    /* read the heap */
    while (c->newSpace->free < free) {
	ReadLong(&tmp,s);
	d = tagToDispatch[(int)tmp];
	obj = (ObjectPtr)c->newSpace->free;
	SetObjectDispatch(obj,d);
	if (!d->read(c,obj,s))
	    return FALSE;
	c->newSpace->free += ObjectSize(obj);
    }

    /* lookup useful symbols */
    InternUsefulSymbols(c);
    return TRUE;
}

void ProtectPointer(InterpreterContext *c,ObjectPtr *pp)
{
    ProtectedPtrBlock *ppb = c->protectedPointers;
    if (ppb == NULL || ppb->count >= PPBSize) {
		CheckNIL(c,ppb = (ProtectedPtrBlock *)malloc(sizeof(ProtectedPtrBlock)));
		ppb->next = c->protectedPointers;
		c->protectedPointers = ppb;
		ppb->count = 0;
    }
    ppb->pointers[ppb->count++] = pp;
    *pp = c->nilObject;
}

static int WriteLong(InterpreterContext *c,long n,Stream *s)
{
    return StreamPutC(c,(int)(n >> 24),s) != StreamEOF
    &&     StreamPutC(c,(int)(n >> 16),s) != StreamEOF
    &&     StreamPutC(c,(int)(n >>  8),s) != StreamEOF
    &&     StreamPutC(c,(int)(n)      ,s) != StreamEOF;
}

static int ReadLong(long *pn,Stream *s)
{
    int c;
    if ((c = StreamGetC(s)) == StreamEOF)
	return FALSE;
    *pn = (long)c << 24;
    if ((c = StreamGetC(s)) == StreamEOF)
	return FALSE;
    *pn |= (long)c << 16;
    if ((c = StreamGetC(s)) == StreamEOF)
	return FALSE;
    *pn |= (long)c << 8;
    if ((c = StreamGetC(s)) == StreamEOF)
	return FALSE;
    *pn |= (long)c;
    return TRUE;
}

static int WritePointer(InterpreterContext *c,ObjectPtr p,Stream *s)
{
    return NumberP(p) ? WriteLong(c,(long)p,s)
    		      : WriteLong(c,(long)((unsigned char *)p - c->newSpace->base),s);
}

static int ReadPointer(InterpreterContext *c,ObjectPtr *pp,Stream *s)
{
    long n;
    if (!ReadLong(&n,s))
	return FALSE;
    *pp = NumberP(n) ? (ObjectPtr)n : (ObjectPtr)(c->newSpace->base + n);
    return TRUE;
}

void CallErrorHandler(InterpreterContext *c,int errorCode,void *data)
{
    (*c->errorHandler)(c,errorCode,data);
}

void InsufficientMemory(InterpreterContext *c)
{
    CallErrorHandler(c,ecInsufficientMemory,0);
}

void StackOverflow(InterpreterContext *c)
{
    CallErrorHandler(c,ecStackOverflow,0);
}

static CMethodHandlerPtr DefaultFindFunctionHandler(char *name)
{
    return NULL;
}

void ResetStack(InterpreterContext *c)
{
    c->sp = c->fp = c->stackTop;
    c->env = c->nilObject;
    c->code = NULL;
}




