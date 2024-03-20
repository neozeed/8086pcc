/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* read in a line using h19key() doing line editting
*/

#include <stdio.h>
#include <em.h>

#undef NULL

#include <h19.h>

#define	CONTROL_U	'\025'

h19line(buf, length, echo)
	char *buf;
	unsigned length;
	char echo; {
	int c;
	unsigned current = 0;
	int i;

	while(1) {
		while((c = h19key()) == NONE) ;

		switch(c) {
		case B_SPACE:
		case DELETE:
			/* delete a character */
			if(current) {
				if(echo) {
					putchar(B_SPACE);
					putchar(' ');
					putchar(B_SPACE);
					}
				buf[current--] = 0;
				}
			break;
		case CR:
		case LF:
			/* enter the line */
			putchar('\n');
			buf[current] = '\0';
			return(c);
			break;
		case ESC:
		case CONTROL_U:
			/* wipe the line */
			if(echo) {
				putchar('\r');
				for(i=0; i<current; i++)
					putchar(' ');
				putchar('\r');
				}
			else {
				putchar(' ');
				putchar('X');
				putchar('X');
				putchar('X');
				putchar('\n');
				}

			current = 0;
			break;
		case C_BREAK:
			/* completely punt this line */
			buf[0] = '\0';
			return(c);
			break;

		default:
			if(current < length-1) {
				buf[current++] = c;
				if(echo) putchar(c);
				}
			else ring();
			}
		}
	}
