/* Default character I/O for ACIA on IBM DOS	6/82 SAW
 * May be replaced by user-supplied routines, eg for interrupt-based
 * operation.
 */ 

_tty_out(p, n)
  register char *p;
  register int n;
 {	while (n--)
	 { dos(4, *p);
	   if (*p++ == '\n') dos(4, '\r');
	 }
	return 0;
 }


_tty_in(p, n)
 register char *p;
 register int n;
 {	register count = 0;
	for (;n--;count++) *p++ = dos(3);
 }
