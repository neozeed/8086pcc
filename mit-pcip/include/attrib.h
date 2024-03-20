/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* Definitions to allow hacking with the attributes of the screen.
 * This only works because of how the screen is done and because the
 * standard I/O library uses the H19 screen package.
 */

extern int	attrib;	/* contains the current attribute byte */

#define	UNDER	1	/* attribute for underline */
#define	NORMAL	0x07	/* attribute for normal video */
#define	INVERT	0x70	/* attribute for inverse video */
#define	BLINK_ON 0x80	/* or into attrib to cause blinking */
#define	BLINK_OFF 0x7f	/* and into attrib to disable blinking */
