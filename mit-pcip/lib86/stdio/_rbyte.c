#include <stdio.h>

_rbyte(p)
	FILE *p; {

	return(--(p)->_cnt>=0? *(p)->_ptr++&0377:_filbuf(p));
	}
