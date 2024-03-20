/* Default low-level CRT/Keyboard IO for IBM Acorn	6/82 SAW
 * May be replaced by user-supplied routines, eg for bitmap or for
 * terminal emulation.
 */

/* Write n characters to screen:					*/

_user_out(p, n)
  register char *p;
  register int n;
 {	while (n--)
	 { dos(2,*p);
	   if (*p++ == '\n') dos(2, '\r');
	 }
	return 0;
 }


/* Read n characters from kbd:						*/

#define	INBUF	80		/* Size of input buffer.		*/
char _user_b[INBUF+2];

_user_in(p, n)
 register char *p;
 register int n;
 {	register count = 0, x;
	int end;
	if ((x=_user_b[0]) == 0)	/* Anything left?	*/
	 { _user_b[0] = INBUF;		/* Nope.		*/
	   dos(0xA, _user_b);
	   dos(2, 0xA);			/* Echo CR/LF		*/
	   x = 2;
	   _user_b[_user_b[1]+2] = '\n';
	 }

	end = _user_b[1] + 2;
	while ((count < n) &&
	       (x <= end))	{ *p++ = _user_b[x++]; count++; }

	if (x > end) _user_b[0] = 0;
	else _user_b[0] = x;
	return count;
 }
