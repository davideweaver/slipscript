/* fdebug.h - Copyright 1995,1996 David Weaver */


#ifndef FDEBUG_H_
#define FDEBUG_H_

/* prototype */
void beginDebug( char *fname );
void writeDebug( char *message, int indent );
void writeInt( char *varname, int value );
void writeString( char *varname, char *value );
void writeFunction( char *name );
void endDebug();

#endif
 
