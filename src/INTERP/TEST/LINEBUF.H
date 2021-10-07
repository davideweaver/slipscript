/* linebuf.c - line buffer functions */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#ifndef __LINEBUF_H__
#define __LINEBUF_H__

#define LBMAX	256

typedef struct linebuffer {
    char lb_buf[LBMAX];
    char *lb_ptr;
} LineBuffer;

LineBuffer *NewLineBuffer(void);
void FreeLineBuffer(LineBuffer *lbuf);
int FillLineBuffer(LineBuffer *lbuf,Stream *s);
int IsMoreOnLine(LineBuffer *lbuf);
int NextToken(LineBuffer *lbuf,char *arg);
int RestOfLine(LineBuffer *lbuf,char *buf);
int WholeLine(LineBuffer *lbuf,char *buf);
void FlushLineBuffer(LineBuffer *lbuf);

#endif

