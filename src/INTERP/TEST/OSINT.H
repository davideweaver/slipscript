/* osint.h - operating system interface routine definitions */
/*
	Copyright (c) 1995, by David Michael Betz
	All rights reserved
*/

#ifndef __OSINT_H__
#define __OSINT_H__

#include <stdio.h>

/* prototypes */
long htonl(long val);
short htons(short val);
long ntohl(long val);
short ntohs(short val);
void oscvtpath(char *filepath,char *path);
long osfilesize(FILE *fp);
int oslock(FILE *fp);
void osunlock(FILE *fp);

#endif
