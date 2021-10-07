// ODBC.h

#ifdef WIN32

#ifndef ODBC_H_
#define ODBC_H_

#include <windows.h>

#include <sql.h>
#include <sqlext.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

extern "C" {
	#include <util.h>
}

typedef struct DESCRIBE_ // ODBC column information
{
  void *pData ;       // pointer to allocated data storage, passed to ODBC
  SWORD sqlType ;     // ODBC SQL datatype for column
  int width ;         // display width of column
  BOOL leftJustify ;  // TRUE if column is left justified else right justified
  SDWORD cbValue ;    // column length/status value from ODBC
} DESCRIBE ;

typedef struct ODBC_  // ODBC control block
{
   char *pUID ;   // /U command line argument - user id
   char *pPWD ;   // /P command line argument - user password
   char *pDB ;    // /d command line argument - USE database
   char *pDSN ;   // /S command line argument - data source name (servername)
   char *pWSID ;  // /H command line argument - workstation name
   char colSeparator[2] ; // column separator string for formatted report (/s)
   long colWidth ;     // screen width for formatted report (/w)
   long connTimeout ;  // connect timeout (/l)
   long queryTimeout ; // statement timeout (/t)
   BOOL setQueryTimeout ; // SQLSetStmtOption(SQL_QUERY_TIMEOUT) supported?
   DESCRIBE *pCol ;    // column descriptions
   SWORD colMax ;      // # of columns
   char *pBuffer ;    // buffer for data print (size = colWidth + 2)
   HENV hEnv ;         // odbc environment handle
   HDBC hDbc ;         // odbc connection handle
   BOOL firstConnect ; // no previous connections on this DSN
   BOOL driverConnect ; // SQLDriverConnect supported?
   BOOL moreResults ;  // SQLMoreResults supported?
   BOOL setConnectOption ; // SQLSetConnectOption supported?
   BOOL setStmtOption ; // SQLSetStmtOption supported?
   long errLevel ;     // error level to print (/m)
   int exitCode ;      // process exit code
} ODBC ;

// values for ODBC::exitCode
// isql returns
#define EXIT_PRIORERROR -100  // Error occurred prior to exit/quit command

typedef struct IO_  // text input/output block
{
   FILE *pFileIn ;    // input (command) file
   FILE *pFileOut ;   // output file
   char *pLine ;     // line buffer (size is maxLine)
   char *pCurrent ;  // current buffer scan pointer
   char *pBegin ;    // ODBC save pointer in buffer
   char *pODBC ;     // saved text for ODBC
   char *pDisplay ;  // saved text for display
   int maxLine ;      // max line size ;
   BOOL skipText ;    // skipping over comments
   BOOL saveLines ;   // saving SQL lines for echo
   BOOL echoLines ;   // echo SQL lines
   BOOL promptLines ; // print prompt
   int lineNo ;       // command line #
} IO ;

typedef struct TOGGLE_  // toggle information
{
   char *pQuery ;    // query from /Q or /q
   BOOL exitQuery ;   // query and exit (/Q)
} TOGGLE ;

typedef struct TOKEN_  // token parsing block
{
   char *pText ;     // pointer to beginning of token
   SWORD lenText ;    // length of token
   UWORD type ;       // token type
   BOOL beginLine ;   // token first in line ?
} TOKEN ;

// values for TOKEN::type
#define TOKENSYMBOL 1  // ODBC symbol
#define TOKENINTEGER 2 // integer value
#define TOKENQUOTED 3  // quoted string
#define TOKENQUOTER 4  // illegal quoted string
#define TOKENDELIM 5   // delimiter

typedef struct ODBCERROR_ // ODBC Error Block
{
   RETCODE ret ;
   char state[SQL_SQLSTATE_SIZE + 1] ;
   char msg[SQL_MAX_MESSAGE_LENGTH] ;
   SDWORD nativeEr ;
   SWORD msgSz ;
} ODBCERROR ;

// command types
#define CMDGO 1
#define CMDEXIT 2
#define CMDRESET 3
#define CMDQUIT 4
#define CMDHELP 5


