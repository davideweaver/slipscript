/* string-lib.c - generic string processing routines
   $Id: string-lib.c,v 1.3 1996/07/24 07:32:22 eekim Exp eekim $

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved.
*/

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "string-lib.h"

/* creates a new string */
char *newstr(char *str)
{
  return strcpy((char *)malloc(sizeof(char) * strlen(str)+1),str);
}

/* retrieves a substring in a string */
char *substr(char *str, int offset, int len)
{
  int slen, start, i;
  char *nstr;

  if (str == NULL)
    return NULL;
  else
    slen = strlen(str);
  nstr = malloc(sizeof(char) * slen + 1);
  if (offset >= 0)
    start = offset;
  else
    start = slen + offset - 1;
  if ( (start < 0) || (start > slen) ) /* invalid offset */
    return NULL;
  for (i = start; i < start+len; i++)
    nstr[i - start] = str[i];
  nstr[len] = '\0';
  return nstr;
}

/* replace < and > with &lt; and &gt; for HTML purposes */
char *replace_ltgt(char *str)
{
  int i,j = 0;
  char *nstr = malloc(sizeof(char) * (strlen(str) * 4 + 1));

  for (i = 0; i < (int)strlen(str); i++) {
    if (str[i] == '<') {
      nstr[j] = '&';nstr[j+1] = 'l';nstr[j+2] = 't';nstr[j+3] = ';';
      j += 3;
    }
    else if (str[i] == '>') {
      nstr[j] = '&';nstr[j+1] = 'g';nstr[j+2] = 't';nstr[j+3] = ';';
      j += 3;
    }
    else
      nstr[j] = str[i];
    j++;
  }
  nstr[j] = '\0';
  return nstr;
}
