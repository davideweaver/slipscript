/* streams.h - definitions for Streams.c */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#ifndef __STREAMS_H__
#define __STREAMS_H__

#include <stdio.h>
#include <stdlib.h>

/* export keyword for library entry points */
#ifndef EXPORT
#ifdef _WINDLL
#define EXPORT __export
#else
#define EXPORT
#endif
#endif

/* end of file indicator for StreamGetC */
#define StreamEOF	(-1)

typedef struct {
  struct StreamIODispatch *d;
} Stream;

typedef struct StreamIODispatch {
  int (*close)(Stream *s);
  int (*getc)(Stream *s);
  int (*putch)(void *c,int ch,Stream *s);
} StreamIODispatch;

/* macros */
#define CloseStream(s)		((*(s)->d->close)(s))
#define StreamGetC(s)		((*(s)->d->getc)(s))
#define StreamPutC(c,ch,s)	((*(s)->d->putch)(c,ch,s))

/* null stream structure */
typedef struct {
  StreamIODispatch *d;
} NullStream;

#define NullInputStream		((Stream *)&nullInputStream)
#define NullOutputStream	((Stream *)&nullOutputStream)

/* string stream structure */
typedef struct {
  StreamIODispatch *d;
  unsigned char *ptr;
  long len;
} StringStream;

/* file stream structure */
typedef struct {
  StreamIODispatch *d;
  FILE *fp;
} FileStream;

/* globals */
extern NullStream nullInputStream;
extern NullStream nullOutputStream;

/* prototypes */
int EXPORT GetLine(Stream *s,char *buf,int len);
int EXPORT StreamPutS(void *c,char *str,Stream *s);
Stream * EXPORT CreateStringStream(unsigned char *buf,long len);
Stream * EXPORT OpenFilePointerStream(FILE *fp);
Stream * EXPORT OpenFileStream(char *fname,char *mode);
Stream * EXPORT CreateFileStream(FILE *fp);

#endif
