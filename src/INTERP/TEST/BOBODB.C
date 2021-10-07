/* bobodb.c - bob object database routines */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include "streams.h"
#include "bobodb.h"
#include "dfile.h"

/* prototypes for object store streams */
static int CloseObjectStream(Stream *s);
static int ObjectStreamGetC(Stream *s);
static int ObjectStreamPutC(void *c,int ch,Stream *s);

/* dispatch structure for object store streams */
StreamIODispatch objectIODispatch = {
  CloseObjectStream,
  ObjectStreamGetC,
  ObjectStreamPutC
};

Stream *CreateObjectStream(CMESSAGE *cmsg)
{
    ObjectStream *s = NULL;
    if ((s = (ObjectStream *)malloc(sizeof(ObjectStream))) != NULL) {
	s->d = &objectIODispatch;
	s->cmsg = cmsg;
    }
    	return (Stream *)s;
}

static int CloseObjectStream(Stream *s)
{
    ObjectStream *ss = (ObjectStream *)s;
    CloseConfMessage(ss->cmsg);
    free((void *)s);
    return 0;
}

static int ObjectStreamGetC(Stream *s)
{
    ObjectStream *ss = (ObjectStream *)s;
    return ConfGetC(ss->cmsg);
}

static int ObjectStreamPutC(void *c,int ch,Stream *s)
{
    ObjectStream *ss = (ObjectStream *)s;
    return ConfPutC(ch,ss->cmsg);
}
