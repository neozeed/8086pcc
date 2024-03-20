#include <sgtty.h>
#include <stdio.h>

/* system call writearounds for MSDOS 2.0
 * Modified March 85 by JSP [see getenv() and system()]
 */

#define	SHORT(XX) (*((short *) XX))
#define	LONG(XX)  (*((long  *) XX))

/* output messsage to console */
prints(p)
 register char *p;
 {	while (*p) write(2, p++, 1);
 }

extern long lseek();

/* UNIX-style getenv:
 * Modified March 85 by JSP to include correct '=' and ' ' detection
 */

char *getenv(name)
 char *name;
 {	static char buf[64];			/* Sigh.		*/
	int i;
	register char *aa, *bb;

	for (i = 0; ; i++) {
	    _genv(i, buf);
	    if (!buf[0]) return 0;
	    for (aa = name, bb = buf; ; aa++, bb++) {
		if (!*aa && (*bb == '=' || *bb == ' ')) {
		   while (*bb == '=' || *bb == ' ') bb++;
		   /* return pos of first non-space char after the '=' */
		   return bb;
		   }
		if (*aa != *bb) break;
		}
	    }
 }

/* Call shell to execute cmd.  If cmd == 0, simply calls an
 * interactive shell.
 * Modified March 85 by JSP to include SWITCHAR sensing
 * Modified July 85 by SAW to do switchar sensing properly.
 */

system(cmd)
 char *cmd;
 {	char cmdbuf[100];
	register char *aa, *bb;
	char *shell, switchar;
	int i;

	if (!(switchar = _swtch()))
	   switchar = '/';			/* default dos switchar */

	aa = cmdbuf + 1; *cmdbuf = 0;

	if (cmd) {
	   *aa++ = switchar;		/* forms -C or /C as necessary */
	   *aa++ = 'C';
	   cmdbuf[0] += 2;
	   for (bb = cmd; *aa = *bb++; aa++) (*cmdbuf)++;
	   }
	*aa++ = '\r'; *aa++ = '\n'; *aa++ = 0;

	if (!(shell = getenv("COMSPEC"))) shell = "COMMAND.COM";

	i = _system(cmdbuf, shell);
	return i;
 }


/* return length of an open file descriptor:
 */

long flength(fid)
 {	long dot, size;
	dot = lseek(fid, 0L, 1);
	size = lseek(fid, 0L, 2);
	lseek(fid, dot, 0);
	return size;
 }

dos_ini()
 {
 }

/* Close all open files.  Called by _exit.
 */

_dos_cl()
 {
 }

int isatty(fid)
 {
	return(-1);
 }

char *sbrk(incr)
  {	char a[1];
	extern char *_memtop;	/* current top of user data */
	register char *p,*q;

	q = _memtop;		/* answer if we succeed */
	if (((int) q) & 1) q++;	/* SAW: malloc seems to want int align	*/

	p = &q[(incr+1)&~1];	/* new top of user data */

	/* allocation succeeds if we didn't wrap around, and if we
	 * are still below stack.
	 */
	if (p >= _memtop && p < &a[-64])
	 { _memtop = p;
	   return(q);
	 }

	return ((char *) -1);
 }

ioctl(fid,request,argp)
 {	register struct _fcb *f;

	prints("***ioctl call ignored\r\n");
	return(-1);
 }
