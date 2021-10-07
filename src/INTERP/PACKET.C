/* packet.c - packet assembly/disassembly routines */
/*
	Copyright (c) 1994, David Betz and M&T Publishing, Inc.
	All rights reserved
*/

#include <stdio.h>
#include <string.h>
#include "dfile.h"
#include "packet.h"
#include "osint.h"

#define gtlong(adr)		(*((long *)(adr)))
#define stlong(adr,val)		(*((long *)(adr)) = (val))
#define gtshort(adr)		(*((short *)(adr)))
#define stshort(adr,val)	(*((short *)(adr)) = (val))

/* prototypes */
static void htop_long(long val,char *buf);
static long ptoh_long(char *buf);
static void htop_short(short val,char *buf);
static short ptoh_short(char *buf);

/* htop_long - convert a long value from host to packet format */
static void htop_long(long val,char *buf)
{
    stlong(buf,htonl(val));
}

/* ptoh_long - convert a long value from packet to host format */
static long ptoh_long(char *buf)
{
    return (ntohl(gtlong(buf)));
}

/* htop_short - convert a short value from host to packet format */
static void htop_short(short val,char *buf)
{
    stshort(buf,htons(val));
}

/* ptoh_short - convert a short value from packet to host format */
static short ptoh_short(char *buf)
{
    return (ntohs(gtshort(buf)));
}

/* htop_ientry - convert an 'Index' entry from host to packet format */
void htop_ientry(IENTRY *entry,char *buf)
{
    stlong(&buf[IE_MSGNO],htonl(entry->ie_msgno));
    stlong(&buf[IE_OFFSET],htonl(entry->ie_offset));
    stshort(&buf[IE_FILENO],htons(entry->ie_fileno));
    stshort(&buf[IE_FLAGS],htons(entry->ie_flags));
    stlong(&buf[IE_VERSION],htonl(entry->ie_version));
}

/* ptoh_ientry - convert an 'Index' entry from packet to host format */
void ptoh_ientry(IENTRY *entry,char *buf)
{
    entry->ie_msgno = ntohl(gtlong(&buf[IE_MSGNO]));
    entry->ie_offset = ntohl(gtlong(&buf[IE_OFFSET]));
    entry->ie_fileno = ntohs(gtshort(&buf[IE_FILENO]));
    entry->ie_flags = ntohs(gtshort(&buf[IE_FLAGS]));
    entry->ie_version = ntohl(gtlong(&buf[IE_VERSION]));
}

/* htop_dheader - convert a 'Data' header from host to packet format */
void htop_dheader(DHEADER *hdr,char *buf)
{
    stlong(&buf[DH_MSGNO],htonl(hdr->dh_msgno));
    stlong(&buf[DH_LENGTH],htonl(hdr->dh_length));
    stlong(&buf[DH_OFFSET],htonl(hdr->dh_offset));
    stshort(&buf[DH_FILENO],htons(hdr->dh_fileno));
    stshort(&buf[DH_FLAGS],htons(hdr->dh_flags));
    stlong(&buf[DH_VERSION],htonl(hdr->dh_version));
}

/* ptoh_dheader - convert a 'Data' header from packet to host format */
void ptoh_dheader(DHEADER *hdr,char *buf)
{
    hdr->dh_msgno = ntohl(gtlong(&buf[DH_MSGNO]));
    hdr->dh_length = ntohl(gtlong(&buf[DH_LENGTH]));
    hdr->dh_offset = ntohl(gtlong(&buf[DH_OFFSET]));
    hdr->dh_fileno = ntohs(gtshort(&buf[DH_FILENO]));
    hdr->dh_flags = ntohs(gtshort(&buf[DH_FLAGS]));
    hdr->dh_version = ntohl(gtlong(&buf[DH_VERSION]));
}
