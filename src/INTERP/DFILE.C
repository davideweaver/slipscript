/* dfile.c - message database routines */
/*
	Copyright (c) 1995, David Betz and M&T Publishing, Inc.
	All rights reserved
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"
#include "dfile.h"
#include "packet.h"
#include "osint.h"

/* forward declarations */
static long GetActiveCount(CFILE *cfile);
static FILE *OpenConfDataFile(CFILE *cfile,int filen);
static void CloseConfDataFile(CFILE *cfile,int filen,FILE *dfp);
static CMESSAGE *FinishOpeningMessage(CMESSAGE *cmsg,long version);
static int SetupToWriteMessageData(CMESSAGE *cmsg);
static FILE *CreateNewDataFile(char *fname);
static int FinishCreatingMessage(CMESSAGE *cmsg);
static int FinishUpdatingMessage(CMESSAGE *cmsg);
static int RewriteMessageHeader(CMESSAGE *cmsg);
static int GetMsgNo(CFILE *cfile,unsigned long *pmsgno,unsigned short *pfileno);
static int GetIndexEntry(CFILE *cfile,unsigned long msgno,IENTRY *entry,long *poffset);
static FILE *GetMessageHeader(CFILE *cfile,int filen,long offset,DHEADER *hdr);
static int FindIndexEntry(CFILE *cfile,unsigned long msgno,IENTRY *entry,long *poffset);
static int SearchBlock(IENTRY *entries,int nentries,unsigned long msgno,IENTRY *entry,long *poffset);
static int ReadIEntries(FILE *fp,IENTRY *entries,int nentries);
static void ClearStruct(char *addr,size_t size);
static int ReadIEntry(FILE *fp,IENTRY *entry);
static int WriteIEntry(FILE *fp,IENTRY *entry);
static int ReadDHeader(FILE *fp,DHEADER *hdr);
static int WriteDHeader(FILE *fp,DHEADER *hdr);
static char *ConfPathIndex(char *path);
static char *ConfPathData(char *path,int filen);

/* CreateConference - create a new conference */
int CreateConference(char *path)
{
    IENTRY ientry;
    FILE *ifp;

    /* open the index file */
    if ((ifp = fopen(ConfPathIndex(path),BINARY_WRITE)) == NULL)
	return -1;

    /* initialize the index entry */
    ClearStruct((char *)&ientry,sizeof(IENTRY));

    /* write the initial index file entry */
    if (!WriteIEntry(ifp,&ientry)) {
	fclose(ifp);
	return -2;
    }

    /* return successfully */
    fclose(ifp);
    return 0;
}

/* OpenConfDatabase - open a conference database file */
CFILE *OpenConfDatabase(char *path,int mode)
{
    CFILE *cfile;
    char *omode;

    /* allocate space for the new conference database structure */
    if ((cfile = (CFILE *)malloc(sizeof(CFILE))) == NULL)
	return NULL;

    /* initialize the conference file structure */
    strcpy(cfile->cf_path,path);
    cfile->cf_mode = mode;
    cfile->cf_dfp = NULL;

    /* open the message file */
    omode = (cfile->cf_mode == CFM_READ ? BINARY_READ : BINARY_UPDATE);
    if ((cfile->cf_ifp = fopen(ConfPathIndex(cfile->cf_path),omode)) == NULL) {
	free(cfile);
	return NULL;
    }

    /* return the handle to the open conference file */
    return cfile;
}

/* CloseConfDatabase - close a conference database */
int CloseConfDatabase(CFILE *cfile)
{
    int result = 0,sts;

    /* close the message file */
    if (cfile->cf_ifp != NULL)
	if ((sts = fclose(cfile->cf_ifp)) != 0)
	    result = sts;

    /* close the current data file */
    if (cfile->cf_dfp != NULL)
	if ((sts = fclose(cfile->cf_dfp)) != 0)
	    result = sts;

    /* free the conference file structure */
    free(cfile);
    return result;
}

