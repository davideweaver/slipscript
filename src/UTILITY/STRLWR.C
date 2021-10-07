/*************************************************************************/
/* Name: strlwr.c 														 */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Makes a null terminated string all lower case.			         */
/*                                                                       */
/* NOTE: Will not be used when compiled on Windows machines.	         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#ifndef WIN32

#include "str.h"
                                     
/* find a string in another string without regards to case */
char *strlwr( char *str ) {

	char *p = str;
	int i = 0;

	while( p[i] != '\0')
		if( p[i] <= 90 && p[i] >= 65 ) p[i] += 32;
		else i++;
	return ( char * ) str;
}

#endif /* WIN32 */