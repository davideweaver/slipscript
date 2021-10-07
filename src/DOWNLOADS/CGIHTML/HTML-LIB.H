/* html-lib.h - header file for html-lib.c
   Eugene Kim, eekim@fas.harvard.edu
   $Id: html-lib.h,v 1.5 1996/08/11 04:22:01 eekim Exp eekim $

   Copyright (C) 1996 Eugene Eric Kim
   All Rights Reserved
*/

void html_header();
void mime_header(char *mime);
void nph_header(char *status);
void show_html_page(char *loc);
void status(char *status);
void pragma(char *msg);
void html_begin(char *title);
void html_end();

/* better to do printf inside of function, or return string? */
void h1(char *header);
void h2(char *header);
void h3(char *header);
void h4(char *header);
void h5(char *header);
void h6(char *header);
void hidden(char *name, char *value);
