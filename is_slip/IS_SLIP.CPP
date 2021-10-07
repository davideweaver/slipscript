/* is_slip.cpp */
/* Copyright (c) 1996, David Weaver */

#include <httpext.h>
#include <string.h>
#include <stdio.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <util.h>
#include <slipscript.h>

extern "C" {
	#include <is_cgi.h>
}

/* constant defines */
#define EMAIL		"<A HREF=mailto://dew118@psu.edu>David Weaver</A>"
#define ERRTITLE	"SlipScript Error"

/* global declarations */

CRITICAL_SECTION gCS;		/* A critical section handle		*/
							/* is used to protect global		*/
							/* state properties					*/

/*	This the the entry and exit point for the extension			*/
/*	it is called when the extension is loaded and unloaded		*/
/*	by IIS.  This is where state properties need to be			*/
/*	retrieved from and store on persistant storage.				*/
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved ) {

    switch( ul_reason_for_call ) {

    case DLL_PROCESS_ATTACH: {

		InitializeCriticalSection( &gCS );
		break;
		}
    case DLL_PROCESS_DETACH: {

		DeleteCriticalSection( &gCS );
		break;
		}

	//case DLL_THREAD_ATTACH:
	//case DLL_THREAD_DETACH:

    }
    return TRUE;
}

/*	GetExtensionVersion - An ISAPI/Win32 API method				*/
/*	This method is required by IIS.  It is called				*/
/*	following the process load to ensure that the				*/
/*	extension is compatable with the server.					*/
BOOL WINAPI GetExtensionVersion( HSE_VERSION_INFO *pVer )
{
  pVer->dwExtensionVersion = MAKELONG(	HSE_VERSION_MINOR,
										HSE_VERSION_MAJOR );

  lstrcpyn( pVer->lpszExtensionDesc,
#ifdef _DEBUG
			"ISA SlipScript (debug)",
#else
            "ISA SlipScript",
#endif
            HSE_MAX_EXT_DLL_NAME_LEN );

  return TRUE;
} 

/*	HttpExtensionProc - ISAPI / Win32 API method				*/
/*	This method is a required by IIS.  It is called				*/
/*	once per client request.  This is where the extension		*/
/*	accomplishes its purpose in life.							*/
DWORD WINAPI HttpExtensionProc( EXTENSION_CONTROL_BLOCK *pECB )
 {

	CHAR						buff[255];
    CHAR						*lpszTemp = NULL;
	DWORD						dwLen=0;
	int							ret = 0;
	daves_buffer				*dbuf = NULL;
	SLIPSCRIPTSTRUCT			slipstruct;

#ifdef _DEBUG
	/* memory tracking stuff */
#endif

    pECB->dwHttpStatusCode = 0; // 0 Failure

	/* fill cgi variable stucture */
	llist entries;
	if( is_read_cgi_input( pECB, &entries ) == 0 )
		slipstruct.entries = &entries;
	
	slipstruct.redirect_url = NULL;
	slipstruct.pECB = pECB;

	getIniValues( &slipstruct );
	
	/* parse the HTML file */
	dbuf = new daves_buffer( 2000 );
	ret = start_parsing_file( pECB->lpszPathTranslated, dbuf, &slipstruct );
	

	/* if we didn't redirect */
	if( slipstruct.redirect_url == NULL ) {

		/* initialize connection */
		/* this should be under the programmer's control */
		wsprintf( buff, "Content-Type: text/html\r\n"
						"\r\n"
						"<!-- Page Created By: %s"
						" (%s) -->\n", SS_VERSION, pECB->lpszPathTranslated );

		dwLen = lstrlen( buff );
		if ( !pECB->ServerSupportFunction( pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER,
					"200 OK", &dwLen, (LPDWORD) buff ) ) {
				return HSE_STATUS_ERROR;
		}
	
		/* test the return value of start_parsing */
		switch( ret ) {

			case seNoMemory: 
				is_html_error( pECB, ERRTITLE, "Memory could not be allocated.", EMAIL );
				return HSE_STATUS_ERROR;
			case seNoInterpreter:
				is_html_error( pECB, ERRTITLE, "The interpreter could not be initialized.", EMAIL );
				return HSE_STATUS_ERROR;
			case seNoCompiler: 
				is_html_error( pECB, ERRTITLE, "The compiler could not be initialized.", EMAIL );
				return HSE_STATUS_ERROR;
			case seBadScript:
				is_html_error( pECB, ERRTITLE, "The embedded script was incomplete.", EMAIL );
				return HSE_STATUS_ERROR;
			case seBadFilename:
				char message[300];
				sprintf( message, "Could not find file - %s.", pECB->lpszPathTranslated ); 
				is_html_error( pECB, ERRTITLE, message, EMAIL );
				return HSE_STATUS_ERROR;
		}

		/* output the new buffer */
		dwLen = dbuf->length();
		pECB->WriteClient( pECB->ConnID, dbuf->data(), &dwLen, 0 );

		pECB->dwHttpStatusCode = 200; /* 200 OK */

	}
	else {		/* redirect to url */

		/* free up memory */
		if( dbuf ) delete dbuf;
//		freeMemory( &slipstruct );
		
		if( pECB->ServerSupportFunction( pECB->ConnID, HSE_REQ_SEND_URL_REDIRECT_RESP,
										 slipstruct.redirect_url, 0, NULL ) )
			return HSE_STATUS_SUCCESS;
	}										

	/* free up memory */
    if( dbuf ) delete dbuf;
//	freeMemory( &slipstruct );
	
#ifdef _DEBUG
	/* memory tracking stuff */
	_CrtDumpMemoryLeaks( );
#endif
	
    return HSE_STATUS_SUCCESS;
} 