class ODBC_Access {
	BOOL showHead;
	ODBC odbc;
	IO io;
	char buf[1024];
	int nrows;
	int ncols;
	daves_buffer *output;
	BOOL StringAttach(char **, char *, SWORD, char *) ;
	char *SkipSpaces(char *) ;
	void AllocError(FILE *) ;
	BOOL ODBCSkip(IO *, char *) ;
	BOOL ODBCSave(IO *, char *) ;
	void FreeSQL(IO *) ;
	void ExecSQL(ODBC *, IO *) ;
	int ProcessInput(ODBC *, IO *) ;
	SDWORD PrintQuery(FILE *, ODBC *, HSTMT, SWORD) ;
	void PrintData(FILE *, ODBC *) ;
	BOOL SetODBCOption(FILE *, ODBC *, HSTMT, UWORD, UDWORD, BOOL *) ;
	BOOL TestODBCFunction(ODBC *, UWORD) ;
	BOOL ODBCConnect(FILE *, ODBC *, char *) ;
	BOOL ODBCStartup(FILE *, ODBC *) ;
	BOOL ODBCShutdown(FILE *, ODBC *) ;
	BOOL ODBCError(FILE *, ODBC *, HSTMT, RETCODE) ;
	BOOL ODBCErrorFetch(FILE *, ODBC *, HSTMT, RETCODE, ODBCERROR *) ;
	void ODBCErrorPrint(FILE *, ODBC *, ODBCERROR *) ;
public:
	ODBC_Access();
	~ODBC_Access();
	int getRowCount() { return nrows; };
	int doSQL(char *sql);
	void setDataBase(char *db) { odbc.pDB = db; };
	void setWorkstationID(char *ws) { odbc.pWSID = ws; };
	void setDataSource(char *ds) { odbc.pDSN = ds; };
	void setUserID(char *userid) { odbc.pUID = userid; };
	void setPassword(char *passwd) { odbc.pPWD = passwd; };
	void setColumnSeparator(char *sep) { strcpy(odbc.colSeparator,sep); };
	void printHeader(BOOL show) { showHead = show; };
	int getData(const char *pHeading, int row, char *value);
	int getColCount() { return ncols; };
	int connectSource();
	void closeSource();
	void querySource(char *sql);
	void reportData(char *output_format, daves_buffer *output, int rows = 0);
	int getColumn( const char *pHeading );
	int getCell( int row, int col, char *value );
};

// constructor
ODBC_Access::ODBC_Access() {

	// setup static values  
	nrows = 0;
	io.pFileIn = NULL;
	io.pFileOut = fopen("odbc.out", "wt");
	strcpy(odbc.colSeparator, "#");
	odbc.colWidth = 2000;
	odbc.connTimeout = odbc.queryTimeout = 8l;
	odbc.firstConnect = TRUE;
	odbc.errLevel = 16;
	odbc.pDB = NULL;	// - USE database
	odbc.pUID = NULL;	// - user id
	odbc.pPWD = NULL;   // - user password
    odbc.pDSN = NULL;   // - data source name (servername)
    odbc.pWSID = NULL;  // - workstation name
	io.pLine = buf;
	io.maxLine = 1024;
	io.promptLines = TRUE;
	io.echoLines = FALSE;
	showHead = TRUE;
	output = new daves_buffer( 1000 );
}

// destructor
ODBC_Access::~ODBC_Access() {

	if( output != NULL )
		delete output;
}

// perform the SQL statement
int ODBC_Access::doSQL(char *sql) {
	/*// startup connection
	if(connectSource()) {
		querySource(sql, out);
	}
   	closeSource();*/
	return 1;
}

// query the open connection
void ODBC_Access::querySource(char *sql) {

	io.pODBC = (char *)malloc(strlen(sql) + 2);
	strcpy(io.pODBC, sql);
	output->remove();
	ExecSQL(&odbc, &io);
}

// connect to the datasource
int ODBC_Access::connectSource() {
	if(odbc.pDSN == NULL && odbc.pDB == NULL) return 0;
	
	if(ODBCStartup(io.pFileOut, &odbc)) {
		odbc.pBuffer = (char *)malloc(odbc.colWidth + 2);
		if (odbc.pBuffer) return 1;
		else {
			AllocError(io.pFileOut);
			return 0;
		}
	}
	return 0;
}

// close down an open connection
void ODBC_Access::closeSource() {
	// shutdown connection
	ODBCShutdown(io.pFileOut, &odbc);
}

