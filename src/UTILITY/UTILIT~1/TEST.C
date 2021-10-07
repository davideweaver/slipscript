/* test file for the utility library */

#include <stdio.h>
#include <stdlib.h>		/* for free() */
#include <string.h>		/* for strlwr() */

#include "..\ini_file.h"
#include "..\tags.h"
#include "..\str.h"


struct ini_data inid[] = {
	{"dave","default",0},
	{"michelle","default",0},
	{"","",0}
};

void main() {

	int ret;
	int testcase = 2;

	switch(testcase) {
	case 1: {
				char *fname = "test.ini";
				
				/* ini_file test */
				ret = loadIniFile(fname, &inid[0]);
				printf("%i\n",ret);
				printf("name=%s,value=%s\n",inid[0].name,inid[0].value);
				printf("name=%s,value=%s\n",inid[1].name,inid[1].value);
			}
	case 2: {
				char str[] = "this <ais a test stringA>, the <Atag is \t\n on \nA> a newline";
				char *contents;

				/* tags test */
				contents = getTagInString(str, "<A,A>");
				if(contents != NULL) 
					printf(contents);

				contents = getTagInString(str, "<A,A>");
				if(contents != NULL)
					printf(contents);
			}
	case 3: {
				/* test getTagValue */ 
				char *ret;
				char str[] = " language  \n= \t \"   SLIPSCRIPT OH YES\"";
				ret = getTagValue( str, "language", " " );
				printf( str );
				printf( ret ); 
			}
	/*case 4: {
				int ret;
				char str[] = "<>xxyy";
				char str2[] = "<>xXYy";
				ret = stricmp( str, str2 );
				printf( str );
				printf( ret );
			}
	case 5: {
				int ret;
				char str[] = "aAbBcC.... !@#$%^&*()<>xXyYzZ\nYeS";
				char str2[] = "<>xX";
				ret = test_stricmp( str, str2 );
				printf( str );
				printf( ret );
			}*/
	}

}