/* OpenConfMessage - open a conference message */
CMESSAGE *OpenConfMessage(CFILE *cfile,long msgno,long version)
{
    CMESSAGE *cmsg;

    /* allocate space for the new open message structure */
    if ((cmsg = (CMESSAGE *)malloc(sizeof(CMESSAGE))) == NULL)
	return NULL;
    cmsg->cm_cfile = cfile;

    /* get index entry */
    if (GetIndexEntry(cfile,msgno,&cmsg->cm_ientry,&cmsg->cm_ioffset) != 0) {
	free ((void *)cmsg);
	return NULL;
    }
    
    /* open the correct version */
    return FinishOpeningMessage(cmsg,version);
}

/* FinishOpeningMessage - finish opening a message */
static CMESSAGE *FinishOpeningMessage(CMESSAGE *cmsg,long version)
{
    CFILE *cfile = cmsg->cm_cfile;
	
    /* read the header and setup to read the message */
    cmsg->cm_dfp = GetMessageHeader(cfile,
				    cmsg->cm_ientry.ie_fileno,
				    cmsg->cm_ientry.ie_offset,
				    &cmsg->cm_dheader);
    if (cmsg->cm_dfp == NULL) {
	free((void *)cmsg);
	return NULL;
    }

    /* check the version number */
    if (version < 0 && (version += cmsg->cm_dheader.dh_version) < 0) {
	CloseConfDataFile(cfile,cmsg->cm_ientry.ie_fileno,cmsg->cm_dfp);
	free((void *)cmsg);
	return NULL;
    }
    if (version != 0 && version != (long)cmsg->cm_dheader.dh_version) {
	while (version < (long)cmsg->cm_dheader.dh_version
	    && cmsg->cm_dheader.dh_offset != 0) {
	    CloseConfDataFile(cfile,cmsg->cm_ientry.ie_fileno,cmsg->cm_dfp);
	    cmsg->cm_dfp = GetMessageHeader(cfile,
					    cmsg->cm_dheader.dh_fileno,
					    cmsg->cm_dheader.dh_offset,
					    &cmsg->cm_dheader);
	    if (cmsg->cm_dfp == NULL) {
		free((void *)cmsg);
		return NULL;
	    }
	}
	if (version != (long)cmsg->cm_dheader.dh_version) {
	    CloseConfDataFile(cfile,cmsg->cm_ientry.ie_fileno,cmsg->cm_dfp);
	    free((void *)cmsg);
	    return NULL;
	}
    }
    
    /* initialize the open message structure */
    cmsg->cm_length = cmsg->cm_dheader.dh_length;
    cmsg->cm_flags = 0;

    /* return successfully */
    return cmsg;
}

/* CloseConfMessage - close a conference message */
void CloseConfMessage(CMESSAGE *cmsg)
{
    if (cmsg->cm_flags & CMF_CREATE)
	FinishCreatingMessage(cmsg);
    else if (cmsg->cm_flags & CMF_UPDATE)
	FinishUpdatingMessage(cmsg);
    else {
	if (cmsg->cm_cfile->cf_mode == CFM_UPDATE)
	    fflush(cmsg->cm_dfp);
	CloseConfDataFile(cmsg->cm_cfile,cmsg->cm_ientry.ie_fileno,cmsg->cm_dfp);
    }
    if (cmsg->cm_flags & CMF_LOCKED)
	osunlock(cmsg->cm_cfile->cf_ifp);
    free((void *)cmsg);
}

/* ConfGetC - get the next character from a conference message */
int ConfGetC(CMESSAGE *cmsg)
{
    if (cmsg->cm_length > 0L) {
	--cmsg->cm_length;
	return getc(cmsg->cm_dfp);
    }
    return EOF;
}

