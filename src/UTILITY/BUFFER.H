/* daves_buffer.h */
/* Copyright (c) 1996, David Weaver */

#ifndef DAVES_BUFFER_H_
#define DAVES_BUFFER_H_

#include <iostream.h>

class daves_buffer {
	char *buffer;
	char *pos;
	char *end;
	int len;

	int allocate_more( int i = 0);
public:
	daves_buffer( int size );
	~daves_buffer();
	char *append( char ch );
	char *append( char *str );
	char *data() { *pos = '\0'; return buffer; }
	char *assign( char *str );
	int length() { return ( pos - buffer ); }
	int bufsize() { return len; }
	void remove();
	friend ostream & operator<<( ostream & out, const daves_buffer & value );
	operator const char * () const { return buffer; }
};

#endif

