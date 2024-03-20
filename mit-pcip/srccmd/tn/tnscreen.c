/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>
#include <attrib.h>

/* Telnet screen handling. Routines which give shape to the telnet screen and
	control the 25th line.
	Format of the 25th line is:

	command/error_msgs 		foreign_host time current
*/

extern in_name tnhost;
extern char tnshost[];
unsigned clear25 = 0;		/* how many seconds to clear the 25th line in*/
unsigned att25 = NORMAL;	/* 25th line attribute byte */

tnscrninit() {
	clear_lines(24, 1);
	write_string(tnshost, 24, 70-strlen(tnshost), NORMAL);
	}

/* Print an error message on the 25th line, setting a timer which will cause
	it to be wiped out in 3 seconds. */

prerr25(s)
	char *s; {

	write_string(s, 24, 0, att25|BLINK_ON);
	clear25 = 3;
	}

prbl25(s)
	char *s; {

	write_string(s, 24, 0, att25|BLINK_ON);
	}


prat25(s, x)
	char *s;
	int x; {

	write_string(s, 24, x, NORMAL);
	}

inv25() {
	char line[160];
	int i;

	att25 = INVERT;
	read_line(line, 24);
	for(i=1; i<160; i=i+2) line[i] = INVERT;
	write_line(line, 24);
	}

norm25() {
	char line[160];
	int i;

	att25 = NORMAL;
	read_line(line, 24);
	for(i=1; i<160; i=i+2) line[i] = NORMAL;
	write_line(line, 24);
	}


pr25(x, s)
	int x;
	char *s; {

	write_string(s, 24, x, att25);
	}