/* CreateConfMessage - create a conference message */
CMESSAGE *CreateConfMessage(CFILE *cfile)
{
    CMESSAGE *cmsg;

    /* allocate space for the new open message structure */
    if ((cmsg = (CMESSAGE *)malloc(sizeof(CMESSAGE))) == NULL)
	return NULL;

    /* lock the conference */
    if (!oslock(cfile->cf_ifp)) {
	free((void *)cmsg);
	return NULL;
    }

    /* initialize the open message structure */
    cmsg->cm_cfile = cfile;
    cmsg->cm_flags = CMF_CREATE | CMF_LOCKED;
    cmsg->cm_length = 0;

    /* initialize the new index file entry */
    ClearStruct((char *)&cmsg->cm_ientry,sizeof(IENTRY));
    cmsg->cm_ientry.ie_version = 1;
    cmsg->cm_ientry.ie_flags = 0;

    /* get the next file number and message number */
    if (GetMsgNo(cfile,&cmsg->cm_ientry.ie_msgno,&cmsg->cm_ientry.ie_fileno) != 0) {
	osunlock(cfile->cf_ifp);
	free((void *)cmsg);
	return NULL;
    }

    /* initialize the message header */
    ClearStruct((char *)&cmsg->cm_dheader,sizeof(DHEADER));
    cmsg->cm_dheader.dh_msgno = cmsg->cm_ientry.ie_msgno;
    cmsg->cm_dheader.dh_fileno = 0;
    cmsg->cm_dheader.dh_offset = 0;
    cmsg->cm_dheader.dh_version = 1;
    cmsg->cm_dheader.dh_length = 0;
    cmsg->cm_dheader.dh_flags = 0;
    
    /* setup to write the data */
    if (SetupToWriteMessageData(cmsg) != 0) {
    	osunlock(cfile->cf_ifp);
	free((void *)cmsg);
	return NULL;
    }
    
    /* return successfully */
    return cmsg;
}

/* UpdateConfMessage - update a conference message */
CMESSAGE *UpdateConfMessage(CFILE *cfile,long msgno)
{
    CMESSAGE *cmsg;

    /* allocate space for the new open message structure */
    if ((cmsg = (CMESSAGE *)malloc(sizeof(CMESSAGE))) == NULL)
	return NULL;

    /* lock the conference */
    if (!oslock(cfile->cf_ifp)) {
	free((void *)cmsg);
	return NULL;
    }

    /* initialize the open message structure */
    cmsg->cm_cfile = cfile;
    cmsg->cm_flags = CMF_UPDATE | CMF_LOCKED;
    cmsg->cm_length = 0;

    /* get the data file number and offset */
    if (GetIndexEntry(cfile,msgno,&cmsg->cm_ientry,&cmsg->cm_ioffset) != 0) {
	osunlock(cfile->cf_ifp);
	free((void *)cmsg);
	return NULL;
    }
    ++cmsg->cm_ientry.ie_version;

    /* initialize the message header */
    cmsg->cm_dheader.dh_fileno = cmsg->cm_ientry.ie_fileno;
    cmsg->cm_dheader.dh_offset = cmsg->cm_ientry.ie_offset;
    cmsg->cm_dheader.dh_version = cmsg->cm_ientry.ie_version;
    cmsg->cm_dheader.dh_length = 0;
    cmsg->cm_dheader.dh_flags = 0;

    /* setup to write the data */
    if (SetupToWriteMessageData(cmsg) != 0) {
    	osunlock(cfile->cf_ifp);
	free((void *)cmsg);
	return NULL;
    }
    
    /* return successfully */
    return cmsg;
}

/* SetupToWriteMessageData - setup to write message data for create or update */
static int SetupToWriteMessageData(CMESSAGE *cmsg)
{
    CFILE *cfile = cmsg->cm_cfile;
    char *fname;
    
    /* close the data file if it's open in the cache */
    CloseConfDataFile(cfile,cmsg->cm_ientry.ie_fileno,NULL);

    /* open the data file */
    fname = ConfPathData(cfile->cf_path,cmsg->cm_ientry.ie_fileno);
    if ((cmsg->cm_dfp = fopen(fname,BINARY_UPDATE)) == NULL
    &&  (cmsg->cm_dfp = CreateNewDataFile(fname)) == NULL)
	return -1;

    /* position at the end of the file */
    if (fseek(cmsg->cm_dfp,0,SEEK_END) != 0) {
	fclose(cmsg->cm_dfp);
	cmsg->cm_dfp = NULL;
	return -1;
    }

    /* get the offset to the new message */
    cmsg->cm_ientry.ie_offset = ftell(cmsg->cm_dfp);

    /* write the message header */
    if (!WriteDHeader(cmsg->cm_dfp,&cmsg->cm_dheader)) {
	fclose(cmsg->cm_dfp);
	cmsg->cm_dfp = NULL;
	return -1;
    }
    
    /* return successfully */
    return 0;
}