// login to ODBC data source
BOOL ODBC_Access::ODBCConnect(FILE *pOut, ODBC *pODBC, char *pDb)
{
   char *pConn, connStr[4096], *pDSN ;
   SWORD connOut ;
   RETCODE ret ;
   BOOL retFlag ;
   ODBCERROR errODBC ;
   pDSN = pODBC->pDSN ? pODBC->pDSN : (char*)"default" ;
   if (pODBC->driverConnect || pODBC->firstConnect)
   {
      pConn = connStr ;
//      pConn += sprintf(pConn, "DSN=%s", pDSN) ;
	  pConn += sprintf(pConn, "%s", pDSN) ;
/*      if (pODBC->pUID)
         pConn += sprintf(pConn, ";UID=%s", pODBC->pUID) ;
      if (pODBC->pPWD)
         pConn += sprintf(pConn, ";PWD=%s", pODBC->pPWD ? pODBC->pPWD : "") ;
      if (pDb)
         pConn += sprintf(pConn, ";DATABASE=%s", pDb) ;
      if (pODBC->pWSID)
         pConn += sprintf(pConn, ";WSID=%s", pODBC->pWSID) ;
*/
      ret = SQLDriverConnect(pODBC->hDbc, NULL, (UCHAR *)connStr, SQL_NTS, (UCHAR *)connStr,
                             128, &connOut, SQL_DRIVER_NOPROMPT) ;
      retFlag = ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ;
      if (retFlag)
      {
         pODBC->firstConnect = FALSE ;
         pODBC->driverConnect = TRUE ;
      }
      else
         while (ODBCErrorFetch(pOut, pODBC, SQL_NULL_HSTMT, ret, &errODBC))
            if (strcmp(errODBC.state, "IM001"))  // SQLDriverConnect support?
               ODBCErrorPrint(pOut, pODBC, &errODBC) ;
            else
               pODBC->driverConnect = FALSE ;
   }
   else
      retFlag = FALSE ;
   if (!retFlag)
   {
      ret = SQLConnect(pODBC->hDbc, (UCHAR *)pDSN, SQL_NTS, (UCHAR *)pODBC->pUID,
                       (SWORD) (pODBC->pUID ? SQL_NTS : 0), (UCHAR *)pODBC->pPWD,
                       (SWORD) (pODBC->pPWD ? SQL_NTS : 0)) ;
      retFlag = ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ;
      if (retFlag)
         pODBC->firstConnect = pODBC->driverConnect = FALSE ;
      else
         ODBCError(pOut, pODBC, SQL_NULL_HSTMT, ret) ;
   }
   if (retFlag)
   {
      pODBC->moreResults = TestODBCFunction(pODBC, SQL_API_SQLMORERESULTS) ;
      pODBC->setConnectOption =
                        TestODBCFunction(pODBC, SQL_API_SQLSETCONNECTOPTION) ;
      pODBC->setStmtOption =
                           TestODBCFunction(pODBC, SQL_API_SQLSETSTMTOPTION) ;
   }
   return retFlag ;
}

// startup ODBC
BOOL ODBC_Access::ODBCStartup(FILE *pOut, ODBC *pODBC)
{
   BOOL retFlag, dummy ;
   retFlag = FALSE ;
   if (SQLAllocEnv(&pODBC->hEnv) == SQL_SUCCESS)
   {
      if (SQLAllocConnect(pODBC->hEnv, &pODBC->hDbc) == SQL_SUCCESS)
      {
         pODBC->setConnectOption = TRUE ;
         SetODBCOption(pOut, pODBC, SQL_NULL_HSTMT, SQL_LOGIN_TIMEOUT,
                       pODBC->connTimeout, &dummy) ;
         retFlag = ODBCConnect(pOut, pODBC, pODBC->pDB) ;
         if (!retFlag)
            SQLFreeConnect(pODBC->hDbc) ;
      }
      else
         AllocError(pOut) ;
      if (!retFlag)
         SQLFreeEnv(pODBC->hEnv) ;
   }
   else
      AllocError(pOut) ;
   return retFlag ;
}

// shutdown ODBC
BOOL ODBC_Access::ODBCShutdown(FILE *pOut, ODBC *pODBC)
{
   SQLDisconnect(pODBC->hDbc) ;
   SQLFreeConnect(pODBC->hDbc) ;
   SQLFreeEnv(pODBC->hEnv) ;
   return TRUE ;
}

// set ODBC connect or stmt option
BOOL ODBC_Access::SetODBCOption(FILE *pOut, ODBC *pODBC, HSTMT hStmt, UWORD fOption,
                   UDWORD vParam, BOOL *pOption)
{
   RETCODE ret ;
   ODBCERROR errODBC ;
   if (hStmt)
   {
      if (!pODBC->setStmtOption)
         return *pOption = FALSE ;
      ret = SQLSetStmtOption(hStmt, fOption, vParam) ;
   }
   else
   {
      if (!pODBC->setConnectOption)
         return *pOption = FALSE ;
      ret = SQLSetConnectOption(pODBC->hDbc, fOption, vParam) ;
   }
   *pOption = TRUE ;
   while (ODBCErrorFetch(pOut, pODBC, hStmt, ret, &errODBC))
      if (strcmp(errODBC.state, "IM001") && strcmp(errODBC.state, "S1092") &&
          strcmp(errODBC.state, "S1C00"))
      {
         if (ret != SQL_SUCCESS_WITH_INFO)
            *pOption = FALSE ;
         ODBCErrorPrint(pOut, pODBC, &errODBC) ;
      }
      else
         *pOption = FALSE ;
   return *pOption ;
}

