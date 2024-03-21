#include <stdio.h>

putc(c, s)
	char c;
	FILE *s; {

	if(!(s->_flag & _IOASCII)) return(_wbyte(c, s));

	if(c == '\n') _wbyte('\r', s);
	return _wbyte(c, s);
	}
