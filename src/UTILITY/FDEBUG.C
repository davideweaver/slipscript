/*************************************************************************/
/* Name: fdebug.c 		                                                 */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Provide an interface to send debug info to a file.		         */
/*                                                                       */
/* NOTE: 														         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#include <time.h> 
#include <stdio.h>       

#include "fdebug.h"

/* global variables */
static FILE *debug_fp;

/* start a debug session, basically open a file */                             
void beginDebug( char *fname ) {

	char tmpbuf[128];

	debug_fp = fopen( fname, "w" );
	if( debug_fp == NULL ) return;
	fprintf( debug_fp, "###   BEGINNING DEBUG SESSION\n" );
	fprintf( debug_fp, "###   DATE: " );
	_strdate( tmpbuf );
	fprintf( debug_fp, tmpbuf );
	fprintf( debug_fp, "\n" );
	fprintf( debug_fp, "###   TIME: " );
	_strtime( tmpbuf );
	fprintf( debug_fp, tmpbuf );
	fprintf( debug_fp, "\n\n" );
}

/* write a message to the debug file */
void writeDebug( char *message, int indent ) {

	int i;

	if( debug_fp == NULL ) return;
	for( i = 0; i <= indent; i++ )
		fprintf( debug_fp, "\t" );
	fprintf( debug_fp, "# " );
	fprintf( debug_fp, message );
	fprintf( debug_fp, "\n" );
}

/* write an integer value to the debug file */
void writeInt( char *varname, int value ) {

	if( debug_fp == NULL ) return;
	fprintf( debug_fp, "\n" );
	fprintf( debug_fp, "# INTEGER: " );
	fprintf( debug_fp, varname );
	fprintf( debug_fp, " = " );
	fprintf( debug_fp, "%i\n", value );
}

/* write a string value to the debug file */
void writeString( char *varname, char *value ) {

	if( debug_fp == NULL ) return;
	fprintf( debug_fp, "\n" );
	fprintf( debug_fp, "# CHAR *: " );
	fprintf( debug_fp, varname );
	fprintf( debug_fp, "\n" );
	fprintf( debug_fp, "# = " );
	fprintf( debug_fp, "%s\n", value );
}

/* write the entrance of a function to the debug file */
void writeFunction( char *name ) {

	if( debug_fp == NULL ) return;
	fprintf( debug_fp, "\n" );
	fprintf( debug_fp, "# ENTERING FUNCTION : " );
	fprintf( debug_fp, "%s\n", name );
}

/* close the debug file */
void endDebug() {

	char tmpbuf[128];

	if( debug_fp == NULL ) return;
	fprintf( debug_fp, "\n###   ENDING DEBUG SESSION\n" );
	fprintf( debug_fp, "###   DATE: " );
	_strdate( tmpbuf );
	fprintf( debug_fp, tmpbuf );
	fprintf( debug_fp, "\n" );
	fprintf( debug_fp, "###   TIME: " );
	_strtime( tmpbuf );
	fprintf( debug_fp, tmpbuf );
	fprintf( debug_fp, "\n\n" );
	fclose( debug_fp );
}
