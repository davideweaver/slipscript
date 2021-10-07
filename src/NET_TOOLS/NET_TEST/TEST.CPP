/* test file for net_tools library */

#include <iostream.h>
#include <net_tools.h>

#include <stdio.h>


void main() {

	cout << "testing net_tools library..." << endl;

	smtp mail;
	mail.setsender( "dew118@psu.edu" );
	mail.setreceiver( "davew@to0l.ie.psu.edu" );
	mail.setmailmachine( "psu.edu" );
	mail.setsubject( "testing net_tools.lib" );
	mail.setmessage( "If you are reading this, I guess it worked.\nDave Weaver" );
	int ret = mail.sendmail();
	cout << "sendmail returned: " << ret << endl;

	char name[255];
	getmachinename( name );
	cout << "\nyour machine name is:" << endl;
	cout << name << endl;

	cout << "Press enter to exit." << endl;
	getchar();

}