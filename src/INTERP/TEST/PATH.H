/* path.h - TechNet path routine definitions */
/*
	Copyright (c) 1994, David Betz and M&T Publishing, Inc.
	All rights reserved
*/

#ifndef __PATH_H__
#define __PATH_H__

/* maximum path size */
#define PSIZE		100

/* keys for ConfPath */
#define CP_INDEX	1	/* index file */
#define CP_DATA		2	/* data file */

char *ConfPath(char *path,int key);
char *ConfPath1(char *path,int key,int arg);

#endif