// call SQLGetFunctions to test for existence of a function
BOOL ODBC_Access::TestODBCFunction(ODBC *pODBC, UWORD fFunction)
{
   UWORD exists ;
   return SQLGetFunctions(pODBC->hDbc, fFunction, &exists) == SQL_SUCCESS &&
          exists == 1 ;
}

// dump ODBC error
BOOL ODBC_Access::ODBCError(FILE *pOut, ODBC *pODBC, HSTMT hStmt, RETCODE ret)
{
   BOOL retFlag ;
   ODBCERROR errODBC ;
   retFlag = FALSE ;
   while (ODBCErrorFetch(pOut, pODBC, hStmt, ret, &errODBC))
   {
       retFlag = TRUE ;
       ODBCErrorPrint(pOut, pODBC, &errODBC) ;
   }
   return retFlag ;
}

BOOL ODBC_Access::ODBCErrorFetch(FILE *pOut, ODBC *pODBC, HSTMT hStmt, RETCODE ret,
                    ODBCERROR *pErr)
{
   BOOL retFlag ;
   pErr->ret = ret ;
   retFlag = FALSE ;
   switch (ret)
   {
      case SQL_SUCCESS :
      case SQL_NO_DATA_FOUND :
         break ;
      case SQL_INVALID_HANDLE :
         strcpy(pErr->state, "******") ;
         strcpy(pErr->msg, "Unknown Error") ;
         pErr->nativeEr = 0 ;
         ODBCErrorPrint(pOut, pODBC, pErr) ;
         break ;
      default :
         ret = SQLError(pODBC->hEnv, pODBC->hDbc, hStmt, (UCHAR *)pErr->state,
                        &pErr->nativeEr, (UCHAR *)pErr->msg,
                        SQL_MAX_MESSAGE_LENGTH - 1, &pErr->msgSz) ;
         switch (ret)
         {
            case SQL_SUCCESS :
            case SQL_SUCCESS_WITH_INFO :
               retFlag = TRUE ;
               break ;
            case SQL_ERROR :
               pErr->ret = ret ;
               strcpy(pErr->state, "00000") ;
               strcpy(pErr->msg, "ODBC Failure") ;
               pErr->nativeEr = 0 ;
               ODBCErrorPrint(pOut, pODBC, pErr) ;
               break ;
            case SQL_NO_DATA_FOUND :
               break ;
            default :
               pErr->ret = SQL_ERROR ;
               strcpy(pErr->state, "00000") ;
               strcpy(pErr->msg, "Unknown Error") ;
               pErr->nativeEr = 0 ;
               ODBCErrorPrint(pOut, pODBC, pErr) ;
               break ;
         }
         break ;
   }
   return retFlag ;
}

// dump ODBC error
void ODBC_Access::ODBCErrorPrint(FILE *pOut, ODBC *pODBC, ODBCERROR *pErr)
{
   long errLevel ;
   switch (pErr->ret)
   {
      case SQL_SUCCESS :
      case SQL_NO_DATA_FOUND :
         errLevel = 0 ;
         break ;
      case SQL_SUCCESS_WITH_INFO :
         errLevel = 1 ;
         break ;
      default :
         pODBC->exitCode = EXIT_PRIORERROR ;
         errLevel = 16 ;
         break ;
   }
   if (errLevel >= pODBC->errLevel) {
      fprintf(pOut, "Msg %ld, Level %ld, State %s:\n%s\n",
              pErr->nativeEr, errLevel, pErr->state, pErr->msg) ;
	  output->remove();
	  output->append("Error, see ODBC.OUT for details.");
   }
}

