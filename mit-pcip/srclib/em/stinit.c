/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include <stdio.h>

#include	<notice.h>

FILE *stdin, *stdout, *stderr;

/* this is a silly kluge to make sure that the new _wbyte will be linked in
*/
int _wbyte();
static int (*foo)() = _wbyte;

stinit() {
	space_initialize(0);

	scr_init();

	stdin  = fdopen(0, "ra");
	stdout = fdopen(1, "wa");
	stderr = fdopen(2, "wa");
	}
