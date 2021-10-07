/* include file for net_tools.lib							*/
/* net_tools.h - Copyright 1995,1996 David Weaver			*/

/* NOTE - wsock32.lib must be included in your list of		*/
/*		  library files for your program to link.			*/


#ifdef WIN32

#include "../src/net_tools/smsmtp.h"
#include "../src/net_tools/tcpstuff.h"

#else

/* UNIX sendmail functions */

#endif /* WIN32 */
