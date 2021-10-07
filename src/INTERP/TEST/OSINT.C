/* osint.c - operating system interface routines */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#include "osint.h"

/* htonl - host to network long */
long htonl(long val)
{
    return (((val >> 24) & 0x000000FFL)
	|   ((val >>  8) & 0x0000FF00L)
	|   ((val <<  8) & 0x00FF0000L)
	|   ((val << 24) & 0xFF000000L));
}

/* htons - host to network short */
short htons(short val)
{
    return (((val >> 8) & 0x00FF)
	|   ((val << 8) & 0xFF00));
}

/* ntohl - network to host long */
long ntohl(long val)
{
    return (((val >> 24) & 0x000000FFL)
	|   ((val >>  8) & 0x0000FF00L)
	|   ((val <<  8) & 0x00FF0000L)
	|   ((val << 24) & 0xFF000000L));
}

/* ntohs - network to host short */
short ntohs(short val)
{
    return (((val >> 8) & 0x00FF)
	|   ((val << 8) & 0xFF00));
}

/* oscvtpath - convert a path to os format */
void oscvtpath(char *filepath,char *path)
{
    char *p;
    int ch;
    p = filepath;
    do {
	*p++ = (ch = *path++) == '/' ? '\\' : ch;
    } while (ch != '\0');
}

/* osfilesize - return the size of a file */
long osfilesize(FILE *fp)
{
    long size,old_pos = ftell(fp);
    int sts = fseek(fp,0,SEEK_END);
    if (sts < 0) return sts;
    size = ftell(fp);
    fseek(fp,old_pos,SEEK_SET);
    return size;
}

/* oslock - lock a file */
int oslock(FILE *fp)
{
    return 1;
}

/* osunlock - unlock a file */
void osunlock(FILE *fp)
{
}