void ODBC_Access::ExecSQL(ODBC *pODBC, IO *pIo)
{
   HSTMT hStmt ;
   BOOL success, dummy ;
   SWORD numCols ;
   SDWORD numRows ;
   RETCODE ret ;
   if (pIo->pODBC && *SkipSpaces(pIo->pODBC))
   {
      success = FALSE ;
      if (SQLAllocStmt(pODBC->hDbc, &hStmt) == SQL_SUCCESS)
      {
         SetODBCOption(pIo->pFileOut, pODBC, hStmt, SQL_QUERY_TIMEOUT,
                       pODBC->queryTimeout, &dummy) ;
         ret = SQLExecDirect(hStmt, (UCHAR *)pIo->pODBC, SQL_NTS) ;
         if (ret == SQL_SUCCESS)
            success = TRUE ;
         else
            ODBCError(pIo->pFileOut, pODBC, hStmt, ret) ;
      }
      else
      {
         hStmt = SQL_NULL_HSTMT ;
         AllocError(pIo->pFileOut) ;
      }
      if (pIo->echoLines)
         fprintf(pIo->pFileOut, pIo->pDisplay) ;
      while (success)
      {
         // if query, fetch results
         if (SQLNumResultCols(hStmt, &numCols) == SQL_SUCCESS && numCols) {
            numRows = PrintQuery(pIo->pFileOut, pODBC, hStmt, numCols) ;
		    nrows = numRows;
			ncols = numCols;
		 }
         else // else get number of rows affected
         {
            numRows = 0l ;
            SQLRowCount(hStmt, &numRows) ;
         }
         //fprintf(pIo->pFileOut, "\n(%ld rows affected)\n", numRows) ;
		 nrows = numRows;
         if (pODBC->moreResults)
         {
            ret = SQLMoreResults(hStmt) ;
            success = ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ;
            ODBCError(pIo->pFileOut, pODBC, hStmt, ret) ;
         }
         else
           success = FALSE ;
      }
      if (hStmt != SQL_NULL_HSTMT)
         SQLFreeStmt(hStmt, SQL_DROP) ;
   }
   else
      fprintf(pIo->pFileOut, "empty sql\n") ;
   FreeSQL(pIo) ;
}

// print query results
SDWORD ODBC_Access::PrintQuery(FILE *pOut, ODBC *pODBC, HSTMT hStmt, SWORD numCols)
{
	long colDisp ;
	SDWORD numRows ;
	char *pHeading, *pMarker, colName[32], *pText ;
	UDWORD prec ;
	SWORD i, len, scale, nullable, type ;
	int width, lenText ;
	BOOL leftJustify ;
	RETCODE ret ;
	numRows = 0l ;
	pODBC->colMax = numCols ;
	pODBC->pCol = (struct DESCRIBE_ *)malloc(numCols * sizeof (DESCRIBE)) ;
	if (pODBC->pCol)
	{
		  // build heading & markers
	  pHeading = pMarker = NULL ;
	  StringAttach(&pHeading, NULL, 0, pODBC->colSeparator) ;
	  StringAttach(&pMarker, NULL, 0, pODBC->colSeparator) ;
	  colDisp = 1 ;
	  for (i = 0 ; i < numCols ; i++)
	  {
		 pText = NULL ;
		 leftJustify = FALSE ;
		 ret = SQLDescribeCol(hStmt, (UWORD) (i + 1), (UCHAR *)colName, 32, &len, 
							  &pODBC->pCol[i].sqlType, &prec, &scale,
							  &nullable) ;
		 if (ret == SQL_SUCCESS)
		 {
			switch (pODBC->pCol[i].sqlType)
			{
			   case SQL_INTEGER :
			   case SQL_TINYINT :
			   case SQL_SMALLINT :
				  width = prec + 1 ;
				  lenText = sizeof (long) ;
				  type = SQL_C_LONG ;
				  break ;
			   case SQL_NUMERIC :
			   case SQL_DECIMAL :
				  width = prec + 2 ;
				  lenText = width + 1 ;
				  type = SQL_C_CHAR ;
				  break ;
			   case SQL_FLOAT :
			   case SQL_REAL :
			   case SQL_DOUBLE :
				  width = 20 ;
				  lenText = sizeof (double) ;
				  type = SQL_C_DOUBLE ;
				  break ;
			   case SQL_DATE :
				  width = 12 ;
				  lenText = sizeof (DATE_STRUCT) ;
				  type = SQL_C_DATE ;
				  break ;
			   case SQL_TIME :
				  width = 8 ;
				  lenText = sizeof (TIME_STRUCT) ;
				  type = SQL_C_TIME ;
				  break ;
			   case SQL_TIMESTAMP :
				  width = 20 ;
				  lenText = sizeof (TIMESTAMP_STRUCT) ;
				  type = SQL_C_TIMESTAMP ;
				  break ;
			   case SQL_BINARY :
			   case SQL_VARBINARY :
			   case SQL_LONGVARBINARY :
				  width = prec * 2 ;
				  lenText = width + 1 ;
				  type = SQL_C_CHAR ;
				  break ;
			   case SQL_BIT :
				  width = 1 ;
				  lenText = width + 1 ;
				  type = SQL_C_BIT ;
				  break ;
			   case SQL_BIGINT :
				  width = prec + 1 ;
				  lenText = width + 1 ;
				  type = SQL_C_CHAR ;
				  leftJustify = TRUE ;
				  break ;
			   case SQL_CHAR :
			   case SQL_VARCHAR :
			   case SQL_LONGVARCHAR :
			   default :
				  width = prec ;
				  lenText = width + 1 ;
				  type = SQL_C_CHAR ;
				  leftJustify = TRUE ;
				  break ;
			}
			pText = (char *)malloc(lenText) ;
			SQLBindCol(hStmt, (UWORD) (i + 1), type, (PTR) pText,
					   (SWORD) lenText, &pODBC->pCol[i].cbValue) ;
			if (width < (int) strlen(colName))
			   width = strlen(colName) ;
			if (width >= pODBC->colWidth)
			   width = pODBC->colWidth - 1 ;
			//sprintf(pODBC->pBuffer, "%-*.*s", width, width, colName) ;
			sprintf(pODBC->pBuffer, "%s%s", colName, pODBC->colSeparator);
			if (width + 1 + colDisp > pODBC->colWidth)
			{
			   StringAttach(&pHeading, NULL, 0, "\n\t") ;
			   StringAttach(&pMarker, NULL, 0, "\n\t") ;
			   colDisp = 8 ;
			}
			StringAttach(&pHeading, pODBC->pBuffer, (SWORD) width,
						 pODBC->colSeparator) ;
			memset(pODBC->pBuffer, '-', width) ;
			StringAttach(&pMarker, pODBC->pBuffer, (SWORD) width,
						 pODBC->colSeparator) ;
			colDisp += width + 1 ;
		 }
		 else
		 {
			ODBCError(pOut, pODBC, hStmt, ret) ;
			width = 0 ;
			pODBC->pCol[i].sqlType = 0 ;
		 }
		 pODBC->pCol[i].pData = pText ;
		 pODBC->pCol[i].width = width ;
		 pODBC->pCol[i].leftJustify = leftJustify ;
	  }
	  StringAttach(&pHeading, NULL, 0, "\n") ;
	  StringAttach(&pMarker, NULL, 0, "\n") ;
	  if(showHead) {
		  if (pHeading)
			 fprintf(pOut, pHeading) ;
		  if (pMarker)
			 fprintf(pOut, pMarker) ;
		  output->append(pHeading);
	  }
	  // fetch and print data
	  while ((ret = SQLFetch(hStmt)) == SQL_SUCCESS ||
			 ret == SQL_SUCCESS_WITH_INFO)
	  {
		 ODBCError(pOut, pODBC, hStmt, ret) ;
		 PrintData(pOut, pODBC) ;
		 ++numRows ;
	  }
	  if (ret != SQL_NO_DATA_FOUND)
		 ODBCError(pOut, pODBC, hStmt, ret) ;
	  for (i = 0 ; i < numCols ; i++)
		 free(pODBC->pCol[i].pData) ;
	  free(pHeading) ;
	  free(pMarker) ;
	  free(pODBC->pCol) ;
	}
	return numRows ;
}