/* CreateNewDataFile - create and initialize a new data file */
static FILE *CreateNewDataFile(char *fname)
{
    FILE *dfp = fopen(fname,BINARY_CUPDATE);
    long data = 0;
    if (dfp != NULL) {
	if (fwrite(&data,sizeof(data),1,dfp) != 1) { /* so no entry has offset zero */
	    fclose(dfp);
	    return NULL;
	}
    }
    return dfp;
}

/* ConfPutC - write the next character to a conference message */
int ConfPutC(int ch,CMESSAGE *cmsg)
{
    //ch = putc(ch,cmsg->cm_dfp);
    //if (ch >= 0) ++cmsg->cm_length;
    return ch;
}

/* FinishCreatingMessage - finish creating a message */
static int FinishCreatingMessage(CMESSAGE *cmsg)
{
    CFILE *cfile = cmsg->cm_cfile;
    
    /* rewrite the message header to include the length */
    if (RewriteMessageHeader(cmsg) != 0)
	return -1;
	
    /* position to write the new index record */
    if (fseek(cfile->cf_ifp,-(long)sizeof(IENTRY),SEEK_END) != 0)
	return -1;

    /* write the new index file entry */
    if (!WriteIEntry(cfile->cf_ifp,&cmsg->cm_ientry))
	return -1;

    /* write the new end record */
    cmsg->cm_ientry.ie_offset = 0;
    if (!WriteIEntry(cfile->cf_ifp,&cmsg->cm_ientry))
	return -1;

    /* make sure the index file gets updated */
    if (fflush(cfile->cf_ifp) != 0)
	return -1;

    /* return successfully */
    return 0;
}

/* FinishUpdatingMessage - finish updating a message */
static int FinishUpdatingMessage(CMESSAGE *cmsg)
{
    CFILE *cfile = cmsg->cm_cfile;
    
    /* rewrite the message header to include the length */
    if (RewriteMessageHeader(cmsg) != 0)
	return -1;
	
    /* rewrite the index file entry */
    if (fseek(cfile->cf_ifp,cmsg->cm_ioffset,SEEK_SET) != 0
    ||  !WriteIEntry(cfile->cf_ifp,&cmsg->cm_ientry))
	return -1;

    /* make sure the index file gets updated */
    if (fflush(cfile->cf_ifp) != 0)
	return -1;

    /* return successfully */
    return 0;
}

/* RewriteMessageHeader - rewrite the message header (to include the length) */
static int RewriteMessageHeader(CMESSAGE *cmsg)
{
    /* check to see if we need to go to a new data file */
    if (cmsg->cm_ientry.ie_offset + cmsg->cm_length >= DSIZE)
	++cmsg->cm_ientry.ie_fileno;

    /* position to rewrite the header record */
    if (fseek(cmsg->cm_dfp,cmsg->cm_ientry.ie_offset,SEEK_SET) != 0) {
	fclose(cmsg->cm_dfp);
	return -1;
    }

    /* rewrite the message header */
    cmsg->cm_dheader.dh_length = cmsg->cm_length;
    if (!WriteDHeader(cmsg->cm_dfp,&cmsg->cm_dheader)) {
	fclose(cmsg->cm_dfp);
	return -1;
    }

    /* make sure the data file gets updated */
    if (fflush(cmsg->cm_dfp) != 0) {
	fclose(cmsg->cm_dfp);
	return -1;
    }
    
    /* close the data file and return */
    return fclose(cmsg->cm_dfp);
}

