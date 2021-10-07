/* packet.h - definitions for packet.c */

#ifndef __PACKET_H__
#define __PACKET_H__

/* data type sizes */
#define RQSSIZE		sizeof(short)
#define RQLSIZE		sizeof(long)

/* 'Index' file entry offsets */
#define IE_MSGNO	0			/* message number */
#define IE_OFFSET	(IE_MSGNO+RQLSIZE)	/* offset into data file */
#define IE_FILENO	(IE_OFFSET+RQLSIZE)	/* data file number */
#define IE_FLAGS	(IE_FILENO+RQSSIZE)	/* flags */
#define IE_VERSION	(IE_FLAGS+RQSSIZE)	/* version number */
#define _IESIZE		(IE_VERSION+RQLSIZE)

/* 'Data' file header offsets */
#define DH_MSGNO	0			/* message number */
#define DH_LENGTH	(DH_MSGNO+RQLSIZE)	/* text length */
#define DH_OFFSET	(DH_LENGTH+RQLSIZE)	/* offset of previous version */
#define DH_FILENO	(DH_OFFSET+RQLSIZE)	/* fileno of previous version */
#define DH_FLAGS	(DH_FILENO+RQSSIZE)	/* flags */
#define DH_VERSION	(DH_FLAGS+RQSSIZE)	/* version number */
#define _DHSIZE		(DH_VERSION+RQLSIZE)

void htop_ientry(IENTRY *entry,char *buf);
void ptoh_ientry(IENTRY *entry,char *buf);
void htop_dheader(DHEADER *hdr,char *buf);
void ptoh_dheader(DHEADER *hdr,char *buf);

#endif