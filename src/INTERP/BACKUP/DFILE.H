/* dfile.h - file structure definitions */
/*
	Copyright (c) 1994, David Betz and M&T Publishing, Inc.
	All rights reserved
*/

#ifndef __DFILE_H__
#define __DFILE_H__

/* conference file open modes */
#define CFM_READ	1
#define CFM_UPDATE	2

/* 'Index' file entry structure */
typedef struct ientry {
    unsigned long ie_msgno;	/* message number */
    unsigned long ie_offset;	/* offset into data file */
    unsigned short ie_fileno;	/* data file number */
    unsigned short ie_flags;	/* flags */
    unsigned long ie_version;	/* version number */
} IENTRY;

/* 'Data' file header structure */
typedef struct dheader {
    unsigned long dh_msgno;	/* message number */
    unsigned long dh_length;	/* length of text */
    unsigned long dh_offset;	/* offset into data file of previous version */
    unsigned short dh_fileno;	/* data file number of previous version */
    unsigned short dh_flags;	/* flags */
    unsigned long dh_version;	/* version number */
} DHEADER;

/* null message number */
#define NULL_MSGNO	((unsigned long)-1)

/* message flags (in both ie_flags and dh_flags) */
#define MF_DELETED	1

#define PSIZE	100	/* maximum path length */
#define DSIZE	(512*K)	/* maximum data file size */
#define BSIZE	512	/* message text buffer size */
#define K	1024L

/* open conference file structure */
typedef struct cfile {
    char cf_path[PSIZE];	/* path to conference directory */
    int cf_mode;		/* open mode */
    FILE *cf_ifp;		/* index file pointer */
    FILE *cf_dfp;		/* most recent data file pointer */
    int cf_fileno;		/* most recent data file number */
} CFILE;

/* conference cursor structure */
typedef struct ccursor {
    CFILE *cc_cfile;		/* conference file structure */
    long cc_ioffset;		/* offset to ientry in index file */
    IENTRY cc_ientry;		/* index entry */
} CCURSOR;

/* open message structure */
typedef struct cmessage {
    CFILE *cm_cfile;		/* conference file structure */
    long cm_ioffset;		/* offset to ientry in index file */
    IENTRY cm_ientry;		/* index entry */
    DHEADER cm_dheader;		/* message header */
    long cm_length;		/* length remaining or current length */
    FILE *cm_dfp;		/* data file pointer */
    int cm_flags;		/* flags */
} CMESSAGE;

/* conference message flags */
#define CMF_LOCKED	1	/* database locked */
#define CMF_CREATE	2	/* message open for creating */
#define CMF_UPDATE	4	/* message open for updating */

int CreateConference(char *path);
CFILE *OpenConfDatabase(char *path,int mode);
int CloseConfDatabase(CFILE *cfile);
CMESSAGE *OpenConfMessage(CFILE *cfile,long msgno,long version);
void CloseConfMessage(CMESSAGE *cmsg);
int ConfGetC(CMESSAGE *cmsg);
int ConfPutC(int ch,CMESSAGE *cmsg);
CMESSAGE *CreateConfMessage(CFILE *cfile);
CMESSAGE *UpdateConfMessage(CFILE *cfile,long msgno);
int DeleteConfMessage(CFILE *cfile,long msgno);
long GetMessageCount(CFILE *cfile);
CCURSOR *CreateConfCursor(CFILE *cfile,long msgno);
int CloseConfCursor(CCURSOR *cursor);
int SetCursorPosition(CCURSOR *cursor,long msgno);
long GetNextMessage(CCURSOR *cursor);
long GetPrevMessage(CCURSOR *cursor);
CMESSAGE *OpenCurrentMessage(CCURSOR *cursor,long version);

#endif
