/*************************************************************************/
/* Name: ini_file.c 		                                             */
/* Vers: 1.0                                                             */
/* Prog: David Weaver, dew118@psu.edu                                 	 */
/*                                                                       */
/* Desc: Provide an interface to an initialization file for certain      */
/*       programs.                                                       */
/*                                                                       */
/* NOTE: 														         */
/*       							                                     */
/*                                                                       */
/* Copyright 1995,1996 David Weaver                						 */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>      /* For optional main section */  
#include <string.h>
#include <stdlib.h>

#include "ini_file.h"
                                     
/* loadIniFile() loads the ini_data structure with ini file data */
/* send a preloaded and 0 terminated ini_data structure		     */ 
/* returns 0 for success or 1 for memory alloc error             */ 
/* or 2 for not found ini file or 3 for invalid ini file		 */
int loadIniFile(char *fname, struct ini_data *id) 
{
	static int i, differ;    
	static FILE *fp; 
	static char line[300],*name,*value;
	static char *s,*v,*t;
	static struct ini_data *x;
	
	if((fp = fopen(fname, "r")) == NULL)
		return 2;
		
	while(fgets(line,300,fp) != NULL) {
		if(line[0] == '#') continue; 
		if((name = strtok(line,"=")) != NULL) { 
			if((value = strtok(NULL,"\n")) == NULL) return 3;
	    	/* loop thru expected values to find a match */
			for (x=id;x->name[0];x++) {
				differ = 0;
				t=x->name;
				for (s=name;*s && *t;s++,t++) {
					if ( toupper(*s)!=toupper(*t) ) {
						differ=1;
						break;
					}
				}
                /* Use if not different, equals found, and end of expected */
				if ( !differ && !*s && !*t ) {
					if ( (x->value=(char *)malloc(strlen(value))) == NULL )
						return 1;
					strcpy(x->value,value);
					x->found=1;
					break;
				}
			}
		}
	}
	return 0;
}				

