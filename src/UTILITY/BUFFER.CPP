/*************************************************************************/
/* Name: daves_buffer.cpp 		                                         */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Provide an interface to a ver simple buffer.			         */
/*                                                                       */
/* NOTE: 														         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"


daves_buffer::daves_buffer( int size ) {

	if( !size ) {
		buffer = NULL;
		pos = NULL;
		return;
	}

	buffer = ( char * )malloc( size );
	pos = buffer;
	end = pos + size;
	len = size;
}

daves_buffer::~daves_buffer() {

	if( buffer ) free( buffer );
}

char *daves_buffer::append( char ch ) {

	if( pos < ( end - 1 ) ) {
		*pos = ch;
		pos++;
	}
	else {
		/* not enough room, allocate more */
		if( allocate_more() ) return NULL;

		return append( ch );
	}

	return buffer;
}

char *daves_buffer::append( char *str ) {

	int size = strlen( str );
	if( ( pos + size ) < end ) {
		memcpy(pos, str, size );
		pos += size;
	}
	else {
		/* not enough room, allocate more */
		if( allocate_more() ) return NULL;

		return append( str );
	}

	return buffer;
}

char *daves_buffer::assign( char *str ) {

	int size = strlen( str );
	if( str == NULL ) return NULL;
	if( size >= len )
		if( allocate_more( size + 1) ) return NULL;
	memcpy( buffer, str, size );
	pos = buffer + size;
	end = buffer + len;
	return buffer;
}

int daves_buffer::allocate_more( int i ) {

	/* not enough room, allocate more */
	char *tempbuf1, *tempbuf2;
	int pos_size;

	if( i == 0 ) 
		len = len*2;
	else 
		len = i;
	if( ( tempbuf1 = ( char * )malloc( len ) ) == NULL ) return 1;
	tempbuf2 = buffer;

	memcpy( tempbuf1, buffer, len/2 );
	pos_size = pos - buffer;
	pos = pos_size + tempbuf1;
	end = tempbuf1 + len;

	buffer = tempbuf1;
	free( tempbuf2 );
	return 0;
}

void daves_buffer::remove() {

	/* clears the buffer, but does not deallocate memory */
	memset( buffer, '\0', len );
	pos = buffer;
}

ostream & operator<<( ostream & out, const daves_buffer & value ) {

	return out << value.buffer;
}