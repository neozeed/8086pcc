#include <stdio.h>

_wbyte(x, p)
	unsigned x;
	FILE *p; {

	return(--(p)->_cnt>=0?((int)(*(p)->_ptr++=(unsigned)(x))):_flsbuf((unsigned)(x),p));
	}