// format and print data for ODBC fetch
void ODBC_Access::PrintData(FILE *pOut, ODBC *pODBC)
{
   SWORD i ;
   long colDisp ;
   char *pText, work[128] ;
   BOOL leftJustify ;
   DATE_STRUCT *pDate ;
   TIME_STRUCT *pTime ;
   TIMESTAMP_STRUCT *pTimestamp ;
   struct tm tm ;
   fprintf(pOut, pODBC->colSeparator) ;
   output->append(pODBC->colSeparator);
   for (colDisp = 1l, i = 0 ; i < pODBC->colMax ; ++i)
   {
      if (pODBC->pCol[i].width + 1 + colDisp > pODBC->colWidth)
      {
         fprintf(pOut, "\n\t") ;
         colDisp = 8 ;
      }
      if (pODBC->pCol[i].sqlType && pODBC->pCol[i].cbValue == SQL_NULL_DATA)
      {
         leftJustify = TRUE ;
         pText = "NULL" ;
      }
      else
      {
         leftJustify = pODBC->pCol[i].leftJustify ;
         switch (pODBC->pCol[i].sqlType)
         {
            case 0 :
                continue ;
            case SQL_INTEGER :
            case SQL_TINYINT :
            case SQL_SMALLINT :
               sprintf(pText = work, "%ld", *((long *) pODBC->pCol[i].pData)) ;
               break ;
            case SQL_FLOAT :
            case SQL_REAL :
            case SQL_DOUBLE :
               sprintf(pText = work, "%.6f",
                       *((double *) pODBC->pCol[i].pData)) ;
               break ;
            case SQL_DATE :
               pDate = (DATE_STRUCT *) pODBC->pCol[i].pData ;
               tm.tm_year = (pDate->year > 1900) ?
                            pDate->year - 1900 : pDate->year ;
               tm.tm_mon = pDate->month - 1 ;
               tm.tm_mday = pDate->day ;
               strftime(pText = work, 127, "%b %d %Y", &tm) ;
               break ;
            case SQL_TIME :
               pTime = (TIME_STRUCT *) pODBC->pCol[i].pData ;
               tm.tm_hour = pTime->hour ;
               tm.tm_min = pTime->minute ;
               tm.tm_sec = pTime->second ;
               strftime(pText = work, 127, "%I:%M%p", &tm) ;
               break ;
            case SQL_TIMESTAMP :
               pTimestamp = (TIMESTAMP_STRUCT *) pODBC->pCol[i].pData ;
               tm.tm_hour = pTimestamp->hour ;
               tm.tm_min = pTimestamp->minute ;
               tm.tm_sec = pTimestamp->second ;
               tm.tm_year = (pTimestamp->year > 1900) ?
                            pTimestamp->year - 1900 : pTimestamp->year ;
               tm.tm_mon = pTimestamp->month - 1 ;
               tm.tm_mday = pTimestamp->day ;
               strftime(pText = work, 127, "%b %d %Y %I:%M%p", &tm) ;
               break ;
            case SQL_BIT :
               sprintf(pText = work, "%d",
                       *((char *) pODBC->pCol[i].pData) != 0) ;
               break ;
            case SQL_BINARY :
            case SQL_VARBINARY :
            case SQL_LONGVARBINARY :
            case SQL_BIGINT :
            case SQL_CHAR :
            case SQL_VARCHAR :
            case SQL_LONGVARCHAR :
            case SQL_NUMERIC :
            case SQL_DECIMAL :
            default :
               pText = (char *)pODBC->pCol[i].pData ;
               break ;
         }
      }
      /*fprintf(pOut, leftJustify ? "%-*.*s%s" : "%*.*s%s",
              pODBC->pCol[i].width, pODBC->pCol[i].width, pText,
              pODBC->colSeparator) ;*/
	  fprintf(pOut, leftJustify ? "%s%s" : "%s%s",
              pText,
              pODBC->colSeparator) ;
	  output->append(pText);
	  output->append(pODBC->colSeparator);
      colDisp += pODBC->pCol[i].width + 1 ;
   }
   output->append("\n");
   fprintf(pOut, "\n") ;
}

