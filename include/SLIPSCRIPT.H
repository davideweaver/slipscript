/* slipscript.h */


#ifndef SLIPSCRIPT_H_
#define SLIPSCRIPT_H_

#include "../src/interp/ver_info.h"

extern "C" {
	#include <cgi.h>
}

#define seExit				0
#define seNoMemory			1
#define seNoCompiler		2
#define seNoInterpreter		3
#define seBadScript			4
#define seBadFilename		5


struct SLIPSCRIPTSTRUCT {

	llist	*entries;		/* cgi variable structure */
	void	*podbca;		/* first of three ODBC objects */
	void	*podbcb;
	void	*podbcc;
	void	*pECB;			/* extension control block */
	char	*redirect_url;	/* if script redirects, this is the url */
	char	*logdir;
	char	*includedir;
	int		usesystem;
	int		usefopen;
};

/* prototypes */
extern "C" int start_parsing( daves_buffer *dbuf, 
							 SLIPSCRIPTSTRUCT *slipstruct );
extern "C" int start_parsing_file( char *filename, daves_buffer *dbuf, 
								  SLIPSCRIPTSTRUCT *slipstruct  );
extern "C" int getServerVariable( void *pecb, char * name, char *value );
extern "C" void getIniValues( SLIPSCRIPTSTRUCT *ss );
extern "C" void freeMemory( SLIPSCRIPTSTRUCT *ss );

#endif