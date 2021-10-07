/* linebuf.c - line buffer functions */
/*
	Copyright (c) 1994, by David Michael Betz
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "execute.h"
#include "streams.h"
#include "linebuf.h"

/* local functions */
static int SkipSpaces(char **sptr);

/* NewLineBuffer - create a line buffer */
LineBuffer *NewLineBuffer(void)
{
    LineBuffer *lbuf;
    
    /* allocate the line buffer structure */
    if ((lbuf = (LineBuffer *)malloc(sizeof(LineBuffer))) == NULL)
	return NULL;

    /* initialize the line buffer */
    FlushLineBuffer(lbuf);
    return lbuf;
}

/* FreeLineBuffer - free a line buffer */
void FreeLineBuffer(LineBuffer *lbuf)
{
    free((void *)lbuf);
}

/* FillLineBuffer - fill a line buffer */
int FillLineBuffer(LineBuffer *lbuf,Stream *s)
{
    FlushLineBuffer(lbuf);
    return GetLine(s,lbuf->lb_buf,LBMAX) == 0;
}

/* NextToken - get next space delimited token */
int NextToken(LineBuffer *lbuf,char *arg)
{
    int ch;

    /* skip leading spaces */
    if ((ch = SkipSpaces(&lbuf->lb_ptr)) == '\0')
	return FALSE;

    /* get the next space delimited token */
    for (; (ch = *lbuf->lb_ptr) != '\0' && !isspace(ch); ++lbuf->lb_ptr)
	*arg++ = ch;
    *arg = '\0';

    /* return successfully */
    return TRUE;
}

/* IsMoreOnLine - check to see if there more on the line */
int IsMoreOnLine(LineBuffer *lbuf)
{
    return SkipSpaces(&lbuf->lb_ptr);
}

/* RestOfLine - get the rest of the current line */
int RestOfLine(LineBuffer *lbuf,char *buf)
{
    if (SkipSpaces(&lbuf->lb_ptr))
	return WholeLine(lbuf,buf);
    return FALSE;
}

/* WholeLine - get the whole (remaining) line */
int WholeLine(LineBuffer *lbuf,char *buf)
{
    strcpy(buf,lbuf->lb_ptr);
    FlushLineBuffer(lbuf);
    return TRUE;
}

/* FlushLineBuffer - flush a line buffer */
void FlushLineBuffer(LineBuffer *lbuf)
{
    lbuf->lb_ptr = lbuf->lb_buf;
    *lbuf->lb_ptr = '\0';
}

/* SkipSpaces - skip leading spaces */
static int SkipSpaces(char **sptr)
{
    int ch;
    while ((ch = **sptr) != '\0' && isspace(ch))
	++(*sptr);
    return ch;
}

