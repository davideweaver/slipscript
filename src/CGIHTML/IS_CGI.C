/****************************************************************/
/* Name: is_cgi.c	 		                                    */
/* Vers: 1.0                                                    */
/* Prog: David Weaver, dew118@psu.edu                           */
/*                                                              */
/* Desc: CGI variable decoding for ISAPI interface.				*/
/*                                                              */
/* NOTE: uses functions from cgihtml.lib					    */
/*       											            */
/*                                                              */
/* Copyright 1995,1996 David Weaver                				*/
/*                                                              */
/****************************************************************/

#ifdef WIN32 /* only use in WIN95/NT */

#include <httpext.h>
#include <stdio.h>
#include "string-lib.h"
#include "cgi-lib.h"

/* display an error as the output								*/
void is_html_error( EXTENSION_CONTROL_BLOCK *pECB, char *title, 
				   char *message, char *email ) {

	DWORD dwLen;
	char error_buffer[255];
	char error_title[100];

	if( title == NULL )
		strcpy( error_title, "CGI Error" );
	else
		strcpy( error_title, title );

	sprintf( error_buffer, "<HTML><TITLE>%s</TITLE>\n", error_title );
	strcat( error_buffer, "<H2>" );
	strcat( error_buffer, error_title );
	strcat( error_buffer, "</H3>\n" );
	strcat( error_buffer, message );
	strcat( error_buffer, "<BR>\n" );
	strcat( error_buffer, "If you do not understand this error, please contact " );
	strcat( error_buffer, EMAIL );
	strcat( error_buffer, "." );

	dwLen = strlen( error_buffer );
	pECB->WriteClient( pECB->ConnID, error_buffer, &dwLen, dwLen );
}

char *is_get_GET( EXTENSION_CONTROL_BLOCK *pECB ) {

  char *buffer;

  if ( pECB->lpszQueryString == NULL ) {
    /*is_html_error( pECB, NULL, "QUERY_STRING is NULL.", EMAIL );*/
	return NULL;
  }
  buffer = newstr( pECB->lpszQueryString );
  return buffer;
}

char *is_get_POST( EXTENSION_CONTROL_BLOCK *pECB )
{
	int cbQuery = 0;
	char *buffer;

    buffer = ( char * )malloc( pECB->cbTotalBytes );
    
	strcpy( buffer, ( char * )pECB->lpbData );

	if( (cbQuery = pECB->cbTotalBytes - pECB->cbAvailable) > 0 )
    {
        pECB->ReadClient(pECB->ConnID, 
                        (LPVOID)(buffer + pECB->cbAvailable), 
                        ( unsigned long * )&cbQuery);
    }

    //
    // For POST requests, two terminating characters are added to the 
    // end of the data. Ignore them by placing a null in the string.
    //
    //*(buffer + pECB->cbTotalBytes - 2 ) = '\0';
    	
	return buffer;
}

#ifdef UPLOADING
int is_parse_form_encoded( EXTENSION_CONTROL_BLOCK *pECB, llist* entries)
{
  long content_length;
  entrytype entry;
  node* window;
  FILE *uploadfile;
  char *uploadfname, *tempstr, *boundary;
  char *buffer = ( char * )malloc(sizeof(char) * BUFSIZ + 1);
  char *prevbuf = ( char * )malloc(sizeof(char) + BUFSIZ + 1);
  short isfile,done,start;
  int i,bytesread,prevbytesread,numentries = 0;

  if (pECB->cbTotalBytes != NULL)
    content_length = pECB->cbTotalBytes;
  else
    return 0;
  /* get boundary */
  tempstr = newstr( pECB->lpszContentType );
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
	/*fprintf(stderr,"cgihtml Error: cannot upload file.\n");*/
	is_html_error( pECB, NULL, "Cannot upload file.", EMAIL );
	return 1;
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
  return 0;
}
#endif /* UPLOADING */

/*	fills in the llist structure with cgi varibles				*/
/*	which is needed to be passed to the slipscript				*/
/*	interpreter													*/
/*	llist is defined and used by cgihtml.lib					*/
int is_read_cgi_input( EXTENSION_CONTROL_BLOCK *pECB, llist* entries ) {

	char *input;

	/* check for form upload.  this needs to be first, because the
	 standard way of checking for POST data is inadequate.  If you
	 are uploading a 100 MB file, you are unlikely to have a buffer
	 in memory large enough to store the raw data for parsing.
	 Instead, parse_form_encoded parses stdin directly.

	 In the future, I may modify parse_CGI_encoded so that it also
	 parses POST directly from stdin. */

	if( ( pECB->lpszContentType != NULL ) &&
		( strstr( pECB->lpszContentType, "multipart/form-data" ) != NULL ) ) {
		is_html_error( pECB, NULL, "File upload not implemented yet.", EMAIL );
		return 1; //is_parse_form_encoded( pECB, entries );
	}

	/* get the input */
	if( pECB->lpszMethod == NULL ) {
		/*is_html_error( pECB, NULL, "REQUEST_METHOD is NULL.", EMAIL );*/
		return 0;
	}
	else if( !strcmp( pECB->lpszMethod, "POST" ) )
		input = is_get_POST( pECB );
	else if( !strcmp( pECB->lpszMethod, "GET" ) )
		input = is_get_GET( pECB );
	else { /* error: invalid request method */
		is_html_error( pECB, NULL, "Invalid request method.", EMAIL );
		return 1;
	}
	/* parse the input */
	if( input == NULL ) return 0;
	return parse_CGI_encoded( entries, input);
}


#endif /* WIN32 */