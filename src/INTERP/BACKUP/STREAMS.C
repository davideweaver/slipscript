/* streams.c - i/o routines */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#include "objects.h"
#include "streams.h"

int GetLine(Stream *s,char *buf,int len)
{
    char *p = buf;
    int ch;
    while ((ch = StreamGetC(s)) != '\n' && ch != StreamEOF)
	if (len > 1) {
	    *p++ = ch;
	    --len;
	}
    *p = '\0';
    return ch == StreamEOF && p == buf ? -1 : 0;
}

int StreamPutS(InterpreterContext *c,char *str,Stream *s)
{
    while (*str != '\0')
	if (StreamPutC(c,*str++,s) == -1)
	    return -1;
    return 0;
}

/* prototypes for null streams */
static int CloseNullStream(Stream *s);
static int NullStreamGetC(Stream *s);
static int NullStreamPutC(InterpreterContext *c,int ch,Stream *s);

/* dispatch structure for null streams */
StreamIODispatch nullIODispatch = {
  CloseNullStream,
  NullStreamGetC,
  NullStreamPutC
};

NullStream nullInputStream = { &nullIODispatch };
NullStream nullOutputStream = { &nullIODispatch };

static int CloseNullStream(Stream *s)
{
    return 0;
}

static int NullStreamGetC(Stream *s)
{
    return StreamEOF;
}

static int NullStreamPutC(InterpreterContext *c,int ch,Stream *s)
{
    return StreamEOF;
}

/* prototypes for string streams */
static int CloseStringStream(Stream *s);
static int StringStreamGetC(Stream *s);
static int StringStreamPutC(InterpreterContext *c,int ch,Stream *s);

/* dispatch structure for string streams */
StreamIODispatch stringIODispatch = {
  CloseStringStream,
  StringStreamGetC,
  StringStreamPutC
};

Stream *CreateStringStream(unsigned char *buf,long len)
{
    StringStream *s = NULL;
    if ((s = (StringStream *)malloc(sizeof(StringStream))) != NULL) {
	s->d = &stringIODispatch;
	s->ptr = buf;
	s->len = len;
    }
    return (Stream *)s;
}

static int CloseStringStream(Stream *s)
{
    free((void *)s);
    return 0;
}

static int StringStreamGetC(Stream *s)
{
    StringStream *ss = (StringStream *)s;
    if (ss->len <= 0)
	return StreamEOF;
    --ss->len;
    return *ss->ptr++;
}

static int StringStreamPutC(InterpreterContext *c,int ch,Stream *s)
{
    return StreamEOF;
}

/* prototypes for file streams */
static int CloseFileStream(Stream *s);
static int FileStreamGetC(Stream *s);
static int FileStreamPutC(InterpreterContext *c,int ch,Stream *s);

/* dispatch structure for file streams */
StreamIODispatch fileDispatch = {
  CloseFileStream,
  FileStreamGetC,
  FileStreamPutC
};

Stream *CreateFileStream(FILE *fp)
{
    FileStream *s = NULL;
    if ((s = (FileStream *)malloc(sizeof(FileStream))) != NULL) {
	s->d = &fileDispatch;
	s->fp = fp;
    }
    return (Stream *)s;
}

Stream *OpenFilePointerStream(FILE *fp)
{
    FileStream *s = NULL;
	if ((s = (FileStream *)malloc(sizeof(FileStream))) == NULL)
	    fclose(fp);
	else {
	    s->d = &fileDispatch;
	    s->fp = fp;
	}
    return (Stream *)s;
}


Stream *OpenFileStream(char *fname,char *mode)
{
    FileStream *s = NULL;
    FILE *fp;
    if ((fp = fopen(fname,mode)) != NULL) {
	if ((s = (FileStream *)malloc(sizeof(FileStream))) == NULL)
	    fclose(fp);
	else {
	    s->d = &fileDispatch;
	    s->fp = fp;
	}
    }
    return (Stream *)s;
}

static int CloseFileStream(Stream *s)
{
    FileStream *fs;
    fs = (FileStream *)s;
    fclose(fs->fp);
    free((void *)s);
    return 0;
}

static int FileStreamGetC(Stream *s)
{
    FileStream *fs = (FileStream *)s;
    return getc(fs->fp);
}

static int FileStreamPutC(InterpreterContext *c,int ch,Stream *s)
{
    FileStream *fs = (FileStream *)s;
    return putc(ch,fs->fp);
}