extern "C" int getServerVariable( void *pecb, char *name, char *value ) {

	EXTENSION_CONTROL_BLOCK *pECB = ( EXTENSION_CONTROL_BLOCK * )pecb;
	DWORD dwLen = 255;
	if( pECB->GetServerVariable( pECB->ConnID, 
								 name, value, &dwLen ) )
		return 1;
	else 
		return 0;
}

/* this function gets ini values and fills in					*/
/* the slipstruct												*/
extern "C" void getIniValues( SLIPSCRIPTSTRUCT *ss ) {

	UCHAR value[255];
	DWORD dwLen = 255;
	HKEY ss_key;

	/* get include directory */
	if( RegOpenKeyEx( HKEY_CLASSES_ROOT,
				  "Software\\SlipScript\\IncludeDir",
				  0,
				  KEY_ALL_ACCESS,
				  &ss_key ) != ERROR_SUCCESS ) return;

	ss->includedir = new char[255];
	RegQueryValueEx( ss_key, NULL, NULL, NULL, 
					 ( unsigned char * )ss->includedir, &dwLen );

	/* get log directory */
	if( RegOpenKeyEx( HKEY_CLASSES_ROOT,
				  "Software\\SlipScript\\LogDir",
				  0,
				  KEY_ALL_ACCESS,
				  &ss_key ) != ERROR_SUCCESS ) return;

	ss->logdir = new char[255];
	RegQueryValueEx( ss_key, NULL, NULL, NULL, 
					 ( unsigned char * )ss->logdir, &dwLen );

	/* get system() call permission */
	if( RegOpenKeyEx( HKEY_CLASSES_ROOT,
				  "Software\\SlipScript\\UseSystem",
				  0,
				  KEY_ALL_ACCESS,
				  &ss_key ) != ERROR_SUCCESS ) return;

	RegQueryValueEx( ss_key, NULL, NULL, NULL, 
					 value, &dwLen );
	ss->usesystem = atoi( ( const char * )value );

	/* get fopen() call permission */
	if( RegOpenKeyEx( HKEY_CLASSES_ROOT,
				  "Software\\SlipScript\\UseFopen",
				  0,
				  KEY_ALL_ACCESS,
				  &ss_key ) != ERROR_SUCCESS ) return;

	RegQueryValueEx( ss_key, NULL, NULL, NULL, 
					 value, &dwLen );
	ss->usefopen = atoi( ( const char * )value );
				 
}
