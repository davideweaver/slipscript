/* is_slipscript.cpp	*/

#include <iostream.h>
#include <fstream.h>
#include <string.h>

extern "C" {
	#include <cgi.h>
	#include <interp.h>
	#include <util.h>
	#include "../src/interp/bob.h"
}

#include <slipscript.h>

/* define slipscript's output callbacks */
typedef struct {
  StreamIODispatch *d;
} ConsoleStream;

static int CloseConsoleStream(Stream *s)
{
    return 0;
}

static int ConsoleStreamGetC(Stream *s)
{
    return getchar();
}

static int ConsoleStreamPutC( void *c, int ch, Stream *s )
{
	InterpreterContext *ic = ( InterpreterContext * )c;
	daves_buffer *output = ( daves_buffer * )ic->pextra1;

	output->append( (char)ch );

	return 1;
}

StreamIODispatch consoleIODispatch = {
  CloseConsoleStream,
  ConsoleStreamGetC,
  ConsoleStreamPutC
};

ConsoleStream consoleStream = { &consoleIODispatch };

/* error handler for slipscript's errors */
void ErrorHandler( InterpreterContext *c, int code, void *data )
{
	char tmp[255];
	daves_buffer *output = ( daves_buffer * )c->pextra1;
	
    switch ( code ) {
    case ecExit:
		throw seExit; 
	case ecTypeError:
		output->append( "\n<BR><TT><B>error: \n" );
		output->append( GetErrorText( code ) );
		output->append( "<BR>\nsymbol: " );
		DisplayValue( c, data, (Stream *)&consoleStream );
		output->append( "<BR>\nin or directly after function: " );
		output->append( c->err );
		output->append( "</B></TT>\n" );
		throw output;
		break;
	case ecSyntaxError:
		sprintf( tmp, "<BR><TT><B>\n%s\n</B></TT><BR>\n", c->err );
		output->append( tmp );
		throw output;
		break;
	case ecTooManyArguments:
	case ecTooFewArguments:
		sprintf( tmp, "\n<BR><TT><B>error: \n%s<BR>\nin function: %s</B></TT><BR>\n",GetErrorText( code ), c->err );
		output->append( tmp );
		throw output;
		break;
	case ecNoMethod:
		output->append( "\n<BR><TT><B>error: \n" );
		output->append( GetErrorText( code ) );
		output->append( "<BR>\nsymbol: " );
		DisplayValue( c, data, (Stream *)&consoleStream );
		throw output;
		break;
    default:
		sprintf( tmp, "<BR><TT><B>\nerror: %s", GetErrorText( code ) );

		/* if it is an unbound variable, display the variable */
		if( code == 6 ) {
			output->append( tmp );
			output->append( " - " );
			DisplayValue( c, data, (Stream *)&consoleStream );
		}
		else {
			output->append( tmp );
		}
		output->append( "</B></TT><BR>" );

		/* throw an exception which will end the session */
		throw output;
		
    }
}
/* end of slipscript's output callbacks */


/* verify the parameters of the <SCRIPT tag */
int verify_script_tag( char *str ) {

	char *parameter;
	int ret = 0;

	/* check if language parameter is correct */
	parameter = getTagValue( str, "language", " " );
	if( parameter == NULL ) return 0;
	if( !stricmp( parameter, "slipscript" ) ) ret = 1;
	
	//parameter = getTagValue( str, "mode", " " );
	//if( parameter == NULL ) return ret;
	//if( !stricmp( parameter, "debug" ) ) {
	//	/* do something */
	//	ret = 1;
	//}
	
	return ret;
}

InterpreterContext *init_interpreter()
{

    InterpreterContext *ic;
	
	/* create an interpreter context (heap) */
    ic = NewInterpreterContext(16384,1024);
	SetExtraInterpreterContextData( ic, ( void * )NULL, ( void * )NULL );
     
    /* setup the error handler */
    ic->errorHandler = ErrorHandler;
    
    /* setup standard i/o */
    ic->standardInput = (Stream *)&consoleStream;
    ic->standardOutput = (Stream *)&consoleStream;
    ic->standardError = (Stream *)&consoleStream;
    
    /* add the library functions to the symbol table */
    EnterLibrarySymbols(ic);
    ic->findFunctionHandler = FindLibraryFunctionHandler;
    
    return ic;
}

CompilerContext *init_compiler( InterpreterContext *ic )
{

    /* create a compiler context */
    return InitCompiler(ic,4096,256);
    
}


