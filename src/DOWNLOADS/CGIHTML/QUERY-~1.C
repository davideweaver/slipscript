/* query-results.c - generic query program using either GET or POST
   Eugene Kim, eekim@fas.harvard.edu
   $Id: query-results.c,v 1.6 1996/08/11 04:22:01 eekim Exp eekim $

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/

#include <stdio.h>
#include "cgi-lib.h"
#include "html-lib.h"

int main()
{
  llist entries;

  html_header();
  html_begin("Query Results");
  if (read_cgi_input(&entries)) {
    h1("Query results");
    print_entries(entries);
  }
  else {
    h1("No input!");
  }
  html_end();
  list_clear(&entries);
  return 0;
}