void ODBC_Access::AllocError(FILE *pOut)
{
   fprintf(pOut, "Msg 0, Level 256, State S1000:\nAllocation Error\n") ;
}

void ODBC_Access::FreeSQL(IO *pIo)
{
   if (pIo->pODBC)
   {
      free(pIo->pODBC) ;
      pIo->pODBC = NULL ;
   }
   /*if (pIo->pDisplay)
   {
      free(pIo->pDisplay) ;
      pIo->pDisplay = NULL ;
   }*/
}



// start/end skip text for ODBC
BOOL ODBC_Access::ODBCSkip(IO *pIo, char *pCurrent)
{
   BOOL retFlag ;
   if (pCurrent)
   {  // start skipping, save previous
      retFlag = pCurrent == pIo->pBegin || ODBCSave(pIo, pCurrent) ;
      pIo->skipText = TRUE ;
      if (!retFlag)
         AllocError(pIo->pFileOut) ;
   }
   else
   {
      // end skipping
      retFlag = TRUE ;
      if (pIo->skipText)
      {
         pIo->pBegin = pIo->pCurrent ;
         pIo->skipText = FALSE ;
      }
   }
   return retFlag ;
}

// save non-comment text for ODBC
BOOL ODBC_Access::ODBCSave(IO *pIo, char *pCurrent)
{
   return pIo->skipText ||
         StringAttach(&pIo->pODBC, pIo->pBegin,
                      (SWORD) (pCurrent - pIo->pBegin), " ") ;
}

// scan over spaces (whitespace)
char *ODBC_Access::SkipSpaces(char *pScan)
{
   while (isspace(*pScan))
      ++pScan ;
   return pScan ;
}


// extend allocated string with characters + separator string
BOOL ODBC_Access::StringAttach(char **ppStr, char *pStr, SWORD len, char *pSeparator)
{
   SWORD sizeOld, sizeSeparator ;
   sizeSeparator = strlen(pSeparator) ;
   if (*ppStr)
   {
      sizeOld = strlen(*ppStr) ;
      *ppStr = (char *)realloc(*ppStr, sizeOld + len + sizeSeparator + 1) ;
   }
   else
   {
      sizeOld = 0 ;
      *ppStr = (char *)malloc(len + sizeSeparator + 1) ;
   }
   if (*ppStr)
   {
      memcpy(*ppStr + sizeOld, pStr, len) ;
      strcpy(*ppStr + sizeOld + len, pSeparator) ;
   }
   return *ppStr != NULL ;
}

