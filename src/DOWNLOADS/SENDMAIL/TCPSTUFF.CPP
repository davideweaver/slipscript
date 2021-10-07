#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <windows.h>
#include "tcpstuff.h"

// NOT documented ......

#define forever for (;;)

int startwinsockets()
{
	int status;
	WSADATA WSAData;
	char szTemp[80];
	status=WSAStartup(MAKEWORD(1,1), &WSAData);
	if (status!=0) 
	{
		sprintf(szTemp, "%d is the err", status);

		return -1;
	}
	return 0;
}
void stopwinsockets()
{
	WSACleanup();
}


int init_connection(char *machine,char *service,
struct sockaddr_in *sockname)
{
	int s,nl,rv,repeat_count=0;
	struct hostent *hostname;
	struct servent *serv_rec;

	do 
	{
		memset(sockname,0,sizeof(struct sockaddr));
		s=socket(AF_INET,SOCK_STREAM,0);
		if (s<0) 
			return -1;

		sockname->sin_family=PF_INET;		
		serv_rec=getservbyname(service,"tcp");
		sockname->sin_port=serv_rec->s_port;

		hostname=gethostbyname(machine);

		if (hostname==NULL)
		{
			closesocket(s);
			return -2;
		}
		memcpy((void *) &(sockname->sin_addr),(void*) hostname->h_addr,hostname->h_length);
		nl=sizeof(*sockname);
		rv=connect(s,(struct sockaddr *) sockname,nl);
		if (rv!=0)
		{
			printf("Attempting to connect\n");
			closesocket(s);
			Sleep(1000);
			repeat_count++;
			if (repeat_count>1) 
				return -3;
		}

	} while (rv!=0);

	return s;
}
void getmachinename(char *name)
{
	char temp[200];
	struct hostent *host;

	// get host name
	gethostname(temp,199);
	host=gethostbyname(temp);
	strcpy(name,host->h_name);

}
void getdomainname(char *name)
{
	char temp[200];
	
	int pos;

	// get host information
	
	getmachinename(temp);
	pos=0;
	while(temp[pos]!='.') pos++;
	pos++;
 	strcpy(name,temp+pos);
		
}
int sendstring(int socket,int options,char *format,...)
{
	char temp[26000]; // !#@#%$^%*& (aargh, many space on stack)
	va_list argptr;
	va_start(argptr,format);
	vsprintf(temp,format,argptr);
	va_end(argptr);
	return sendstring(socket,temp,options);
}
int sendstring(int socket,char *name,int options)
{
	return send(socket,name,strlen(name),options);
}

#define tempLen 50
int getline(int socket, char *buffer)
{
	char temp[tempLen+1];
	int c,len,linelen=0;
	buffer[0]=0;
	forever
	{
		len=recv(socket,temp,tempLen,MSG_PEEK);
		
		if (len==0) return -1;
		
		if (len>0) 
		{
			
			
			c=0;
			while(((temp[c]!='\n') && c<len)) c++;
			recv(socket,temp,c,0);
			linelen=c;
			for (c=0;c<linelen;c++)
			{
				if (temp[c]=='\r') 
				{
					memmove(temp+c,temp+c+1,linelen-(c+1));
					linelen--;
					
				}
			}
			temp[linelen]=0;
			strcat(buffer,temp);
			if (c!=len)
			{
				recv(socket,temp,1,0);
				return 0;
			}
		}
	}
}
