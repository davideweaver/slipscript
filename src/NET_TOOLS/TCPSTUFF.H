/* include file for tcpstuff.cpp   							*/
/* tcpstuff.h - Copyright 1995,1996 David Weaver	    	*/

/* NOTE - wsock32.lib must be included in your list of		*/
/*		  library files for your program to link.			*/

#ifndef TCPSTUFF_H_
#define TCPSTUFF_H_

int init_connection(char *machine,char *service,struct sockaddr_in *sockname); 	
int getmachinename(char *name);  												
int getdomainname(char *name);													
int sendstring(int socket,char *name,int options); 		
														
int sendstring(int socket,int options,char *format,...);
														
void stopwinsockets();									
int startwinsockets();									
int getline(int socket, char *buffer);					

#endif