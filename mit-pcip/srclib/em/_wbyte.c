/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>

_wbyte(x, p)
	char x;
	FILE *p; {

	if(p->_file == 1) {
		em(x);
		return (unsigned)x;
		}

	return(--(p)->_cnt>=0? ((int)(*(p)->_ptr++=(unsigned)(x))):_flsbuf((unsigned)(x),p));
	}
