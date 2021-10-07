/* string-lib.h - headers for string-lib.c
   $Id: string-lib.h,v 1.2 1996/07/24 07:32:22 eekim Exp eekim $
*/

char *newstr(char *str);
char *substr(char *str, int offset, int len);
char *replace_ltgt(char *str);
