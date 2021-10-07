/* cgi-lib.c - C routines that make writing CGI scripts in C a breeze
   Eugene Kim, <eekim@fas.harvard.edu>
   $Id: cgi-lib.c,v 1.9 1996/08/11 04:22:01 eekim Exp eekim $
   Motivation: Perl is a much more convenient language to use when
     writing CGI scripts.  Unfortunately, it is also a larger drain on
     the system.  Hopefully, these routines will make writing CGI
     scripts just as easy in C.

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "cgi-lib.h"
#include "html-lib.h"
#include "string-lib.h"

#ifdef UNIX
void die()
{
/* this routine needs some beefing up.  I hope to eventually add:
     o more detailed information
     o error logging
     o perhaps sending a header which signifies an internal error
*/
  html_header();
  html_begin("CGI Error");
  printf("<h1>CGI Error</h1>\r\n");
  printf("An internal error has occured.  Please contact your web\r\n");
  printf("administrator.  Thanks.\r\n");
  html_end();
  exit(1);
}
#endif

short accept_image()
{
  char *httpaccept = getenv("HTTP_ACCEPT");

  if (strstr(httpaccept,"image") == NULL)
    return 0;
  else
    return 1;
}

/* x2c() and unescape_url() stolen from NCSA code */
char x2c(char *what)
{
  register char digit;

  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}

void unescape_url(char *url)
{
  register int x,y;

  for (x=0,y=0; url[y]; ++x,++y) {
    if((url[x] = url[y]) == '%') {
      url[x] = x2c(&url[y+1]);
      y+=2;
    }
  }
  url[x] = '\0';
}

char *get_DEBUG()
{
  int bufsize = 1024;
  char *buffer = malloc(sizeof(char) * bufsize + 1);
  int i = 0;
  char ch;

  fprintf(stderr,"\n--- cgihtml Interactive Mode ---\n");
  fprintf(stderr,"Enter CGI input string.  Remember to encode appropriate ");
  fprintf(stderr,"characters.\nPress ENTER when done:\n\n");
  while ( (i<=bufsize) && ((ch = getc(stdin)) != '\n') ) {
    buffer[i] = ch;
    i++;
    if (i>bufsize) {
      bufsize *= 2;
      buffer = realloc(buffer,bufsize);
    }
  }
  buffer[i] = '\0';
  fprintf(stderr,"\n Input string: %s\nString length: %d\n",buffer,i);
  fprintf(stderr,"--- end cgihtml Interactive Mode ---\n\n");
  return buffer;
}

char *get_POST()
{
  int content_length;
  char *buffer;

  if (CONTENT_LENGTH != NULL) {
    content_length = atoi(CONTENT_LENGTH);
    buffer = malloc(sizeof(char) * content_length + 1);
    if (fread(buffer,sizeof(char),content_length,stdin) != content_length) {
      /* consistency error. */
      fprintf(stderr,"caught by cgihtml: input length < CONTENT_LENGTH\n");
      exit(1);
    }
    buffer[content_length] = '\0';
  }
  else { /* null content length */
    /* again, perhaps more detailed, robust error message here */
    fprintf(stderr,"caught by cgihtml: CONTENT_LENGTH is null\n");
    exit(1);
  }
  return buffer;
}

char *get_GET()
{
  char *buffer;

  if (QUERY_STRING == NULL) {
    fprintf(stderr,"caught by cgihtml: QUERY_STRING is null\n");
    exit(1);
  }
  buffer = newstr(QUERY_STRING);
  return buffer;
}

