/*************************************************************************/
/* Name: strstri.c 														 */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Finds a string in a string without regards to case.	         */
/*                                                                       */
/* NOTE: 														         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#include <string.h>
#include <malloc.h>

#include "str.h"
                                     
/* find a string in another string without regards to case */
char *strstri(const char *str1, char *str2) {

	char *sav_str, *pos, *ret;

	sav_str = strcpy( ( char * ) malloc( strlen(str1) + 1 ), str1 );	
	strlwr( sav_str );
	strlwr( str2 );
	if( ( pos = strstr( sav_str, str2 ) ) == NULL )
		return NULL;
	ret = ( char * ) &str1[pos - sav_str];
	free( sav_str );
	return ret;

}
