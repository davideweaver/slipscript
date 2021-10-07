/* bobodb.h - bob object database definitions */

#ifndef __BOBODB_H__
#define __BOBODB_H__

#include "streams.h"
#include "dfile.h"

/* object stream structure */
typedef struct {
  StreamIODispatch *d;
  CMESSAGE *cmsg;
} ObjectStream;

/* prototypes */
Stream *CreateObjectStream(CMESSAGE *cmsg);

#endif