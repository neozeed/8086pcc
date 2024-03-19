#include	<stdio.h>
#include	<errno.h>

struct _iobuf *
fopen(file, mode)
char *file;
register char *mode;
{
	extern int errno;
	register f;
	register struct _iobuf *iop;
	extern struct _iobuf *_lastbuf;
	static char init=0;


	if (!init)
	 { register char *p = (char *) &(_iob[3]);
	   register int i, top;

	   _lastbuf = &_iob[_NFILE];	/* SAW: somehow, we need this too. */

	   for (i = (_NFILE-3)*(sizeof _iob[0]); i--;) *p++ = 0;
	   init = 1;
	 }

	for (iop = _iob; iop->_flag&(_IOREAD|_IOWRT); iop++)
		if (iop >= _lastbuf) return(NULL);


	if (*mode=='w')
		f = creat(file, 0666);
	else if (*mode=='a') {
		if ((f = open(file, 1)) < 0) {
			if (errno == ENOENT)
				f = creat(file, 0666);
		}
		lseek(f, 0L, 2);
	} else
		f = open(file, 0);
	if (f < 0)
		return(NULL);
	iop->_cnt = 0;
	iop->_file = f;
	if (*mode != 'r')
		iop->_flag |= _IOWRT;
	else
		iop->_flag |= _IOREAD;
	return(iop);
}
