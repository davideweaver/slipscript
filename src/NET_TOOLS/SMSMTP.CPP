/****************************************************************/
/* Name: smtp.cpp 		                                        */
/* Vers: 1.0                                                    */
/* Prog: David Weaver, dew118@psu.edu                           */
/*                                                              */
/* Desc: Provide an interface smtp email.						*/
/*                                                              */
/* NOTE: 													    */
/*       											            */
/*                                                              */
/* Copyright 1995,1996 David Weaver                				*/
/*                                                              */
/****************************************************************/

#include <ctype.h>
#include <stdio.h>
#include "tcpstuff.h"
#include "smsmtp.h"


/* checks a string <s> on character codes under 32				*/
/* returns:														*/
/* 		1 if string contains a character code under 32			*/
/*		0 otherwise												*/
checkstring(char *s) {

	while (*s) {
		if (*s<32) return 1;
		s++;
	}
	return 0;
}

/* waits for a smtp status on socket <socket>,					*/
/* returns:														*/
/* 		a smtp status, or -1 if the socket was closed			*/
int waitforstatus(int socket) {

	char recvBuffer[200];
	getline(socket,recvBuffer);
	return atol(recvBuffer);
}

/* constructor for the smtp class								*/
/*	initializes all variables									*/
/*	allocates memory											*/
/* returns:														*/
/*		nothing													*/
smtp::smtp() {

	message=new char[S_MESSAGE_LEN];
	strcpy(receiver,"");
	strcpy(sender,"");
	strcpy(subject,"");
	strcpy(message,"");
	strcpy(cc,"");
	strcpy(mailmachine,"");
	linebreak=STANDARD_LINEBREAK;
}

/* sets the mail machine for the current smtp class				*/
/* instance														*/	
/* returns:														*/
/*		nothing													*/
void smtp::setmailmachine(char *mach) {	

	strncpy(mailmachine,mach,S_MAILMACHINE_LEN);
}

/* function:													*/
/*		int smtp::sendmail()									*/
/* description:													*/
/*		sends the mail											*/
/* returns:														*/
/*		0 if all went well										*/
/*		1 if mail parameters not initialized					*/
/*		2 if smtp server was not there							*/
/*		3 if smtp server not responding							*/
/*		4 if error getting UserName								*/
/*		>10000 if a socket error occured						*/
int smtp::sendmail() {

	int status, err;
	int sent=1;
	if ( receiver[0] == 0 ) return 1;
	if ( checkstring( receiver ) ) return 1;
	if ( checkstring( sender ) ) return 1;
	if ( checkstring( subject ) ) return 1;
	if ( checkstring( mailmachine ) ) return 1;

	err = startwinsockets();
	if( err ) return err;

	s = init_connection( mailmachine, "smtp", &sockname );
	if( s > 10000 ) return s; /* an error has occured */

	do
	{
		char domainname[100];
		char machinename[100];
		DWORD namelen = 99;
		// check if smtp server is there
		if( s < 0 ) break;
		status=waitforstatus( s );
		if( status != 220 ) return 2;
		
		// send helo and wait for response
		getmachinename( machinename );
		sendstring( s, 0, "helo %s\r\n", machinename );

		status = waitforstatus( s );
		if( status != 250 ) return 3;
		
		// send mail from and wait for response
		if( sender[0] == 0 ) {
			char username[100];

			if( err = getdomainname(domainname) ) return err;
			if( !GetUserName( username, &namelen ) ) return 4;

			username[namelen] = 0;
			sprintf( sender, "%s@%s", username, domainname );
		}

		if( err = sendstring( s, 0, "mail from:<%s>\r\n", sender) ) return err;
		
		status=waitforstatus( s );
		if( status != 250 && status != 251 ) return 3;

		// send rcpt to and wait for response		
		if( err = sendstring( s, 0, "rcpt to:<%s>\r\n", receiver ) ) return err;
		status = waitforstatus( s );
		if( status != 250 && status != 251 ) return 3;

		// data sending
		if( err = sendstring( s, 0, "data\r\n" ) ) return err;
		status = waitforstatus( s );
		if( status != 354 ) return 3;

		// make subject and date fields
		
		sendstring( s, 0, "X-Sender: %s\r\n", sender );
		sendstring( s, 0, "Mime-Version: 1.0\r\n" );
		sendstring( s, 0, "Content-Type: text/plain; charset=\"us-ascii\"\r\n" );
		sendstring( s, 0, "To: %s\r\n", receiver);
		sendstring( s, 0, "From: %s\r\n", sender);
		sendstring( s, 0, "Subject: %s\r\n", subject );
		if ( !checkstring( cc ) )
			sendstring( s, 0, "Cc: %s\r\n", cc);
		sendstring( s, 0, "X-Mailer: <smsmtp>\r\n" );

		sendstring( s, 0, "\r\n" );		
		sendstring( s, 0, "%s\r\n", message );
		sendstring( s, 0, ".\r\n" );

		status = waitforstatus( s );
		if( status != 250 ) return 3;

		sendstring( s, 0, "quit\r\n" );
		status = waitforstatus( s );
		sent = 0;
	}
	while( 0 );
	if( s >= 0 ) closesocket( s );
	return sent;		
}

/* sets the subject for the current instance of the smtp class	*/
/* returns:														*/
/*		nothing													*/
void smtp::setsubject( char *sub ) {

	strncpy( subject, sub, S_SUBJECT_LEN );
}

/* sets the receiver for the current instance of the smtp class	*/
/* returns:														*/
/*		nothing													*/
void smtp::setreceiver( char *rec ) {

	strncpy( receiver, rec, S_RECEIVER_LEN );
}

/* sets the sender for the current instance of the smtp class	*/
/* returns:														*/
/*		nothing													*/
void smtp::setsender( char *sen ) {

	strncpy( sender, sen, S_SENDER_LEN );
}

/* sets the copies for the current instance of the smtp class	*/
/* returns:														*/
/*		nothing													*/
void smtp::setcc( char *copy ) {

	strncpy( cc, copy, S_CC_LEN );
}


/* sets the linebreak for the message							*/
/* returns:														*/
/*		nothing													*/
void smtp::setlinebreak( char linebr ) {

	linebreak = linebr;	
}

/* sets the message and replaces the linebreaks by \r\n			*/
/* returns:														*/
/*		nothing													*/
void smtp::setmessage( char *mes ) {

	int pos = 0;
	int linepos = 0;
	int last;
	while( *mes ) {

		if( *mes == linebreak ) {
			message[pos++] = '\r';
			message[pos++] = '\n';			
			linepos = 0;
			mes++;
		}
		else if( *mes == '\r' ) {
			mes++;
		}
		else if( *mes == '\n' ) {
			mes++;			
		}
		else {
			last = message[pos++] = *mes++;
			if( last == '.' && linepos == 0) message[pos++] = '.';
			linepos++;
		}
		if ( pos>S_MESSAGE_LEN - 3 ) break;
		
	}
	message[pos] = 0;
}

/* cleans things up												*/
/* returns:														*/
/*		nothing													*/
smtp::~smtp() {

	delete message;
}