/* DeleteConfMessage - delete a conference message */
int DeleteConfMessage(CFILE *cfile,long msgno)
{
    IENTRY entry;
    DHEADER hdr;
    long offset;
    FILE *dfp;
    
    /* get index entry */
    if (GetIndexEntry(cfile,msgno,&entry,&offset) != 0)
	return -1;
	
    /* mark it as deleted and write it back out */
    entry.ie_flags |= MF_DELETED;
    if (fseek(cfile->cf_ifp,offset,SEEK_SET) != 0
    ||  !WriteIEntry(cfile->cf_ifp,&entry))
	return -1;
	
    /* read the header */
    dfp = GetMessageHeader(cfile,entry.ie_fileno,entry.ie_offset,&hdr);
    if (dfp == NULL)
	return -1;

    /* mark it as deleted and write it back out */
    hdr.dh_flags |= MF_DELETED;
    if (fseek(dfp,entry.ie_offset,SEEK_SET) != 0
    ||  !WriteDHeader(dfp,&hdr)) {
	CloseConfDataFile(cfile,entry.ie_fileno,dfp);
	return -1;
    }
    CloseConfDataFile(cfile,entry.ie_fileno,dfp);
    
    /* mark previous versions as deleted */
    while (hdr.dh_offset != 0) {
	dfp = GetMessageHeader(cfile,hdr.dh_fileno,hdr.dh_offset,&hdr);
	if (dfp == NULL)
	    return -1;
	hdr.dh_flags |= MF_DELETED;
	if (fseek(dfp,hdr.dh_offset,SEEK_SET) != 0
	||  !WriteDHeader(dfp,&hdr)) {
	    CloseConfDataFile(cfile,hdr.dh_fileno,dfp);
	    return -1;
	}
	CloseConfDataFile(cfile,hdr.dh_fileno,dfp);
    }
    
    /* return successfully */
    return 0;
}

/* GetMessageCount - get the number of messages in a conference */
long GetMessageCount(CFILE *cfile)
{
    unsigned long msgno;
    unsigned short filen;
    if (GetMsgNo(cfile,&msgno,&filen) != 0)
	return 0;
    return msgno - 1;
}

/* GetActiveCount - get the number of active messages in a conference */
static long GetActiveCount(CFILE *cfile)
{
    long size;
    if ((size = osfilesize(cfile->cf_ifp)) < 0L)
        return 0L;
    return size / sizeof(IENTRY) - 1;
}

/* OpenConfDataFile - open a conference data file */
static FILE *OpenConfDataFile(CFILE *cfile,int filen)
{
    char *omode;
    FILE *dfp;

    /* check for the file already being open */
    if ((dfp = cfile->cf_dfp) != NULL && cfile->cf_fileno == filen)
	cfile->cf_dfp = NULL;
    else {
	omode = (cfile->cf_mode == CFM_READ ? BINARY_READ : BINARY_UPDATE);
	dfp = fopen(ConfPathData(cfile->cf_path,filen),omode);
    }
    return dfp;
}

/* CloseConfDataFile - close a conference data file */
static void CloseConfDataFile(CFILE *cfile,int filen,FILE *dfp)
{
    if (dfp) {
	if (cfile->cf_dfp == NULL) {
	    cfile->cf_fileno = filen;
	    cfile->cf_dfp = dfp;
	}
	else
	    fclose(dfp);
    }
    else if (cfile->cf_dfp && cfile->cf_fileno == filen) {
	fclose(cfile->cf_dfp);
	cfile->cf_dfp = NULL;
    }
}

/* GetMsgNo - get the next message number and file number */
static int GetMsgNo(CFILE *cfile,unsigned long *pmsgno,unsigned short *pfileno)
{
    IENTRY entry;
    if (fseek(cfile->cf_ifp,-(long)sizeof(IENTRY),SEEK_END) != 0
    ||  !ReadIEntry(cfile->cf_ifp,&entry))
	return -1;
    *pmsgno = entry.ie_msgno + 1;
    *pfileno = entry.ie_fileno;
    return 0;
}

/* GetMessageHeader - get a message header and setup to read the message */
static FILE *GetMessageHeader(CFILE *cfile,int filen,long offset,DHEADER *hdr)
{
    FILE *dfp;
    
    /* open the appropriate data file */
    if ((dfp = OpenConfDataFile(cfile,filen)) == NULL)
	return NULL;

    /* read the message header */
    if (fseek(dfp,offset,SEEK_SET) != 0
    ||  !ReadDHeader(dfp,hdr)) {
	CloseConfDataFile(cfile,filen,dfp);
	return NULL;
    }
    
    /* return successfully */
    return dfp;
}

/* GetIndexEntry - get the index entry of a message */
static int GetIndexEntry(CFILE *cfile,unsigned long msgno,IENTRY *pentry,long *poffset)
{
    /* find the index entry for the specified message number */
    if (FindIndexEntry(cfile,msgno,pentry,poffset) != 0
    ||  pentry->ie_msgno != msgno
    ||  (pentry->ie_flags & MF_DELETED) != 0)
	return -1;
    return 0;
}

