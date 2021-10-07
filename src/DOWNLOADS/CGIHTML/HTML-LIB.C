/* html-lib.c - C routines that output various HTML constructs
   Eugene Kim, eekim@fas.harvard.edu
   $Id: html-lib.c,v 1.5 1996/08/11 04:22:01 eekim Exp eekim $

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "html-lib.h"

/* HTTP headers */

void html_header()
{
  printf("Content-type: text/html\r\n\r\n");
}

void mime_header(char *mime)
/* char *mime = valid mime type */
{
  printf("Content-type: %s\r\n\r\n",mime);
}

void nph_header(char *status)
{
  printf("HTTP/1.0 %s\r\n",status);
  printf("Server: CGI using cgihtml\r\n");
}

void show_html_page(char *loc)
{
  printf("Location: %s\r\n\r\n",loc);
}

void status(char *status)
{
  printf("Status: %s\r\n",status);
}

void pragma(char *msg)
{
  printf("Pragma: %s\r\n",msg);
}

/* HTML shortcuts */

void html_begin(char *title)
{
  printf("<html> <head>\n");
  printf("<title>%s</title>\n",title);
  printf("</head>\n\n");
  printf("<body>\n");
}

void html_end()
{
  printf("</body> </html>\n");
}

/* what's the best way to implement these tags?  Think about this a little
more before you settle on a way to do this. */

void h1(char *header)
{
  printf("<h1>%s</h1>\n",header);
}

void h2(char *header)
{
  printf("<h2>%s</h2>\n",header);
}

void h3(char *header)
{
  printf("<h3>%s</h3>\n",header);
}

void h4(char *header)
{
  printf("<h4>%s</h4>\n",header);
}

void h5(char *header)
{
  printf("<h5>%s</h5>\n",header);
}

void h6(char *header)
{
  printf("<h6>%s</h6>\n",header);
}

/* state related functions */
void hidden(char *name, char *value)
{
  printf("<input type=hidden name=\"%s\" value=\"%s\">\n",name,value);
}
