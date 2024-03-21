#include <stdio.h>

/* getc which does newline folding on read */

int getc(s)
	FILE *s; {
	int c;

	if((c = _rbyte(s)) == EOF) return EOF;

	if(!(s->_flag & _IOASCII)) return(c&0377);

	if(c == '\r') {
		c = _rbyte(s);
		if(c != '\n') {
			ungetc(c, s);
			return('\r');
			}
		}

	return c;
	}

	
