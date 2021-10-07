/****************************************************************/
/* Name: tcpstuff.cpp 		                                    */
/* Vers: 1.0                                                    */
/* Prog: David Weaver, dew118@psu.edu                           */
/*                                                              */
/* Desc: Provide an interface to tcp functions.					*/
/*                                                              */
/* NOTE: 													    */
/*       											            */
/*                                                              */
/* Copyright 1995,1996 David Weaver                				*/
/*                                                              */
/****************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include "tcpstuff.h"

#define FOREVER for (;;)

#ifndef TCPSTUFF_H
#define TCPSTUFF_H

/* initialize socket use, must be called first					*/
int startwinsockets() {

	WSADATA WSAData;

	if( WSAStartup( MAKEWORD(1,1), &WSAData ) ) return WSAGetLastError();
	return 0;
}

/* cleanup after all sockets are closed							*/
void stopwinsockets() {

	WSACleanup();
}

/* initialize and create an open socket							*/
int init_connection( char *machine, char *service, struct sockaddr_in *sockname ) {

	int s,nl,rv,repeat_count=0;
	struct hostent *hostname;
	struct servent *serv_rec;
	int error_code = 0;

	do 
	{
		/* initialize sockaddr */
		memset( sockname, 0, sizeof( struct sockaddr ) );

		/* create a socket */
		if( ( s = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) return WSAGetLastError();

		sockname->sin_family = PF_INET;		
		if( ( serv_rec = getservbyname( service, "tcp" ) ) == NULL ) return WSAGetLastError();
		sockname->sin_port = serv_rec->s_port;

		/* gets host information corresponding to a host name */
		if( ( hostname = gethostbyname( machine ) ) == NULL ) {
			int err_code;
			err_code = WSAGetLastError(); 
			closesocket(s);
			return err_code;
		}

		memcpy( ( void * ) &( sockname->sin_addr ),( void* ) hostname->h_addr, hostname->h_length );
		nl = sizeof( *sockname );
		if( ( rv = connect( s, ( struct sockaddr * )sockname, nl) ) ) {
			error_code = WSAGetLastError();
			closesocket( s );
			Sleep( 1000 );
			repeat_count++;
			if ( repeat_count > 1 ) 
				return error_code;
		}

	} while ( rv );

	return s;
}

/* gets the hostname of your machine							*/
int getmachinename(char *name) {

	char temp[200];
	struct hostent *host;

	/* get host name */
	if( gethostname( temp, 199 ) ) return WSAGetLastError();
	if( (host = gethostbyname( temp ) ) == NULL ) return WSAGetLastError();
	strcpy( name, host->h_name );
	return 0;
}

int getdomainname( char *name ) {

	char temp[200];
	int error_code;
	int pos;

	/* get host information */
	if( error_code = getmachinename( temp ) ) return error_code;
	pos = 0;
	while( temp[pos] != '.' ) pos++;
	pos++;
 	strcpy( name, temp + pos );
	return 0;
}

int sendstring( int socket, int options, char *format, ... ) {

	char temp[26000]; /* too much stack space */
	va_list argptr;

	va_start( argptr, format );
	vsprintf( temp, format, argptr );
	va_end( argptr );
	return sendstring( socket, temp, options );
}

int sendstring( int socket, char *name, int options ) {

	if( !send( socket, name, strlen( name ), options ) ) return WSAGetLastError();
	return 0;
}

#define tempLen 50

int getline( int socket, char *buffer ) {

	char temp[tempLen+1];
	int c, len, linelen = 0;
	buffer[0] = 0;

	FOREVER {

		len = recv( socket, temp, tempLen, MSG_PEEK );
		if ( len == 0 ) return WSAGetLastError();
		
		if ( len > 0 ) {
			
			c = 0;
			while( ( ( temp[c] != '\n' ) && c < len ) ) c++;
			if( !recv( socket, temp, c, 0) ) return WSAGetLastError();
			linelen = c;
			for( c = 0; c < linelen; c++ ) {
				if( temp[c] == '\r' ) {
					memmove( temp + c, temp + c + 1, linelen - ( c + 1 ) );
					linelen--;	
				}
			}
			temp[linelen] = 0;
			strcat( buffer, temp );
			if ( c != len ) {
				if( !recv( socket, temp, 1, 0 ) ) return WSAGetLastError();
				return 0;
			}
		}
	}
}

#endif