int parse_CGI_encoded(llist *entries, char *buffer)
{
  int i,j,numentries;
  int content_length = strlen(buffer);
  short NM = 1;
  entrytype entry;
  node* window;

  list_create(entries);
  window = entries->head;
  if (content_length == 0)
    return 0;
  else {
    numentries = 0;
    j = 0;
    entry.name = malloc(sizeof(char) * content_length + 1);
    entry.value = malloc(sizeof(char) * content_length + 1);
    for (i = 0; i < content_length; i++) {
      if (buffer[i] == '=') {
	entry.name[j] = '\0';
	unescape_url(entry.name);
	if (i == content_length - 1) {
	  strcpy(entry.value,"");
	  window = list_insafter(entries,window,entry);
	}
	j = 0;
	NM = 0;
      }
      else if ( (buffer[i] == '&') || (i == content_length - 1) ) {
	if (i == content_length - 1) {
	  entry.value[j] = buffer[i];
	  j++;
	}
	entry.value[j] = '\0';
	unescape_url(entry.value);
	window = list_insafter(entries,window,entry);
	j = 0;
	NM = 1;
	numentries++;
      }
      else if (NM) {
	if (buffer[i] == '+')
	  entry.name[j] = ' ';
	else
	  entry.name[j] = buffer[i];
	j++;
      }
      else if (!NM) {
	if (buffer[i] == '+')
	  entry.value[j] = ' ';
	else
	  entry.value[j] = buffer[i];
	j++;
      }
    }
    return numentries;
  }
}

/* stolen from k&r and seriously modified to do what I want */

int getline(char s[], int lim)
{
  int c, i=0, num;

  for (i=0; (i<lim) && ((c=getchar())!=EOF) && (c!='\n'); i++)
    {
    s[i] = c;
    }
  if (c == '\n') {
    s[i] = c;
  }
  if ((i==0) && (c!='\n'))
    num = 0;
  else if (i == lim)
    num = i;
  else
    num = i+1;
  return num;
}

int parse_form_encoded(llist* entries)
{
  long content_length;
  entrytype entry;
  node* window;
  FILE *uploadfile;
  char *uploadfname, *tempstr, *boundary;
  char *buffer = malloc(sizeof(char) * BUFSIZ + 1);
  char *prevbuf = malloc(sizeof(char) + BUFSIZ + 1);
  short isfile,done,start;
  int i,bytesread,prevbytesread,numentries = 0;

  if (CONTENT_LENGTH != NULL)
    content_length = atol(CONTENT_LENGTH);
  else
    return 0;
  /* get boundary */
  tempstr = newstr(CONTENT_TYPE);
  boundary = strstr(tempstr,"boundary=");
  boundary += (sizeof(char) * 9);
  /* create list */
  list_create(entries);
  window = entries->head;
  /* ignore first boundary; this isn't so robust; improve it later */
  getline(buffer,BUFSIZ);
  /* now start parsing */
  while ((bytesread=getline(buffer,BUFSIZ)) != 0) {
    start = 1;
    buffer[bytesread] = '\0';
    tempstr = newstr(buffer);
    tempstr += (sizeof(char) * 38); /* 38 is header up to name */
    entry.name = tempstr;
    entry.value = malloc(BUFSIZ);
    strcpy(entry.value,"");
    while (*tempstr != '"')
      tempstr++;
    *tempstr = '\0';
    if (strstr(buffer,"filename=\"") != NULL) {
      isfile = 1;
      tempstr = newstr(buffer);
      tempstr = strstr(tempstr,"filename=\"");
      tempstr += (sizeof(char) * 10);
      uploadfname = malloc(strlen(UPLOADDIR) + strlen(tempstr) + 2);
      sprintf(uploadfname,"%s/%s",UPLOADDIR,tempstr);
      tempstr = uploadfname;
      while (*tempstr != '"')
	tempstr++;
      *tempstr = '\0';
      if ( (uploadfile = fopen(uploadfname,"w")) == NULL) {
	fprintf(stderr,"cgihtml Error: cannot upload file\n");
	exit(1);
      }
    }
    else
      isfile = 0;
    /* ignore rest of headers and first blank line */
    getline(buffer,BUFSIZ);
    if (isfile) getline(buffer,BUFSIZ);
    done = 0;
    while (!done) {
      bytesread = getline(buffer,BUFSIZ);
      buffer[bytesread] = '\0';
      if (strstr(buffer,boundary) == NULL) {
	if (start) {
	  i = 0;
	  while (i < bytesread) {
	    prevbuf[i] = buffer[i];
	    i++;
	  }
	  prevbytesread = bytesread;
	  start = 0;
	}
	else {
	  /* flush buffer */
	  i = 0;
	  while (i < prevbytesread) {
	    if (isfile)
	      fputc(prevbuf[i],uploadfile);
	    else
	      entry.value[i] = prevbuf[i];
	    i++;
	  }
	  /* buffer new input */
	  i = 0;
	  while (i < bytesread) {
	    prevbuf[i] = buffer[i];
	    i++;
	  }
	  prevbytesread = bytesread;
	}
      }
      else {
	done = 1;
	/* flush buffer except last two characters */
	i = 0;
	while (i < prevbytesread - 2) {
	  if (isfile)
	    fputc(prevbuf[i],uploadfile);
	  else
	    entry.value[i] = prevbuf[i];
	  i++;
	}
      }
    }
    if (isfile)
      fclose(uploadfile);
    else {
      window = list_insafter(entries,window,entry);
      numentries++;
    }
  }
  return numentries;
}