/* sends the found script to the interpreter */
char *do_script( char *start, InterpreterContext *ic ) {

	char *end, *script, *ret;
	Stream *s;
 	
	end = start;
	/* advance ptr */
	end++;
	end++;
	
	/* it is a slipscript script */
	script = end;
	/* now find terminating </SCRIPT> tag */
	end = strstr( script, "@>" );
	if( end == NULL )
		throw seBadScript;

	end[0] = '\0';
	end++;
	ret = end;
	/* compile script */
	try {
		s = CreateStringStream( ( unsigned char * )script, strlen( script ) );
#ifdef _DEBUG
	writeString( "script", script );
#endif
		CompileFile( ( CompilerContext * )ic->cc, s, ic->standardOutput );
	}
	catch( int ret ) {
		/* if ret = 0, then exit() was called, do nothing */
		if( ret ) {
			/* an error occured */
		}
	}
	free( s );

	return ++ret;
}

int find_script( InterpreterContext *ic, char *script ) {

	char *start, *end;
	daves_buffer *output = ( daves_buffer * )ic->pextra1;

	/* find the script tag */
	if( ( start = strstr( script, "<@" ) ) == NULL ) {
		output->append( script );
		return 1;
	}

	end = start;
	/* loop while script tags are found */
	while( end != NULL ) {
		/* print out non-script */
		end[0] = '\0';
		output->append( script );
		end[0] = '<';

		/* now parse script tags */
		start = do_script( end, ic );

		/* find another script if there is one */
		end = strstr( start, "<@" );
		script = start;

	}
	if( start != NULL) 
		output->append( start );

	return 1;
}

/* This is the entry point for running the SlipScript			*/
/* interprter. The buffer contains the actual contents			*/
/* of the html file and is then filled with the output			*/
/* when returned.												*/
/* returns:														*/
/*			1 = memory allocation error							*/
/*			2 = could not initialize compiler					*/
/*			3 = could not initialize interpreter				*/
/*			4 = incomplete script found							*/
extern "C" int start_parsing( daves_buffer *buffer, 
							 SLIPSCRIPTSTRUCT *slipstruct ) {

	InterpreterContext *ic;
	daves_buffer *output;
	char *old_buffer = buffer->data();

#ifdef _DEBUG
	beginDebug( "c:\\temp\\error.out" );
	writeFunction( "start_parsing" );
	writeInt( "len", buffer->length() );
#endif

	// create output buffer 
	output = new daves_buffer( 2000 );
	if( output == NULL ) return seNoMemory;

	// setup compiler and interpreter 
	ic = init_interpreter();
	slipstruct->podbca = NULL;
	slipstruct->podbcb = NULL;
	slipstruct->podbcc = NULL;

	if( ic == NULL ) return seNoInterpreter;
	SetExtraInterpreterContextData( ic, output, slipstruct );

	// initialize compiler 
	ic->cc = init_compiler( ic );
	if( ic->cc == NULL ) return seNoCompiler;

	// extract the script from the buffer 
	try {
		find_script( ic, buffer->data() );
	}
	catch( daves_buffer *error_output ) {
		*buffer = *error_output;
		return 0;
	}
	catch( int err_code ) {
		return err_code;
	}

	// cleanup interpreter 
	cleanup( ic );

	// assign output to buffer 
	
	buffer->remove();
	buffer->append( output->data() );
	
	delete output;

#ifdef _DEBUG
	writeString( "buffer", buffer->data() );
	endDebug();
#endif

	return 0;
}

/* this function takes a filename for input						*/
/* returns:														*/
/*			4 = bad memory allocation							*/
/*			5 = bad filename sent								*/
extern "C" int start_parsing_file( char *filename, daves_buffer *dbuf, 
								  SLIPSCRIPTSTRUCT *slipstruct ) {

	char buff[255];
	FILE *fp;


	/* read in client specified file */
	if( dbuf == NULL ) return seNoMemory;

	/* open file */
	fp = fopen( filename, "r" );
	if( fp == NULL ) return seBadFilename;
		
	/* put contents in buffer */
	while( fgets( buff, 255, fp ) != NULL )
		dbuf->append( buff );
	fclose( fp );

	/* parse the file buffer */
	return start_parsing( dbuf, slipstruct );

}

/* this function frees all memory needed to						*/
/* compile and execute a script									*/
extern "C" void freeMemory( SLIPSCRIPTSTRUCT *ss ) {

	if( ss->entries != NULL  ) list_clear(ss->entries); 
	if( ss->redirect_url != NULL ) free( ss->redirect_url ); 
	if( ss->logdir != NULL ) free( ss->logdir );
	if( ss->includedir != NULL ) free( ss->includedir );
}
