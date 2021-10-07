/* boberr.h - error codes */

#ifndef __BOBERR_H__
#define __BOBERR_H__

#include "objects.h"

/* error codes */
#define ecExit			0
#define ecInsufficientMemory	1
#define ecStackOverflow		2
#define ecTooManyArguments	3
#define ecTooFewArguments	4
#define ecTypeError		5
#define ecUnboundVariable	6
#define ecIndexOutOfBounds	7
#define ecNoMethod		8
#define ecBadOpcode		9
#define ecTooManyLiterals	10
#define ecTooMuchCode		11
#define ecStoreIntoConstant	12
#define ecSyntaxError		13

/* prototypes */
char * EXPORT GetErrorText(int code);

#endif