int read_cgi_input(llist* entries)
{
  char *input;

  /* check for form upload.  this needs to be first, because the
     standard way of checking for POST data is inadequate.  If you
     are uploading a 100 MB file, you are unlikely to have a buffer
     in memory large enough to store the raw data for parsing.
     Instead, parse_form_encoded parses stdin directly.

     In the future, I may modify parse_CGI_encoded so that it also
     parses POST directly from stdin. */
  if ((CONTENT_TYPE != NULL) &&
      (strstr(CONTENT_TYPE,"multipart/form-data") != NULL) )
    return parse_form_encoded(entries);

  /* get the input */
  if (REQUEST_METHOD == NULL)
    input = get_DEBUG();
  else if (!strcmp(REQUEST_METHOD,"POST"))
    input = get_POST();
  else if (!strcmp(REQUEST_METHOD,"GET"))
    input = get_GET();
  else { /* error: invalid request method */
    fprintf(stderr,"caught by cgihtml: REQUEST_METHOD invalid\n");
    exit(1);
  }
  /* parse the input */
  return parse_CGI_encoded(entries,input);
}

char *cgi_val(llist l, char *name)
{
  short FOUND = 0;
  node* window;

  window = l.head;
  while ( (window != 0) && (!FOUND) )
    if (!strcmp(window->entry.name,name))
      FOUND = 1;
    else
      window = window->next;
  if (FOUND)
    return window->entry.value;
  else
    return NULL;
}

/* cgi_val_multi - contributed by Mitch Garnaat <garnaat@wrc.xerox.com>;
   modified by me */

char **cgi_val_multi(llist l, char *name)
{
  short FOUND = 0;
  node* window;
  char **ret_val = 0;
  int num_vals = 0, i;

  window = l.head;
  while (window != 0) {
    if (!strcmp(window->entry.name,name)) {
      FOUND = 1;
      num_vals++;
    }
    window = window->next;
  }
  if (FOUND) {
    /* copy the value pointers into the returned array */
    ret_val = (char**) malloc(sizeof(char*) * (num_vals + 1));
    window = l.head;
    i = 0;
    while (window != NULL) {
      if (!strcmp(window->entry.name,name)) {
	ret_val[i] = window->entry.value;
	i++;
      }
      window = window->next;
    }
    /* NULL terminate the array */
    ret_val[i] = 0;
    return ret_val;
  }
  else
    return NULL;
}

