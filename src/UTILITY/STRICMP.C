/*************************************************************************/
/* Name: stricmp.c 														 */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Compare two strings without regard to case.					 */
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
int stricmp( const char *str1, const char *str2 )
{
	char *sav_str, *sav_str2;
	int ret;

	sav_str = strcpy( ( char * ) malloc( strlen(str1) + 1 ), str1 );	
	sav_str2 = strcpy( ( char * ) malloc( strlen(str2) + 1 ), str2 );	
	strlwr( sav_str );
	strlwr( sav_str2 );
	ret = strcmp( sav_str, sav_str2 );
	free( sav_str );
	free( sav_str2 );
	return ret;
}

#endif /* WIN32 */