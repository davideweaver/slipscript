/* include file for smsmtp.cpp   							*/
/* smsmtp.h - Copyright 1995,1996 David Weaver	    		*/

/* NOTE - wsock32.lib must be included in your list of		*/
/*		  library files for your program to link.			*/

#ifndef SMSMTP_H_
#define SMSMTP_H_

#include <winsock.h>

#define STANDARD_LINEBREAK	10
#define S_SENDER_LEN		100
#define S_RECEIVER_LEN		100
#define S_CC_LEN			100
#define S_SUBJECT_LEN		100
#define S_MAILMACHINE_LEN	200
#define S_MESSAGE_LEN		10000


class smtp
{
	int s;
	int linebreak;
	struct sockaddr_in sockname;
	char sender[S_SENDER_LEN+1];
	char receiver[S_RECEIVER_LEN+1];
	char subject[S_SUBJECT_LEN+1];
	char cc[S_CC_LEN+1];
	char mailmachine[S_MAILMACHINE_LEN+1];
	char *message;
	
	public:
	smtp();
	void setlinebreak(char linebreak);
	int sendmail();
	void setmessage(char *mes);
	void setsubject(char *sub);
	void setreceiver(char *rec);
	void setsender(char *sen);
	void setcc(char *copy);
	void setmailmachine(char *mach);
	~smtp();	
};

#endif