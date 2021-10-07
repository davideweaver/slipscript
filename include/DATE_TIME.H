////////////////////////////////////////////////////////////////////////
// date_time.h
// a class for obtaining a formatted time and date
//
// Dave Weaver - 1/12/97

#include <time.h>

class date_time {
public:
	void get_formatted(char *format, char *buf);
};

void date_time::get_formatted(char *format, char *buf) {
	
	char *ptr, *ptr2;
	time_t now;
	struct tm *tm;
	static char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	static char *wdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

	time ( &now );
	tm = localtime ( &now );

	for ( ptr = format, ptr2 = buf; *ptr != '\0'; ptr++ ) {
		if ( strncmp ( ptr, "%a", 2 ) == 0 ) {
			sprintf ( ptr2, "%3s", wdays[tm->tm_wday] );
			ptr++;
			ptr2 += 3;
		}
		else if ( strncmp ( ptr, "%b", 2 ) == 0 ||
			strncmp ( ptr, "%h", 2 ) == 0 ) {
			sprintf ( ptr2, "%3s", months[tm->tm_mon] );
			ptr++;
			ptr2 += 3;
		}
		else if ( strncmp ( ptr, "%d", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_mday );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%H", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_hour );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%I", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_hour % 12 ? 12: tm->tm_hour );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%m", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_mon + 1 );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%p", 2 ) == 0 ) {
			sprintf ( ptr2, "%2s", tm->tm_hour > 11 ? "PM" : "AM" );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%S", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_sec );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%y", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_year % 100 );
			ptr++;
			ptr2 += 2;
		}
		else if ( strncmp ( ptr, "%Y", 2 ) == 0 ) {
			sprintf ( ptr2, "%04d", tm->tm_year + 1900 );
			ptr++;
			ptr2 += 4;
		}
		else if ( strncmp ( ptr, "%M", 2 ) == 0 ) {
			sprintf ( ptr2, "%02d", tm->tm_min );
			ptr++;
			ptr2 += 2;
		}
		else {
			*ptr2 = *ptr;
			ptr2++;
		}
	}
	*ptr2 = '\0';	/* NULL terminate the path */
}