// Retrieves data matching the heading and in the given row. Fills
// value with its pointer and returns the number of bytes 
int ODBC_Access::getData(const char *pHeading, int row, char *value)
{
	int count;
	int col_pos = 1;
	char *ptr = NULL;
	char *last_ptr = NULL;
	int ret = 0;
	int stop_when = 0;

	if(row > nrows) return 0;
	
	ptr = output->data();
	while( ( ptr = strstr( ptr, odbc.colSeparator ) ) != NULL ) {
		ptr++;
		ret = strnicmp( pHeading, ptr, strlen( pHeading ) );
		if( !ret ) break; /* matched the header */
		col_pos++; /* go to next column */
		if( col_pos > ncols ) return 0; /* no more columns left */
	}
	if( ptr == NULL ) return 0; /* end of data reached */

	stop_when = row * ncols + col_pos + row + 1; /* how many seperators should there be */
	ptr = output->data();
	for( int i=1; i<=stop_when; i++ ) {
		last_ptr = ptr;
		while( *( ptr++ ) != odbc.colSeparator[0] ) {
			if( ptr == NULL ) return 0;
		}
	}
	count = ptr - last_ptr;
	memcpy( value, last_ptr, count );
	value[count - 1] = '\0';
	return ( count - 1 );
}

void ODBC_Access::reportData( char *output_format, daves_buffer *out, int rows ) {

	int text[100], data[100];
	char *base, *ptr, *format;
	char value[1000];
	int i = 1;
	int size = 0;
	int get_rows;
	
	get_rows = nrows;
	if(rows > 0) 
		if(rows < nrows) get_rows = rows;

	if( get_rows == 0 ) return;

	/* make a copy of output_format */
	format = ( char * )malloc( strlen( output_format ) + 1 );
	if( format == NULL ) {
		out->append( "Out of memory error.\n" );
		return;
	}
	strcpy( format, output_format );

	/* parse format and fill arrays */
	base = ptr = format;
	text[0] = 0; /* set text base */
	ptr = strstr( ptr, "%" ); /* find first tag */
	while( ptr != NULL ) {
		ptr[0] = '\0';
		ptr++;
		data[i] = ptr - base;
		/* find trailer tag */
		if( ( ptr = strstr( ptr, "%" ) ) == NULL ) {
			out->append( "Error in report format string - " );
			out->append( format );
			delete( format );
			return;
		}
		ptr[0] = '\0';
		ptr++;
		text[i] = ptr - base;
		ptr = strstr( ptr, "%" );
		i++;
	}
	size = i;

	/* change data ptrs to cols */
	for( i=1; i<size; i++ ) {
		ptr = base + data[i];
		data[i] = getColumn( ptr );
	}

	for( i=1; i<=get_rows; i++ ) {
		for( int j=1; j<size; j++ ) {
			ptr = base + text[j - 1];
			out->append( ptr );
			if( getCell( i, data[j], value ) )
				out->append( value );
		}
		ptr = base + text[j - 1];
		out->append( ptr );
	}
	delete( format );
}

/* returns the col corresponding to a header */
int ODBC_Access::getColumn( const char *pHeading ) {

	int col_pos = 1;
	char *ptr = NULL;
	int ret = 0;

	ptr = output->data();
	while( ( ptr = strstr( ptr, odbc.colSeparator ) ) != NULL ) {
		ptr++;
		ret = strnicmp( pHeading, ptr, strlen( pHeading ) );
		if( !ret ) break; /* matched the header */
		col_pos++; /* go to next column */
		if( col_pos > ncols ) return 0; /* no more columns left */
	}
	if( ptr == NULL ) return 0; /* end of data reached */
	return col_pos;
}

/* returns the value of the cell at col,row */
int ODBC_Access::getCell( int row, int col, char *value ) {

	int count;
	char *ptr = NULL;
	char *last_ptr = NULL;
	int stop_when = 0;

	if( ( row > nrows ) || ( col > ncols ) ) return 0; /* invalid cell reference */
	
	stop_when = row * ncols + col + row + 1; /* how many seperators should there be */
	ptr = output->data();
	for( int i=1; i<=stop_when; i++ ) {
		last_ptr = ptr;
		while( *( ptr++ ) != odbc.colSeparator[0] ) {
			if( ptr == NULL ) return 0;
		}
	}
	count = ptr - last_ptr;
	memcpy( value, last_ptr, count );
	value[count - 1] = '\0';
	return ( count - 1 );
}
	
#endif // ODBC_H_

#endif // WIN32

