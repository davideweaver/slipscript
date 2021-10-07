/*************************************************************************/
/* Name: strnicmp.c 													 */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Compare characters of two strings without regard to case.		 */
/*                                                                       */
/* NOTE: Only use on non-Windows machines.						         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#ifndef WIN32

#include <string.h>
#include <malloc.h>

#include "str.h"
                                     
/* find a string in another string without regards to case */
int strnicmp( const char *str1, const char *str2, int count )
{
	char *sav_str, *sav_str2;
	int ret;

	sav_str = strcpy( ( char * ) malloc( strlen(str1) + 1 ), str1 );	
	sav_str2 = strcpy( ( char * ) malloc( strlen(str2) + 1 ), str2 );	
	strlwr( sav_str );
	strlwr( sav_str2 );
	ret = strncmp( sav_str, sav_str2, count );
	free( sav_str );
	free( sav_str2 );
	return ret;

}

#endif /* WIN32 */