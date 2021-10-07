// constants......

#define STANDARD_LINEBREAK	10
#define S_SENDER_LEN		100
#define S_RECEIVER_LEN		100
#define S_SUBJECT_LEN		100
#define S_MAILMACHINE_LEN	200
#define S_MESSAGE_LEN		10000

// class definition
// smtp class
// sends messages to a smtp-server

// function:
// 		smtp::smtp()
// description:
//		constructor for the smtp class
//		initializes all variables
//		allocates memory
// returns:
//		nothing

// function:
//		void smtp::setmailmachine(char *mach)
// description:
//		sets the mail machine for the current smtp class instance
// returns:
//		nothing

// function:
//		int smtp::sendmail()
// description:
//		sends the mail 
// returns:
//		1 if all went well
//		0 if mail was not correctly sent

// function:
//		void smtp::setsubject()
// description:
//		sets the subject for the current instance of the smtp class
// returns:
//		nothing

// function:
//		void smtp::setreceiver()
// description:
//		sets the receiver for the current instance of the smtp class
// returns:
//		nothing

// function:
//		void smtp::setsender()
// description:
//		sets the sender for the current instance of the smtp class
// returns:
//		nothing

// function:
//		void smtp::setlinebreak()
// description:
//		sets the linebreak for the message
// returns:
//		nothing

// function:
//		void smtp::setmessage()
// description:
//		sets the message and replaces the linebreaks by \r\n
// returns:
//		nothing

// function:
//		smtp::~smtp()
// description
// 		cleans things up
// returns:
//		nothing


class smtp
{
	int s;
	int linebreak;
	struct sockaddr_in sockname;
	char sender[S_SENDER_LEN+1];
	char receiver[S_RECEIVER_LEN+1];
	char subject[S_SUBJECT_LEN+1];
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
	void setmailmachine(char *mach);
	~smtp();
	
};
