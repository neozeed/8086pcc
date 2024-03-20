/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* lint file for symbols defined in assembly code by the emulator
*/
/* from curse.a86 */
_curse() { }

/* from kh.a86 */
int kbd_stat() { }

char kbd_in() { }

/* from sh.a86 */
scr_init() { }

/*ARGSUSED*/
move_lines(x, y, n)
	int x, y, n; { }

scrollup() { }

scrolldn() { }

/*ARGSUSED*/
clear_lines(y, n)
	int y, n; { }

/*ARGSUSED*/
write_line(buf, y)
	char *buf;
	int y; { }

/*ARGSUSED*/
read_line(buf, y)
	char *buf;
	int y; { }

/*ARGSUSED*/
write_char(c, y, x)
	int c;
	int y, x; { }

/*ARGSUSED*/
set_cursor(y, x)
	int y,x; { }

rset_cursor() { }

scr_rest() { }
