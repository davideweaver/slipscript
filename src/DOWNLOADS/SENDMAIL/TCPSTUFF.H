int init_connection(char *machine,char *service,struct sockaddr_in *sockname); 	// open service on machine
void getmachinename(char *name);  												// get machine name (with domain)
void getdomainname(char *name);													// get domain name 
int sendstring(int socket,char *name,int options); 		// send string <name> on socket <socket>
														// with options <options>
int sendstring(int socket,int options,char *format,...);// send string, but with same format
														// as printf
void stopwinsockets();									// stop winsock library
int startwinsockets();									// init winsock library
int getline(int socket, char *buffer);					// get a line from socket <socket>
