/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

_prints(s)
	register char *s; {

	write(1, s, strlen(s));
	}