#define IBLKSIZE	16

/* FindIndexEntry - find the specified index entry or the next higher */
static int FindIndexEntry(CFILE *cfile,unsigned long msgno,IENTRY *entry,long *poffset)
{
    IENTRY entries[IBLKSIZE];
    long left,right,probe;
    int cnt;

    /* initialize */
    left = 0L;
    right = (GetActiveCount(cfile) - 1L) / IBLKSIZE;

    /* do a binary search for the block containing the message number */
    while (left <= right) {
	probe = (left + right) / 2;
	if (fseek(cfile->cf_ifp,probe * IBLKSIZE * sizeof(IENTRY),SEEK_SET) != 0
	||  (cnt = ReadIEntries(cfile->cf_ifp,entries,IBLKSIZE)) == 0)
	    return -1;
	switch (SearchBlock(entries,cnt,msgno,entry,poffset)) {
	case -1:
	    right = probe - 1L;
	    break;
	case 0:
	    *poffset += (long)probe * (long)IBLKSIZE * (long)sizeof(IENTRY);
	    return 0;
	case 1:
	    left = probe + 1L;
	    break;
	}
    }

    /* message number is not in the index file */
    *poffset = (long)left * (long)IBLKSIZE * (long)sizeof(IENTRY);
    return 0;
}

/* SearchBlock - search one block of the index */
static int SearchBlock(IENTRY *entries,int nentries,unsigned long msgno,IENTRY *entry,long *poffset)
{
    int left,right,probe;

    /* initialize */
    left = 0;
    right = nentries - 1;
    entry->ie_msgno = NULL_MSGNO;

    /* do a binary search for the record containing the message number */
    while (left <= right) {
	probe = (left + right) / 2;
	*entry = entries[probe];
	if (msgno < entry->ie_msgno)
	    right = probe - 1;
	else if (msgno > entry->ie_msgno)
	    left = probe + 1;
	else {
	    *poffset = probe * sizeof(IENTRY);
	    return 0;
	}
    }

    /* message number is not in this block */
    *poffset = left * sizeof(IENTRY);
    return right < 0 ? -1 : left >= nentries ? 1 : 0;
}

/* ReadIEntries - read a bunch of IENTRY structures */
static int ReadIEntries(FILE *fp,IENTRY *entries,int nentries)
{
    int cnt;
    for (cnt = 0; cnt < nentries; ++cnt, ++entries)
	if (!ReadIEntry(fp,entries))
	    break;
    return cnt;
}

/* CreateConfCursor - create a conference cursor */
CCURSOR *CreateConfCursor(CFILE *cfile,long msgno)
{
    CCURSOR *cursor;

    /* allocate space for the new cursor structure */
    if ((cursor = (CCURSOR *)malloc(sizeof(CCURSOR))) == NULL)
	return NULL;
    cursor->cc_cfile = cfile;
    cursor->cc_ientry.ie_msgno = NULL_MSGNO;
    
    /* set the initial cursor position */
    if (SetCursorPosition(cursor,msgno) != 0) {
	free((void *)cursor);
	return NULL;
    }
    return cursor;
}

/* CloseConfCursor - close a conference cursor */
int CloseConfCursor(CCURSOR *cursor)
{
    free((void *)cursor);
    return 0;
}

/* SetCursorPosition - seek to a particular message number */
int SetCursorPosition(CCURSOR *cursor,long msgno)
{
    CFILE *cfile = cursor->cc_cfile;
    long offset;

    /* no current message */
    cursor->cc_ientry.ie_msgno = NULL_MSGNO;

    /* set the cursor position */
    if (msgno == 0) {
	offset = 0;
    }
    else if (msgno == -1) {
	if (fseek(cfile->cf_ifp,-(long)sizeof(IENTRY),SEEK_END) != 0)
	    return -1;
	offset = ftell(cfile->cf_ifp);
    }
    else {
        if (FindIndexEntry(cfile,msgno,&cursor->cc_ientry,&offset) != 0)
	    return -1;
    }

    /* remember the index position */
    cursor->cc_ioffset = offset;
    return 0;
}

