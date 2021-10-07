/* function.c - built-in functions */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util.h>
#include <net_tools.h>
#include <date_time.h>

#include <slipscript.h>
#include "ver_info.h"

extern "C" {
	#include "compiler.h"
	#include "function.h"
	#include "execute.h"
	#include "objects.h"
	#include "linebuf.h"
	#include "boberr.h"
	#include "bobodb.h"
	#include "dfile.h"
	#include <cgi.h>
}

#include <odbc.h>	/* header for database functions */

/* prototypes */
static ObjectPtr BIF_New(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Clone(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_List(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Cons(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Car(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_SetCar(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Cdr(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_SetCdr(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Intern(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Class(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetLocalProperty(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Length(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Print(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Display(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_DecodeMethod(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GC(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_FOpen(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Close(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetC(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_PutC(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_CreateObjectStore(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_OpenObjectStore(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_CloseObjectStore(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_ObjectCount(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_OpenObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_CreateObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_UpdateObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_DeleteObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_ObjectNumber(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_ObjectVersion(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_CreateCursor(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_CloseCursor(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_SetCursorPosition(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetNextObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetPreviousObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_OpenCurrentObject(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Flatten(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Unflatten(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_NewLineBuffer(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_FillLineBuffer(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_NextToken(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_RestOfLine(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_WholeLine(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_IsMoreOnLine(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Load(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Quit(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Getenv(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetBrowserName(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetBrowserVersion(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetValue(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_ConnectDB(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_QueryDB(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetValueDB(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_ReportDB(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_InsertFile(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetRowsDB(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetColsDB(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Log(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_System(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Email(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_NewString(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_Version(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetDateTime(InterpreterContext *c,int n,ObjectPtr *p);
static ObjectPtr BIF_GetIniValue(InterpreterContext *c,int n,ObjectPtr *p);
static void EnterBuiltInStream(InterpreterContext *c,char *name,Stream *s);


/* function table */
static FunctionTableEntry functionTable[] = {
{	"new",					BIF_New					},
{	"clone",				BIF_Clone				},
{	"newList",				BIF_List				},
{	"Cons",					BIF_Cons				},
{	"Car",					BIF_Car					},
{	"SetCar",				BIF_SetCar				},
{	"Cdr",					BIF_Cdr					},
{	"SetCdr",				BIF_SetCdr				},
{	"Intern",				BIF_Intern				},
{	"Class",				BIF_Class				},
{	"GetLocalProperty",		BIF_GetLocalProperty	},
{	"sizeof",				BIF_Length				},
{	"error",				BIF_Print				},
{	"write",				BIF_Display				},
{	"DecodeMethod",			BIF_DecodeMethod		},
{	"GC",					BIF_GC					},
{	"fopen",				BIF_FOpen				},
{	"fclose",				BIF_Close				},
{	"getc",					BIF_GetC				},
{	"putc",					BIF_PutC				},
{	"CreateObjectStore",	BIF_CreateObjectStore	},
{	"OpenObjectStore",		BIF_OpenObjectStore		},
{	"CloseObjectStore",		BIF_CloseObjectStore	},
{	"ObjectCount",			BIF_ObjectCount			},
{	"OpenObject",			BIF_OpenObject			},
{	"CreateObject",			BIF_CreateObject		},
{	"UpdateObject",			BIF_UpdateObject		},
{	"DeleteObject",			BIF_DeleteObject		},
{	"ObjectNumber",			BIF_ObjectNumber		},
{	"ObjectVersion",		BIF_ObjectVersion		},
{	"CreateCursor",			BIF_CreateCursor		},
{	"CloseCursor",			BIF_CloseCursor			},
{	"SetCursorPosition",	BIF_SetCursorPosition	},
{	"GetNextObject",		BIF_GetNextObject		},
{	"GetPreviousObject",	BIF_GetPreviousObject	},
{	"OpenCurrentObject",	BIF_OpenCurrentObject	},
{	"store",				BIF_Flatten				},
{	"retrieve",				BIF_Unflatten			},
{	"newLineBuffer",		BIF_NewLineBuffer		},
{	"getl",					BIF_FillLineBuffer		},
{	"NextToken",			BIF_NextToken			},
{	"RestOfLine",			BIF_RestOfLine			},
{	"WholeLine",			BIF_WholeLine			},
{	"IsMoreOnLine",			BIF_IsMoreOnLine		},
{	"include",				BIF_Load				},
{	"exit",					BIF_Quit				},
{	"getenv",				BIF_Getenv				},
{	"getBrowserName",		BIF_GetBrowserName		},
{	"getBrowserVersion",	BIF_GetBrowserVersion	},
{	"getValue",				BIF_GetValue			},
{	"connectDB",			BIF_ConnectDB			},
{	"queryDB",				BIF_QueryDB				},
{	"getValueDB",			BIF_GetValueDB			},
{	"getRowsDB",			BIF_GetRowsDB			},
{	"getColsDB",			BIF_GetColsDB			},
{	"reportDB",				BIF_ReportDB			},
{	"insertFile",			BIF_InsertFile			},
{	"log",					BIF_Log					},
{	"system",				BIF_System				},
{	"email",				BIF_Email				},
{	"newString",			BIF_NewString			},
{	"version",				BIF_Version				},
{	"getDateTime",			BIF_GetDateTime			},
{	"getIniValue",			BIF_GetIniValue			},
{0,0}
}; 

/* EnterLibrarySymbols - enter the built-in functions and symbols */
void EnterLibrarySymbols(InterpreterContext *c)
{
    FunctionTableEntry *p;

    /* enter the built-in functions */
    for (p = functionTable; p->name != 0; ++p)
	EnterBuiltInFunction(c,p->name,p->handler);

    /* enter the built-in streams */
    EnterBuiltInStream(c,"stdin",c->standardInput);
    EnterBuiltInStream(c,"stdout",c->standardOutput);
    EnterBuiltInStream(c,"stderr",c->standardError);
}

/* FindLibraryFunctionHandler - find a library function handler */
CMethodHandlerPtr FindLibraryFunctionHandler(char *name)
{
    FunctionTableEntry *p;
    for (p = functionTable; p->name != 0; ++p)
	if (strcmp(p->name,name) == 0)
	    return p->handler;
    return NULL;
}

/* EnterBuiltInStream - add a built-in stream to the symbol table */
static void EnterBuiltInStream(InterpreterContext *c,char *name,Stream *s)
{
    cpush(c,InternCString(c,name));
    SetSymbolValue(top(c),NewForeignPtrObject(c,FileStreamTypeTag,(void *)s));
    drop(c,1);
}

/* clean up after the databases created */
extern "C" void cleanup( InterpreterContext *ic )
{
	ODBC_Access *o;
	SLIPSCRIPTSTRUCT *slipstruct = ( SLIPSCRIPTSTRUCT * )ic->pextra2;

	if( slipstruct->podbca != NULL ) {
		o = ( ODBC_Access * )slipstruct->podbca;
		o->closeSource();
		delete o;
	}
	if( slipstruct->podbcb != NULL ) {
		o = ( ODBC_Access * )slipstruct->podbcb;
		o->closeSource();
		delete o;
	}
	if( slipstruct->podbcc != NULL ) {
		o = ( ODBC_Access * )slipstruct->podbcc;
		o->closeSource();
		delete o;
	}

	/* free interpreter context */
	free( ic->oldSpace );
    free( ic->newSpace );
    free( ic->stack );
	free( ic->protectedPointers );
	free( ic );
}

/* getOpenSlot - gets an open database slot, return 0 if full		*/
int getOpenSlot( InterpreterContext *c ) 
{
	SLIPSCRIPTSTRUCT *ss = ( SLIPSCRIPTSTRUCT * )c->pextra2;

	/* find an empty database slot	*/
	/* return 0 if full				*/
	if( ss->podbca == NULL ) {
		ss->podbca = (void *)new ODBC_Access();
		return 1;
	}
	else if( ss->podbcb == NULL ) {
		ss->podbcb = (void *) new ODBC_Access();
		return 2;
	}
	else if( ss->podbcc == NULL ) {
		ss->podbcc = (void *) new ODBC_Access();
		return 3;
	}
	else return 0;
}

/* returns the odbc object for that slot */
ODBC_Access *getSlot( InterpreterContext *c, int slot )
{
	SLIPSCRIPTSTRUCT *ss = ( SLIPSCRIPTSTRUCT * )c->pextra2;

	if( slot == 1 ) return ( ODBC_Access * )ss->podbca;
	if( slot == 2 ) return ( ODBC_Access * )ss->podbcb;
	if( slot == 3 ) return ( ODBC_Access * )ss->podbcc;
	return NULL;
}

/* BIF_New - built-in function 'New' */
static ObjectPtr BIF_New(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(1);
    CheckType(1,ObjectP);
    return NewObject(c,GetArg(1));
}

/* BIF_Clone - built-in function 'Clone' */
static ObjectPtr BIF_Clone(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(1);
    CheckType(1,ObjectP);
    return CloneObject(c,GetArg(1));
}

/* BIF_List - built-in function 'List' */
static ObjectPtr BIF_List(InterpreterContext *c,int n,ObjectPtr *p)
{
    ObjectPtr val = c->nilObject;
    p -= n;
    while (--n >= 0)
	val = Cons(c,*p++,val);
    return val;
}

/* BIF_Cons - built-in function 'Cons' */
static ObjectPtr BIF_Cons(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(2);
    return Cons(c,GetArg(1),GetArg(2));
}

/* BIF_Car - built-in function 'Car' */
static ObjectPtr BIF_Car(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(1);
    CheckType(1,ConsP);
    return Car(GetArg(1));
}

/* BIF_SetCar - built-in function 'SetCar' */
static ObjectPtr BIF_SetCar(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(2);
    CheckType(1,ConsP);
    return SetCar(GetArg(1),GetArg(2));
}

/* BIF_Cdr - built-in function 'Cdr' */
static ObjectPtr BIF_Cdr(InterpreterContext *c,int n,ObjectPtr *p)
{
	int *i = new int();
	delete i;
    CheckArgCount(1);
    CheckType(1,ConsP);
    return Cdr(GetArg(1));
}

/* BIF_SetCdr - built-in function 'SetCdr' */
static ObjectPtr BIF_SetCdr(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(2);
    CheckType(1,ConsP);
    return SetCdr(GetArg(1),GetArg(2));
}

/* BIF_Intern - built-in function 'Intern' */
static ObjectPtr BIF_Intern(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(1);
    CheckType(1,StringP);
    return InternSymbol(c,GetArg(1));
}

/* BIF_Class - built-in function 'Class' */
static ObjectPtr BIF_Class(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(1);
    CheckType(1,ObjectP);
    return ObjectClass(GetArg(1));
}

/* BIF_GetLocalProperty - built-in function 'GetLocalProperty' */
static ObjectPtr BIF_GetLocalProperty(InterpreterContext *c,int n,ObjectPtr *p)
{
    ObjectPtr binding;
    CheckArgCount(2);
    CheckType(1,ObjectP);
    binding = GetLocalProperty(c,GetArg(1),GetArg(2));
    return binding ? binding : c->nilObject;
}

/* BIF_Length - built-in function 'Length' */
static ObjectPtr BIF_Length(InterpreterContext *c,int n,ObjectPtr *p)
{
    ObjectPtr arg;
    CheckArgCount(1);
    arg = GetArg(1);
    if (StringP(arg))
        return BoxNumber(StringSize(arg));
    else if (VectorP(arg))
        return BoxNumber(VectorSize(arg));
    else if (ConsP(arg)) {
        long len = 0;
        while (ConsP(arg)) {
            arg = Cdr(arg);
            ++len;
        }
        return BoxNumber(len);
    }
    else if (arg == c->nilObject)
        return BoxNumber(0);
    else
        TypeError(c,arg);
	return BoxNumber(0);
}

/* BIF_Print - built-in function 'Print' */
static ObjectPtr BIF_Print(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    CheckArgRange(1,2);
    if (n == 2) {
	CheckType(2,StreamP);
	s = (Stream *)ForeignPtr(GetArg(2));
    }
    else
	s = c->standardOutput;
    PrintValue(c,GetArg(1),s);
    return c->nilObject;
}

/* BIF_Display - built-in function 'Display' */
static ObjectPtr BIF_Display(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    CheckArgRange(1,2);
    if (n == 2) {
	CheckType(2,StreamP);
	s = (Stream *)ForeignPtr(GetArg(2));
    }
    else
	s = c->standardOutput;
    DisplayValue(c,GetArg(1),s);
    return c->nilObject;
}

/* BIF_GC - built-in function 'GC' */
static ObjectPtr BIF_GC(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(0);
    CollectGarbage(c);
    return c->nilObject;
}

/* BIF_DecodeMethod - built-in function 'DecodeMethod' */
static ObjectPtr BIF_DecodeMethod(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    CheckArgRange(1,2);
    CheckType(1,MethodP);
    if (n == 2) {
	CheckType(2,StreamP);
	s = (Stream *)ForeignPtr(GetArg(2));
    }
    else
	s = c->standardOutput;
    DecodeProcedure(c,GetArg(1),s);
    return c->trueObject;
}

/* BIF_FOpen - built-in function 'FOpen' */
static ObjectPtr BIF_FOpen(InterpreterContext *c,int n,ObjectPtr *p)
{
    char *fname,*mode;
    Stream *s;
    CheckArgCount(2);
    CheckType(1,StringP);
    CheckType(2,StringP);
    fname = (char *)StringDataAddress(GetArg(1));
    mode = (char *)StringDataAddress(GetArg(2));
    s = OpenFileStream(fname,mode);
    return s == NULL ? c->nilObject : NewForeignPtrObject(c,FileStreamTypeTag,(void *)s);
}

/* BIF_Close - built-in function 'Close' */
static ObjectPtr BIF_Close(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    CheckArgCount(1);
    CheckType(1,StreamP);
    s = (Stream *)ForeignPtr(GetArg(1));
    return CloseStream(s) == 0 ? c->trueObject : c->falseObject;
}

/* BIF_GetC - built-in function 'GetC' */
static ObjectPtr BIF_GetC(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    int ch;
    CheckArgCount(1);
    if (n == 1) {
	CheckType(1,StreamP);
	s = (Stream *)ForeignPtr(GetArg(1));
    }
    else
        s = c->standardInput;
    return (ch = StreamGetC(s)) < 0 ? c->nilObject : BoxNumber(ch);
}

/* BIF_PutC - built-in function 'PutC' */
static ObjectPtr BIF_PutC(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    int ch,rch;
    CheckArgRange(1,2);
    CheckType(1,NumberP);
    if (n == 2) {
	CheckType(2,StreamP);
	s = (Stream *)ForeignPtr(GetArg(2));
    }
    else
        s = c->standardOutput;
    ch = (int)UnboxNumber(GetArg(1));
    rch = StreamPutC(c,ch,s);
    return rch == ch ? GetArg(1) : c->nilObject;
}

static ObjectPtr BIF_CreateObjectStore(InterpreterContext *c,int n,ObjectPtr *p)
{
    char *name;
    CheckArgCount(1);
    CheckType(1,StringP);
    name = (char *)StringDataAddress(GetArg(1));
    return CreateConference(name) == 0 ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_OpenObjectStore(InterpreterContext *c,int n,ObjectPtr *p)
{
    char *name;
    CFILE *cfp;
    int mode;
    CheckArgRange(1,2);
    CheckType(1,StringP);
    if (n == 2)
        mode = GetArg(2) == c->nilObject ? CFM_READ : CFM_UPDATE;
    else
        mode = CFM_UPDATE;
    name = (char *)StringDataAddress(GetArg(1));
    cfp = OpenConfDatabase(name,mode);
    return cfp == NULL ? c->nilObject : NewForeignPtrObject(c,ObjectStoreTypeTag,(void *)cfp);
}

static ObjectPtr BIF_CloseObjectStore(InterpreterContext *c,int n,ObjectPtr *p)
{
    int result = 1;
    CFILE *cfp;
    CheckArgCount(1);
    CheckType(1,ObjectStoreP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    if (cfp)
	result = CloseConfDatabase(cfp);
    SetForeignPtr(GetArg(1),NULL);
    return result == 0 ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_ObjectCount(InterpreterContext *c,int n,ObjectPtr *p)
{
    CFILE *cfp;
    CheckArgCount(1);
    CheckType(1,ObjectStoreP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    return BoxNumber(GetMessageCount(cfp));
}

static ObjectPtr BIF_OpenObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    long msgno,version;
    CMESSAGE *cmsg;
    CFILE *cfp;
    Stream *s;
    CheckArgCount(3);
    CheckType(1,ObjectStoreP);
    CheckType(2,NumberP);
    CheckType(3,NumberP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    msgno = UnboxNumber(GetArg(2));
    version = UnboxNumber(GetArg(3));
    if ((cmsg = OpenConfMessage(cfp,msgno,version)) == NULL)
	return c->nilObject;
    else if ((s = CreateObjectStream(cmsg)) == NULL) {
	CloseConfMessage(cmsg);
	return c->nilObject;
    }
    return NewForeignPtrObject(c,ObjectStreamTypeTag,(void *)s);
}

static ObjectPtr BIF_CreateObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    CMESSAGE *cmsg;
    CFILE *cfp;
    Stream *s;
    CheckArgCount(1);
    CheckType(1,ObjectStoreP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    if ((cmsg = CreateConfMessage(cfp)) == NULL)
	return c->nilObject;
    else if ((s = CreateObjectStream(cmsg)) == NULL) {
	CloseConfMessage(cmsg);
	return c->nilObject;
    }
    return NewForeignPtrObject(c,ObjectStreamTypeTag,(void *)s);
}

static ObjectPtr BIF_UpdateObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    CMESSAGE *cmsg;
    CFILE *cfp;
    Stream *s;
    CheckArgCount(2);
    CheckType(1,ObjectStoreP);
    CheckType(2,NumberP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    if ((cmsg = UpdateConfMessage(cfp,UnboxNumber(GetArg(2)))) == NULL)
	return c->nilObject;
    else if ((s = CreateObjectStream(cmsg)) == NULL) {
	CloseConfMessage(cmsg);
	return c->nilObject;
    }
    return NewForeignPtrObject(c,ObjectStreamTypeTag,(void *)s);
}

static ObjectPtr BIF_DeleteObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    CFILE *cfp;
    CheckArgCount(2);
    CheckType(1,ObjectStoreP);
    CheckType(2,NumberP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    return DeleteConfMessage(cfp,UnboxNumber(GetArg(2))) ? c->falseObject : c->trueObject;
}

static ObjectPtr BIF_ObjectNumber(InterpreterContext *c,int n,ObjectPtr *p)
{
    ObjectStream *s;
    CheckArgCount(1);
    CheckType(1,ObjectStreamP);
    s = (ObjectStream *)ForeignPtr(GetArg(1));
    return BoxNumber(s->cmsg->cm_dheader.dh_msgno);
}

static ObjectPtr BIF_ObjectVersion(InterpreterContext *c,int n,ObjectPtr *p)
{
    ObjectStream *s;
    CheckArgCount(1);
    CheckType(1,ObjectStreamP);
    s = (ObjectStream *)ForeignPtr(GetArg(1));
    return BoxNumber(s->cmsg->cm_dheader.dh_version);
}

static ObjectPtr BIF_CreateCursor(InterpreterContext *c,int n,ObjectPtr *p)
{
    CCURSOR *cursor;
    CFILE *cfp;
    long msgno;
    CheckArgCount(2);
    CheckType(1,ObjectStoreP);
    CheckType(2,NumberP);
    cfp = (CFILE *)ForeignPtr(GetArg(1));
    msgno = UnboxNumber(GetArg(2));
    if ((cursor = CreateConfCursor(cfp,msgno)) == NULL)
	return c->nilObject;
    return NewForeignPtrObject(c,CursorTypeTag,(void *)cursor);
}

static ObjectPtr BIF_CloseCursor(InterpreterContext *c,int n,ObjectPtr *p)
{
    CCURSOR *cursor;
    CheckArgCount(1);
    CheckType(1,CursorP);
    cursor = (CCURSOR *)ForeignPtr(GetArg(1));
    return CloseConfCursor(cursor) == 0 ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_SetCursorPosition(InterpreterContext *c,int n,ObjectPtr *p)
{
    CCURSOR *cursor;
    long msgno;
    CheckArgCount(2);
    CheckType(1,CursorP);
    CheckType(2,NumberP);
    cursor = (CCURSOR *)ForeignPtr(GetArg(1));
    msgno = UnboxNumber(GetArg(2));
    return SetCursorPosition(cursor,msgno) == 0 ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_GetNextObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    CCURSOR *cursor;
    long msgno;
    CheckArgCount(1);
    CheckType(1,CursorP);
    cursor = (CCURSOR *)ForeignPtr(GetArg(1));
    msgno = GetNextMessage(cursor);
    return msgno == -1 ? c->nilObject : BoxNumber(msgno);
}

static ObjectPtr BIF_GetPreviousObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    CCURSOR *cursor;
    long msgno;
    CheckArgCount(1);
    CheckType(1,CursorP);
    cursor = (CCURSOR *)ForeignPtr(GetArg(1));
    msgno = GetPrevMessage(cursor);
    return msgno == -1 ? c->nilObject : BoxNumber(msgno);
}

static ObjectPtr BIF_OpenCurrentObject(InterpreterContext *c,int n,ObjectPtr *p)
{
    CCURSOR *cursor;
    CMESSAGE *cmsg;
    long version;
    Stream *s;
    CheckArgCount(2);
    CheckType(1,CursorP);
    CheckType(2,NumberP);
    cursor = (CCURSOR *)ForeignPtr(GetArg(1));
    version = UnboxNumber(GetArg(2));
    if ((cmsg = OpenCurrentMessage(cursor,version)) == NULL)
	return c->nilObject;
    else if ((s = CreateObjectStream(cmsg)) == NULL) {
	CloseConfMessage(cmsg);
	return c->nilObject;
    }
    return NewForeignPtrObject(c,ObjectStreamTypeTag,(void *)s);
}

static ObjectPtr BIF_Flatten(InterpreterContext *c,int n,ObjectPtr *p)
{
    Stream *s;
    CheckArgCount(2);
    CheckType(2,StreamP);
    s = (Stream *)ForeignPtr(GetArg(2));
    return FlattenObject(c,GetArg(1),c->nilObject,s) ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_Unflatten(InterpreterContext *c,int n,ObjectPtr *p)
{
    ObjectPtr obj;
    Stream *s;
    CheckArgCount(1);
    CheckType(1,StreamP);
    s = (Stream *)ForeignPtr(GetArg(1));
    obj = UnflattenObject(c,c->nilObject,s);
    return obj ? obj : c->nilObject;
}

static ObjectPtr BIF_NewLineBuffer(InterpreterContext *c,int n,ObjectPtr *p)
{
    LineBuffer *lbuf;
    CheckArgCount(0);
    if ((lbuf = NewLineBuffer()) == NULL)
	return c->nilObject;
    return NewForeignPtrObject(c,LineBufferTypeTag,(void *)lbuf);
}

static ObjectPtr BIF_FillLineBuffer(InterpreterContext *c,int n,ObjectPtr *p)
{
    LineBuffer *lbuf;
    Stream *s;
    CheckArgCount(2);
    CheckType(1,LineBufferP);
    CheckType(2,StreamP);
    lbuf = (LineBuffer *)ForeignPtr(GetArg(1));
    s = (Stream *)ForeignPtr(GetArg(2));
    return FillLineBuffer(lbuf,s) ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_NextToken(InterpreterContext *c,int n,ObjectPtr *p)
{
    LineBuffer *lbuf;
    char buf[LBMAX];
    CheckArgCount(1);
    CheckType(1,LineBufferP);
    lbuf = (LineBuffer *)ForeignPtr(GetArg(1));
    if (!NextToken(lbuf,buf))
	return c->nilObject;
    return NewCStringObject(c,buf);
}

static ObjectPtr BIF_RestOfLine(InterpreterContext *c,int n,ObjectPtr *p)
{
    LineBuffer *lbuf;
    char buf[LBMAX];
    CheckArgCount(1);
    CheckType(1,LineBufferP);
    lbuf = (LineBuffer *)ForeignPtr(GetArg(1));
    if (!RestOfLine(lbuf,buf))
	return c->nilObject;
    return NewCStringObject(c,buf);
}

static ObjectPtr BIF_WholeLine(InterpreterContext *c,int n,ObjectPtr *p)
{
    LineBuffer *lbuf;
    char buf[LBMAX];
    CheckArgCount(1);
    CheckType(1,LineBufferP);
    lbuf = (LineBuffer *)ForeignPtr(GetArg(1));
    if (!WholeLine(lbuf,buf))
	return c->nilObject;
    return NewCStringObject(c,buf);
}

static ObjectPtr BIF_IsMoreOnLine(InterpreterContext *c,int n,ObjectPtr *p)
{
    LineBuffer *lbuf;
    CheckArgCount(1);
    CheckType(1,LineBufferP);
    lbuf = (LineBuffer *)ForeignPtr(GetArg(1));
    return IsMoreOnLine(lbuf) ? c->trueObject : c->falseObject;
}

static ObjectPtr BIF_Load(InterpreterContext *c,int n,ObjectPtr *p)
{
    char *name;
    CheckArgCount(1);
    CheckType(1,StringP);
    if (c->cc) {
        name = (char *)StringDataAddress(GetArg(1));
        return LoadFile((struct CompilerContext *)c->cc,
				name,c->standardOutput) == 0 ? c->trueObject
		        : c->falseObject;
    }
    else
	return c->falseObject;
}

/* BIF_Quit - built-in function 'Quit' */
static ObjectPtr BIF_Quit(InterpreterContext *c,int n,ObjectPtr *p)
{
    CheckArgCount(0);
    CallErrorHandler(c,ecExit,0);
	return 0; /* never reached */
}


static ObjectPtr BIF_Getenv(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *name;
	char *value;
    CheckArgCount(1);
	CheckType(1,StringP);
	name = (char *)StringDataAddress(GetArg(1));
	value = getenv(name);
	return value == NULL ? c->nilObject : NewCStringObject(c,value);
}


static ObjectPtr BIF_GetBrowserName(InterpreterContext *c,int n,ObjectPtr *p)
{
#ifdef _CPP
	char *value;
	FormIn f;
    CheckArgCount(0);
	value = f.getClientName();
	return value == NULL ? nilObject : NewCStringObject(value);
#endif
	return c->nilObject;
}

static ObjectPtr BIF_GetBrowserVersion(InterpreterContext *c,int n,ObjectPtr *p)
{
#ifdef _CPP
	char *value;
	FormIn f;
    CheckArgCount(0);
	value = f.getClientVersion();
	return value == NULL ? nilObject : NewCStringObject(value);
#endif
	return c->nilObject;
}

static ObjectPtr BIF_GetValue(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *name;
	char *value;
	SLIPSCRIPTSTRUCT *slipstruct;
	llist *entries;

	slipstruct = ( SLIPSCRIPTSTRUCT * )c->pextra2;
	entries = ( llist * )slipstruct->entries;

    CheckArgCount(1);
	CheckType(1,StringP);
	name = ( char * )StringDataAddress( GetArg( 1 ) );
	if( entries == NULL ) return c->nilObject;
	value = cgi_val( *entries, name );
	return value == NULL ? c->nilObject : NewCStringObject( c, value );
}


static ObjectPtr BIF_GetValueDB(InterpreterContext *c,int n,ObjectPtr *p)
{
	char value[LBMAX];
	char *header;
	int which_slot = 0;
	ODBC_Access *o;
	int row;

    CheckArgCount(3);
	CheckType(1,NumberP);
	CheckType(2,StringP);
	CheckType(3,NumberP);

	which_slot = UnboxNumber( GetArg( 1 ) );
	if( ( o = getSlot( c, which_slot ) ) == NULL ) return c->nilObject;

	header = (char *)StringDataAddress(GetArg(2));
	row = UnboxNumber(GetArg(3));

	if( o->getData( header, row, value ) ) 
		return NewCStringObject( c, value );
	return c->nilObject;
}

static ObjectPtr BIF_ConnectDB(InterpreterContext *c,int n,ObjectPtr *p)
{

	int which_slot = 0;
	char *dsn;
	char *userid;
	char *passwd;
	ODBC_Access *o;
    CheckArgRange(1,3);

	/* find an empty database slot	*/
	if( !( which_slot = getOpenSlot( c ) ) )
		return BoxNumber( 0 );
	if( ( o = getSlot( c, which_slot ) ) == NULL ) return BoxNumber( 0 );

	if(n >= 1) {
		CheckType(1,StringP);
		dsn = (char *)StringDataAddress(GetArg(1));
		o->setDataSource(dsn);
	}
	if(n >= 2) {
		CheckType(2,StringP);
		userid = (char *)StringDataAddress(GetArg(2));
		o->setUserID(userid);
	}
	if(n == 3) {
		CheckType(3,StringP);
		passwd = (char *)StringDataAddress(GetArg(3));
		o->setPassword(passwd);
	}
	if(o->connectSource()) return BoxNumber(which_slot);
	else return BoxNumber( 0 );

}

static ObjectPtr BIF_QueryDB(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *sql;
	int which_slot = 0;
	ODBC_Access *o;

    CheckArgCount( 2 );
	CheckType( 1, NumberP );
	CheckType( 2, StringP );
	which_slot = UnboxNumber( GetArg( 1 ) );
	if( ( o = getSlot( c, which_slot ) ) == NULL ) return c->nilObject;
	
	sql = ( char * )StringDataAddress( GetArg( 2 ) );
	o->querySource( sql );
	
	return c->nilObject;
}

static ObjectPtr BIF_ReportDB(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *format;
	int which_slot = 0;
	ODBC_Access *o;
	int rows = 0;

	InterpreterContext *ic = ( InterpreterContext * )c;
	daves_buffer *output = ( daves_buffer * )ic->pextra1;


    CheckArgRange(1,3);
	CheckType(1,NumberP);
	CheckType(2,StringP);

	which_slot = UnboxNumber( GetArg( 1 ) );
	if( ( o = getSlot( c, which_slot ) ) == NULL ) return c->nilObject;
	format = ( char * )StringDataAddress( GetArg( 2 ) );
	if(n == 3) {
		CheckType( 3, NumberP );
		rows = UnboxNumber( GetArg( 3 ) );
	}

	o->reportData( format, output, rows );	
	return c->nilObject;
}

static ObjectPtr BIF_GetRowsDB(InterpreterContext *c,int n,ObjectPtr *p)
{
	int which_slot = 0;
	ODBC_Access *o;

	CheckArgCount(1);
	CheckType(1,NumberP);
	which_slot = UnboxNumber(GetArg(1));
	if( ( o = getSlot( c, which_slot ) ) == NULL ) return c->nilObject;
	return BoxNumber(o->getRowCount());
}

static ObjectPtr BIF_GetColsDB(InterpreterContext *c,int n,ObjectPtr *p)
{
	int which_slot = 0;
	ODBC_Access *o;

	CheckArgCount(1);
	CheckType(1,NumberP);
	which_slot = UnboxNumber(GetArg(1));
	if( ( o = getSlot( c, which_slot ) ) == NULL ) return c->nilObject;
	return BoxNumber(o->getColCount());
}

/* fixed for DELAWARE 9/13/96 */
static ObjectPtr BIF_Email(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *to;
	char *from;
	char *cc;
	char *subject;
	char *body;
	char *host;
	int ret_val;

	CheckArgRange(5,6);
	smtp e;
	if(n >= 5) {
		CheckType(1,StringP);
		CheckType(2,StringP);
		CheckType(3,StringP);
		CheckType(4,StringP);
		CheckType(5,StringP);
		to = (char *)StringDataAddress(GetArg(1));
		from = (char *)StringDataAddress(GetArg(2));
		subject = (char *)StringDataAddress(GetArg(3));
		body = (char *)StringDataAddress(GetArg(4));
		host = (char *)StringDataAddress(GetArg(5));
		e.setreceiver(to);
		e.setsender(from);
		e.setsubject(subject);
		e.setmessage(body);
		e.setmailmachine(host);
	}
	if(n >= 6) {
		CheckType(6,StringP);
		cc = (char *)StringDataAddress(GetArg(6));
		e.setcc(cc); 
	}

	/*		0 if all went well										*/
	/*		1 if mail parameters not initialized					*/
	/*		2 if smtp server was not there							*/
	/*		3 if smtp server not responding							*/
	/*		4 if error getting UserName								*/
	/*		>10000 if a socket error occured						*/
	ret_val = e.sendmail();

	return BoxNumber( ret_val );
}

/* fixed for DELAWARE 9/13/96 */
static ObjectPtr BIF_InsertFile(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *file;
	char content[LBMAX];
	FILE *fp;

	// get the output buffer
	daves_buffer *output = ( daves_buffer * )c->pextra1;

	CheckArgCount(1);
	CheckType(1,StringP);
	file = (char *)StringDataAddress(GetArg(1));
	fp = fopen(file,"r");
	if(fp != NULL) {
		while(fgets(content,sizeof(content),fp) != NULL)
			output->append( content );
		fclose(fp);
	}
	return c->nilObject;
}

static ObjectPtr BIF_Log(InterpreterContext *c,int n,ObjectPtr *p)
{
#ifdef _CPP
	char msg[LBMAX];
	char dir[LBMAX];
	FormIn f;
	CheckArgRange(1,2);
	CheckType(1,StringP);
	GetCString(GetArg(1),msg,sizeof(msg));
	if(n == 2) {
		CheckType(2,StringP);
		GetCString(GetArg(2),dir,sizeof(dir));
	} 
	else {
		/* now check ini file version (g) */
		if(ini == NULL) 
			ini = new ini_file("slipscript.ini");
		int len = ini->getValue("logdir",dir);
		/* check for trailing backslash, must not have it */
		if(dir[len-1] == '\\')
			dir[len-1] = '\0';
	}
	f.log(msg,dir);
	return nilObject;
#endif
	return c->nilObject;
}

static ObjectPtr BIF_System(InterpreterContext *c,int n,ObjectPtr *p)
{
	char *name;
	int value;

#ifdef _INI
	/* check if system call is permitted version (g) */
	if(ini == NULL) 
		ini = new ini_file("slipscript.ini");
	if(!ini->isTrue("usesystem")) return nilObject;
#endif

    CheckArgCount(1);
	CheckType(1,StringP);
	name = (char *)StringDataAddress(GetArg(1));
	value = system(name);
	return BoxNumber(value);
}

static ObjectPtr BIF_NewString(InterpreterContext *c,int n,ObjectPtr *p)
{
	/* version (f) */
	int count;
	char *newstring;
	ObjectPtr str;
    CheckArgCount(1);
	CheckType(1,NumberP);
	count = UnboxNumber(GetArg(1));
	if(count > LBMAX) return c->nilObject;
	newstring = (char *)malloc(count+1);
	memset(newstring,' ',count+1);
	newstring[count] = '\0';
	str = NewCStringObject(c,newstring);
	free(newstring);
	return str;
}


static ObjectPtr BIF_Version(InterpreterContext *c,int n,ObjectPtr *p)
{
	/* version (e) */
	Stream *s;
	char ver[] = SS_VERSION;
    CheckArgCount(0);
	s = c->standardOutput;
	DisplayValue(c,NewCStringObject(c,ver),s);
	return c->nilObject;
}

/* fixed for DELAWARE 9/13/96 */
static ObjectPtr BIF_GetDateTime(InterpreterContext *c,int n,ObjectPtr *p)
{
	/* version (f) */
	char datetime[LBMAX];
	char *format;
    CheckArgCount(1);
	CheckType(1,StringP);
	format = (char *)StringDataAddress(GetArg(1));
	date_time cal;
	cal.get_formatted(format,datetime);
	return NewCStringObject(c,datetime);
}

static ObjectPtr BIF_GetIniValue(InterpreterContext *c,int n,ObjectPtr *p)
{
#ifdef _CPP
	/* version (h) */
	char name[LBMAX];
	char value[LBMAX];
    CheckArgCount(1);
	CheckType(1,StringP);
	GetCString(GetArg(1),name,sizeof(name));
	/* now check ini file version (g) */
	if(ini == NULL) 
		ini = new ini_file("slipscript.ini");
	int len = ini->getValue(name,value);
	return len ? NewCStringObject(value) : nilObject;
#endif
	return c->nilObject;
}