char *cgi_name(llist l, char *value)
{
  short FOUND = 0;
  node* window;

  window = l.head;
  while ( (window != 0) && (!FOUND) )
    if (!strcmp(window->entry.value,value))
      FOUND = 1;
    else
      window = window->next;
  if (FOUND)
    return window->entry.name;
  else
    return NULL;
}

char **cgi_name_multi(llist l, char *value)
{
  short FOUND = 0;
  node* window;
  char **ret_val = 0;
  int num_vals = 0, i;

  window = l.head;
  while (window != 0) {
    if (!strcmp(window->entry.value,value)) {
      FOUND = 1;
      num_vals++;
    }
    window = window->next;
  }
  if (FOUND) {
    /* copy the value pointers into the returned array */
    ret_val = (char**) malloc(sizeof(char*) * (num_vals + 1));
    window = l.head;
    i = 0;
    while (window != NULL) {
      if (!strcmp(window->entry.value,value)) {
	ret_val[i] = window->entry.name;
	i++;
      }
      window = window->next;
    }
    /* NULL terminate the array */
    ret_val[i] = 0;
    return ret_val;
  }
  else
    return NULL;
}

/* miscellaneous useful CGI routines */

void print_cgi_env()
{
  printf("<p>SERVER_SOFTWARE = %s<br>\n",SERVER_SOFTWARE);
  printf("SERVER_NAME = %s<br>\n",SERVER_NAME);
  printf("GATEWAY_INTERFACE = %s<br>\n",GATEWAY_INTERFACE);

  printf("SERVER_PROTOCOL = %s<br>\n",SERVER_PROTOCOL);
  printf("SERVER_PORT = %s<br>\n",SERVER_PORT);
  printf("REQUEST_METHOD = %s<br>\n",REQUEST_METHOD);
  printf("PATH_INFO = %s<br>\n",PATH_INFO);
  printf("PATH_TRANSLATED = %s<br>\n",PATH_TRANSLATED);
  printf("SCRIPT_NAME = %s<br>\n",SCRIPT_NAME);
  printf("QUERY_STRING = %s<br>\n",QUERY_STRING);
  printf("REMOTE_HOST = %s<br>\n",REMOTE_HOST);
  printf("REMOTE_ADDR = %s<br>\n",REMOTE_ADDR);
  printf("AUTH_TYPE = %s<br>\n",AUTH_TYPE);
  printf("REMOTE_USER = %s<br>\n",REMOTE_USER);
  printf("REMOTE_IDENT = %s<br>\n",REMOTE_IDENT);
  printf("CONTENT_TYPE = %s<br>\n",CONTENT_TYPE);
  printf("CONTENT_LENGTH = %s<br></p>\n",CONTENT_LENGTH);
}

void print_entries(llist l)
{
  node* window;

  window = l.head;
  printf("<dl>\n");
  while (window != NULL) {
    printf("  <dt> <b>%s</b>\n",window->entry.name);
    printf("  <dd> %s\n",replace_ltgt(window->entry.value));
    window = window->next;
  }
  printf("</dl>\n");
}

char *escape_input(char *str)
/* takes string and escapes all metacharacters.  should be used before
   including string in system() or similar call. */
{
  int i,j = 0;
  char *new = malloc(sizeof(char) * (strlen(str) * 2 + 1));

  for (i = 0; i < strlen(str); i++) {
    if (!( ((str[i] >= 'A') && (str[i] <= 'Z')) ||
	   ((str[i] >= 'a') && (str[i] <= 'z')) ||
	   ((str[i] >= '0') && (str[i] <= '9')) )) {
      new[j] = '\\';
      j++;
    }
    new[j] = str[i];
    j++;
  }
  new[j] = '\0';
  return new;
}

short is_form_empty(llist l)
{
  node* window;
  short EMPTY = 1;

  window = l.head;
  while ( (window != NULL) && (EMPTY == 1) ) {
    if (strcmp(window->entry.value,""))
      EMPTY = 0;
    window = window->next;
  }
  return EMPTY;
}