/* GetNextMessage - get the message after a specified message number */
long GetNextMessage(CCURSOR *cursor)
{
    FILE *ifp = cursor->cc_cfile->cf_ifp;

    /* look for an active entry */
    do {
	if (fseek(ifp,cursor->cc_ioffset,SEEK_SET) != 0
	||  !ReadIEntry(ifp,&cursor->cc_ientry)
	||  cursor->cc_ientry.ie_offset == 0) {
	    cursor->cc_ientry.ie_msgno = NULL_MSGNO;
	    return -1;
	}
	cursor->cc_ioffset += sizeof(IENTRY);
    } while (cursor->cc_ientry.ie_flags & MF_DELETED);
    return cursor->cc_ientry.ie_msgno;
}

/* GetPrevMessage - get the message before a specified message number */
long GetPrevMessage(CCURSOR *cursor)
{
    FILE *ifp = cursor->cc_cfile->cf_ifp;

    /* look for an active entry */
    do {
	if (cursor->cc_ioffset == 0) {
	    cursor->cc_ientry.ie_msgno = NULL_MSGNO;
	    return -1;
	}
	cursor->cc_ioffset -= sizeof(IENTRY);
	if (fseek(ifp,cursor->cc_ioffset,SEEK_SET) != 0
	||  !ReadIEntry(ifp,&cursor->cc_ientry)) {
	    cursor->cc_ientry.ie_msgno = NULL_MSGNO;
	    return -1;
	}
    } while (cursor->cc_ientry.ie_flags & MF_DELETED);
    return cursor->cc_ientry.ie_msgno;
}

/* OpenCurrentMessage - open the current message */
CMESSAGE *OpenCurrentMessage(CCURSOR *cursor,long version)
{
    CMESSAGE *cmsg;

    /* make sure there is a current message */
    if (cursor->cc_ientry.ie_msgno == NULL_MSGNO)
	return NULL;

    /* allocate space for the new open message structure */
    if ((cmsg = (CMESSAGE *)malloc(sizeof(CMESSAGE))) == NULL)
	return NULL;

    /* copy the conference file and index entry */
    cmsg->cm_cfile = cursor->cc_cfile;
    cmsg->cm_ioffset = cursor->cc_ioffset;
    cmsg->cm_ientry = cursor->cc_ientry;
    
    /* finish opening the message */
    return FinishOpeningMessage(cmsg,version);
}

/* ClearStruct - clear a structure */
static void ClearStruct(char *addr,size_t size)
{
    for (; size > 0; --size)
	*addr++ = '\0';
}

/* ReadIEntry - read an IENTRY structure */
static int ReadIEntry(FILE *fp,IENTRY *entry)
{
    char ebuffer[_IESIZE];
    if (fread(ebuffer,_IESIZE,1,fp) != 1)
	return FALSE;
    ptoh_ientry(entry,ebuffer);
    return TRUE;
}

/* WriteIEntry - write an IENTRY structure */
static int WriteIEntry(FILE *fp,IENTRY *entry)
{
    char ebuffer[_IESIZE];
    htop_ientry(entry,ebuffer);
    return fwrite(ebuffer,_IESIZE,1,fp) == 1;
}

/* ReadDHeader - read a DHEADER structure */
static int ReadDHeader(FILE *fp,DHEADER *hdr)
{
    char hbuffer[_DHSIZE];
    if (fread(hbuffer,_DHSIZE,1,fp) != 1)
	return FALSE;
    ptoh_dheader(hdr,hbuffer);
    return TRUE;
}

/* WriteDHeader - write a DHEADER structure */
static int WriteDHeader(FILE *fp,DHEADER *hdr)
{
    char hbuffer[_DHSIZE];
    htop_dheader(hdr,hbuffer);
    return fwrite(hbuffer,_DHSIZE,1,fp) == 1;
}

/* ConfPathIndex - return the path to a conference index file */
static char *ConfPathIndex(char *path)
{
    static char filepath[256];
    oscvtpath(filepath,path);
    strcat(filepath,".ndx");
    return filepath;
}

/* ConfPathData - return the path to a conference data file */
static char *ConfPathData(char *path,int filen)
{
    static char filepath[256];
    oscvtpath(filepath,path);
    sprintf(&filepath[strlen(filepath)],".%03d",filen);
    return filepath;
}
