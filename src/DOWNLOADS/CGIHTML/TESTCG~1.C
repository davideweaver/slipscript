/* test.cgi.c - Uses functions print_cgi_env() and print_entries()
     to test CGI.

   Eugene Kim, eekim@fas.harvard.edu
   $Id: test.cgi.c,v 1.4 1996/08/11 04:22:01 eekim Exp eekim $

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/

#include <stdio.h>
#include "html-lib.h"
#include "cgi-lib.h"

int main()
{
  llist entries;

  html_header();
  html_begin("Test CGI");
  h1("CGI Test Program");
  printf("<hr>\n");
  h2("CGI Environment Variables");
  print_cgi_env();
  if (read_cgi_input(&entries)) {
    h2("CGI Entries");
    print_entries(entries);
  }
  html_end();
  list_clear(&entries);
  return 0;
}

