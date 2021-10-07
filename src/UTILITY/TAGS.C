/*************************************************************************/
/* Name: tags.c 														 */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Performs tag parsing on strings and files.				         */
/*                                                                       */
/* NOTE: 														         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>      /* For optional main section */  
#include <stdlib.h>
#include <string.h>

#include "str.h"
#include "tags.h"
                                     
/* given a string and a comma seperated pretag and		*/
/* post tag (ie. "<TITLE>,</TITLE>"), this function		*/
/* will return the text between the tags.				*/
/* returns a pointer to the string on success or		*/
/* NULL on error. (not case sensitve)					*/
char *getTagInString(const char *tagged_str, char *tags) 
{
	int differ, i; 
	char *pre, *post, *ret_str;
	char *pos, *start, *end;
	char *str = '\0';

	/* seperate tags */
	if((pre  = strtok(tags, ",")) == NULL) return NULL;
	post = strtok(NULL, ",");

	/* find start and end pos */
	start = pos = strstri(tagged_str,pre);
	if(start == NULL) return NULL;
	start += (strlen(pre));
	if((end = strstri(pos,post)) == NULL) return NULL;
	
	/* allocate memory for substring */
	differ = end - start;
	if((str = (char *)malloc((size_t)differ + 1)) == NULL) return NULL;
	memset(str, '\0', (size_t)differ + 1);

	/* copy string */
	for(i=0; i<differ; i++)
		str[i] = start[i];
	str[i] = '\0';
	ret_str = str;

	return ret_str;
}				

/* retrieves a value found in the tag's command.			*/
/* a tag would be like name|tok|value (i.e. name=value)		*/
char *getTagValue(char *command, char *name, char *tok) {

	char *ptr, *endptr;
	char *query_string;
	char *namestart;
	char *ret_val = "";
	unsigned int i;

	if ( ! command )
		return ( "" );

	query_string = (char *) malloc ( strlen ( command ) + 2 );
	memset ( query_string, '\0', strlen ( command ) + 2 );
	strcpy ( query_string, command );

	ptr = strstri( query_string, name );
	while( ptr ) {
		/* move to end of name */
		ptr += strlen( name );
		/* remove any spaces newlines or tabs */
		while( ptr[0] == ' ' || ptr[0] == '\t' || ptr[0] == '\n' ) ptr++;
		if( ptr[0] == '=' ) {
			ptr++;
			/* remove any spaces newlines or tabs */
			while( ptr[0] == ' ' || ptr[0] == '\t' || ptr[0] == '\n' ) ptr++;
			if( ptr[0] == '\"' ) {  /* value in quotes */
				ptr++;
				endptr = strtok( ptr, "\"" );
				if( endptr == NULL ) return NULL;
				endptr = '\0';
				return ptr; /* must be deleted */
			}
			/* not in quotes, only return 1 word */
			endptr = strtok( ptr, " " );
			if( endptr == NULL ) return ptr;
			endptr = '\0';
			return ptr;
		}
		ptr = strstri( ptr, name );
	}
	return NULL;

	/*namestart = (char *) malloc ( strlen ( name ) + 2 );
	sprintf ( namestart, "%s=", name );

	ptr = strtok ( query_string, tok );
	while( ptr ) {
		// remove all newline, tab, and etc. chars
		while( ptr[0] <= 10 ) ptr++;
		if( strnicmp ( ptr, namestart, strlen ( namestart ) ) == 0 ) {
			// check if the value is within quotes
			char *where = ( ptr + strlen ( namestart ) );
			if( where[0] == '\"') {
				// get quoted value
				endptr = strtok ( NULL, "\"");
				ptr[strlen(ptr)] = ' ';
				ptr++;
				ret_val = ptr + strlen ( namestart );
			}
			else {
				ret_val = ptr + strlen ( namestart );
				// limit value to one word if no quotes are used
				for (i=0; i<strlen(ret_val); i++) 
					if(ret_val[i] <= 10) {
						ret_val[i] = '\0';
						break;
				}
			}
			free( namestart );
			return ( ret_val );
		}
		ptr = strtok ( NULL, tok );
	}
	free( namestart );
	free( query_string );
	return NULL;*/
}
