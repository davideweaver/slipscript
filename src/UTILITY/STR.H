/* str.h - Copyright 1995,1996 David Weaver */


#ifndef STR_H_
#define STR_H_

/* prototype */
char *strstri( const char *str1, char *str2 );

#ifndef WIN32

char *strlwr( char *str );
int stricmp( const char *str1, const char *str2 );
int strnicmp( const char *str1, const char *str2, int count );

#endif /* WIN32 */

#endif