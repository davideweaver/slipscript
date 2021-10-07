/* ini_file.h - Copyright 1995,1996 David Weaver */


#ifndef INI_FILE_H_
#define INI_FILE_H_

/* ini_data - holds information from an ini file */
struct ini_data
{
   char *name;          /* Name of parameter (search tag) */
   char *value;         /* A string containing value as found or defaulted */

   int  found;          /* Was this parameter retrieved? */
};

/* prototype */
int loadIniFile(char *fname, struct ini_data *id);

#endif
 
