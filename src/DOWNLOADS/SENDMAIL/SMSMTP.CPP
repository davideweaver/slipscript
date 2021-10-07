#include <ctype.h>
#include <stdio.h>
#include <winsock.h>
#include <windows.h>
#include "tcpstuff.h"
#include "smsmtp.h"


// function:
// 		checkstring(char *s)
// description:
//		checks a string <s> on character codes under 32
// returns:
// 		1 if string contains a character code under 32
//		0 otherwise

checkstring(char *s)
{
	while (*s)
	{
		if (*s<32) return 1;
		s++;
	}
	return 0;
}

// function:
// 		waitforstatus(int socket)
// description:
// 		waits for a smtp status on socket <socket>,
// returns:
// 		a smtp status, or -1 if the socket was closed

int waitforstatus(int socket)
{
	char recvBuffer[200];
	getline(socket,recvBuffer);
	return atol(recvBuffer);
}

// function:
// 		smtp::smtp()
// description:
//		constructor for the smtp class
//		initializes all variables
//		allocates memory
// returns:
//		nothing

smtp::smtp()
{
	message=new char[S_MESSAGE_LEN];
	strcpy(receiver,"");
	strcpy(sender,"");
	strcpy(subject,"");
	strcpy(message,"");
	strcpy(mailmachine,"");
	linebreak=STANDARD_LINEBREAK;
}

// function:
//		void smtp::setmailmachine(char *mach)
// description:
//		sets the mail machine for the current smtp class instance
// returns:
//		nothing

void smtp::setmailmachine(char *mach)
{	
	strncpy(mailmachine,mach,S_MAILMACHINE_LEN);
}

// function:
//		int smtp::sendmail()
// description:
//		sends the mail 
// returns:
//		1 if all went well
//		0 if mail was not correctly sent

int smtp::sendmail()
{
	int status;
	int sent=0;
	if (receiver[0]==0) return 0;
	if (checkstring(receiver)) return 0;
	if (checkstring(sender)) return 0;
	if (checkstring(subject)) return 0;
	if (checkstring(mailmachine)) return 0;

	s=init_connection(mailmachine,"smtp",&sockname);

	do
	{
		char domainname[100];
		char machinename[100];
		DWORD namelen=99;
		// check if smtp server is there
		if (s<0) break;
		status=waitforstatus(s);
		if (status!=220) 
			break;
		
		// send helo and wait for response
		getmachinename(machinename);
		sendstring(s,0,"helo %s\r\n",machinename);

		status=waitforstatus(s);
		if (status!=250) 
			break;
		
		// send mail from and wait for response
		if (sender[0]==0)
		{
			char username[100];
			getdomainname(domainname);
			if (!GetUserName(username,&namelen))
				break;
			username[namelen]=0;
			//sendstring(s,0,"mail from:<%s@%s>\r\n",username,domainname);
			sprintf(sender,"%s@%s",username,domainname);

		}
		sendstring(s,0,"mail from:<%s>\r\n",sender);
		
		status=waitforstatus(s);
		if (status!=250 && status!=251)
			break;

		// send rcpt to and wait for response		
		sendstring(s,0,"rcpt to:<%s>\r\n",receiver);
		status=waitforstatus(s);
		if (status!=250 && status!=251)
			break;

		// data sending
		sendstring(s,0,"data\r\n");
		status=waitforstatus(s);
		if (status!=354)
			break;

		// make subject and date fields
		
		sendstring(s,0,"X-Sender: %s\r\n",sender);
		sendstring(s,0,"Mime-Version: 1.0\r\n");
		sendstring(s,0,"Content-Type: text/plain; charset=\"us-ascii\"\r\n");
		sendstring(s,0,"To: %s\r\n",receiver);
		sendstring(s,0,"From: %s\r\n",sender);

		sendstring(s,0,"Subject: %s\r\n",subject);
		sendstring(s,0,"X-Mailer: <smsmtp>\r\n");

		sendstring(s,0,"\r\n");		
		sendstring(s,0,"%s\r\n",message);
		sendstring(s,0,".\r\n");
		status=waitforstatus(s);
		if (status!=250) 
			break;
		sendstring(s,0,"quit\r\n");
		status=waitforstatus(s);
		sent=1;
	}
	while (0);
	if (s>=0) closesocket(s);
	return sent;		
}

// function:
//		void smtp::setsubject()
// description:
//		sets the subject for the current instance of the smtp class
// returns:
//		nothing

void smtp::setsubject(char *sub)
{
	strncpy(subject,sub,S_SUBJECT_LEN);
}

// function:
//		void smtp::setreceiver()
// description:
//		sets the receiver for the current instance of the smtp class
// returns:
//		nothing

void smtp::setreceiver(char *rec)
{
	strncpy(receiver,rec,S_RECEIVER_LEN);
}

// function:
//		void smtp::setsender()
// description:
//		sets the sender for the current instance of the smtp class
// returns:
//		nothing

void smtp::setsender(char *sen)
{
	strncpy(sender,sen,S_SENDER_LEN);
}


// function:
//		void smtp::setlinebreak()
// description:
//		sets the linebreak for the message
// returns:
//		nothing

void smtp::setlinebreak(char linebr)
{
	linebreak=linebr;	

}

// function:
//		void smtp::setmessage()
// description:
//		sets the message and replaces the linebreaks by \r\n
// returns:
//		nothing

void smtp::setmessage(char *mes)
{
	int pos=0;
	int linepos=0;
	int last;
	while (*mes)
	{
		if (*mes==linebreak)
		{
			
			message[pos++]='\r';
			message[pos++]='\n';			
			linepos=0;
			mes++;
		}
		else
		if (*mes=='\r')
		{
			mes++;
		}
		else 
		if (*mes=='\n')
		{
			mes++;			
		}
		else
		{
			last=message[pos++]=*mes++;
			if (last=='.' && linepos==0) message[pos++]='.';
			linepos++;
		}
		if (pos>S_MESSAGE_LEN-3) break;
		
	}
	message[pos]=0;
}

// function:
//		smtp::~smtp()
// description
// 		cleans things up
// returns:
//		nothing
smtp::~smtp()
{
	delete message;
}
