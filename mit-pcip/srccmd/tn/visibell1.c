/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include "attrib.h"

/* the visible bell - requires the terminal emulator screen handler
*/

extern char NORM_VIDEO, REV_VIDEO;

ring() {
	char hibuf[160];	/* buffer for high line */
	char lobuf[160];	/* buffer for low line */
	char ybuf[160];
	int y_start;
	int y_end;
	int x_start = 33;
	int x_end = 46;
	int x, y;

	/* loop in y for repetitions of the box */

	for(y_start=11, y_end = 12;
	    y_start >= 0;
	    y_start--, y_end++, x_start -= 3, x_end += 3) {
		read_line(hibuf, y_start);
		read_line(lobuf, y_end);
		for(x=x_start; x<=x_end; x++) {
			if(hibuf[(x<<1)+1]&INVERT)
				hibuf[(x<<1)+1] = NORM_VIDEO;
			else hibuf[(x<<1)+1] = REV_VIDEO;

			if(lobuf[(x<<1)+1]&INVERT)
				lobuf[(x<<1)+1] = NORM_VIDEO;
			else lobuf[(x<<1)+1] = REV_VIDEO;

			}

		write_line(hibuf, y_start);
		write_line(lobuf, y_end);

		for(y=y_start+1; y<y_end; y++) {
			read_line(ybuf, y);
			if(ybuf[(x_start<<1)+1] & INVERT)
				ybuf[(x_start<<1)+1] = NORM_VIDEO;
			else ybuf[(x_start<<1)+1] = REV_VIDEO;

			if(ybuf[(x_end<<1)+1] & INVERT)
				ybuf[(x_end<<1)+1] = NORM_VIDEO;
			else ybuf[(x_end<<1)+1] = REV_VIDEO;

			write_line(ybuf, y);
			}

/*		for(x=0; x<2500; x++);		*/

		for(x=x_start; x<=x_end; x++) {
			if(hibuf[(x<<1)+1]&INVERT)
				hibuf[(x<<1)+1] = NORM_VIDEO;
			else hibuf[(x<<1)+1] = REV_VIDEO;

			if(lobuf[(x<<1)+1]&INVERT)
				lobuf[(x<<1)+1] = NORM_VIDEO;
			else lobuf[(x<<1)+1] = REV_VIDEO;

			}

		write_line(hibuf, y_start);
		write_line(lobuf, y_end);

		for(y=y_start+1; y<y_end; y++) {
			read_line(ybuf, y);
			if(ybuf[(x_start<<1)+1] & INVERT)
				ybuf[(x_start<<1)+1] = NORM_VIDEO;
			else ybuf[(x_start<<1)+1] = REV_VIDEO;

			if(ybuf[(x_end<<1)+1] & INVERT)
				ybuf[(x_end<<1)+1] = NORM_VIDEO;
			else ybuf[(x_end<<1)+1] = REV_VIDEO;

			write_line(ybuf, y);
			}

		}
	}
