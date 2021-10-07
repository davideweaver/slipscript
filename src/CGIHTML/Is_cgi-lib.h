/* is_cgi-lib.h - header file for is_cgi.c

   Copyright (C) 1996 David Weaver
   All Rights Reserved
*/

#ifndef IS_CGI_LIB_H_
#define IS_CGI_LIB_H_

#include <stdlib.h>
#include <httpext.h>
#include "cgi-lib.h"
#include "cgi-llist.h"


#ifdef WIN32
int is_read_cgi_input(EXTENSION_CONTROL_BLOCK *pECB, llist* entries);
void is_html_error( EXTENSION_CONTROL_BLOCK *pECB, char *title, char *message, char *email );
#endif /* WIN32 */

#endif