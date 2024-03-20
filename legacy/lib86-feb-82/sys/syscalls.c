#include <sgtty.h>

/* system call writearounds for IBM DOS -- cjt & saw (1/82)
 * Updated for general (random-access) file reads & writes, SAW 6/8/82
 */

#define NFILE 20	/* number of fcb's to allocate 			*/
#define NDRIVE 4	/* number of disk drives supported 		*/

#define	RECPOW	7	/* log of recsize(below)			*/
#define	RECSIZE	(1<<RECPOW) /* record size we use; must be power of 2	*/

extern char *_stdi, *_stdo, *_stde;	/* defined in crt0		*/

struct _fcb {
  char drive;		/* first part replicates IBM DOS file control block */
  char filename[8];
  char fileext[3];
  char curblock[2];	/* short					*/
  char recsize[2];	/* short					*/
  char filesize[4];	/* long						*/
  char date[2];		/* short					*/
  char system[10];
  char currec;
  char ranrec[4];	/* long						*/
 
  char type;		/* tells type of device this fcb is for */
  long RWPointer;	/* current read/write pointer			*/
  long RWSize;		/* our copy of file size, for closing.		*/
};

#define	SHORT(XX) (*((short *) XX))
#define	LONG(XX)  (*((long  *) XX))


/* Macro to access ranrec:					*/

/* various devices */
#define F_FREE	0	/* this fcb is unallocated */
#define F_FILE	1	/* disk file */
#define F_USER	2	/* keyboard/display */
#define F_TTY	3	/* asynchronous communications adapater */

static struct _fcb files[NFILE];	/* array of fcb's */

extern _user_out(),	/* Interface to console I/O		*/
       _user_in(),
       _tty_out(),	/* and to external tty device		*/
       _tty_in();

/* initialize fcb structures */
dos_ini()
  {	register struct _fcb *f;

	/* mark all the fcb's as ready for allocation */
	for (f = files; f < &files[NFILE]; f++) f->type = F_FREE;

	/* now set up default files */


	if (_stdi)		/* redirected inpu?		*/
	 { 
	   if (open(_stdi, 0) != 0)
		{ prints("Cant open stdin\n"); _bomb(); }
	 }
	else
	   files[0].type = F_USER;	/* standard input from keyboard */

	if (_stdo)		/* redirected outut?		*/
	 {
	   if (creat(_stdo) != 1)
		{ prints("Cant create stdout\n"); _bomb(); }
	 }
	else
	   files[1].type = F_USER;	/* standard output to display */

	files[2].type = F_USER;		/* standard error to display */
}

/* Close all open files.  Called by _exit.
 */

_dos_cleanup()
  {	register struct _fcb *f;
	int fid;

	for (fid=0; fid < NFILE; fid++)
	 { if ((f = &files[fid])->type == F_FREE) continue;
	   close(fid);
	 }
 }


int close(fid)
 register int fid;
 {	register struct _fcb *f;

	if ((fid >= 0) && (fid < NFILE))
	  switch ((f = &files[fid])->type) {

	    case F_FILE:
			 _rwcheck(f);		/* adjust size		*/
			 LONG(f->filesize) =	/* set size (works???)	*/
				 f->RWSize;
			 dos(0x10, f);		/* tell DOS we're done.	*/

	    case F_USER:
	    case F_TTY:	
			 f->type = F_FREE;
			 return 0;

	    case F_FREE: return -102;	/* closed file.			*/

	    default:	 return(-1);
	  }

	return(-1);
}

/* trivial write-arounds:						*/

int creat(name,mode)
 char *name;
 {	int fid = _open(name, 1, 0x16);
	return fid;
 }

int open(name,mode)
 char *name;
 {	int fid = _open(name, mode, 0xF);
	return fid;
 }

int unlink(name)
 char *name;
 {	struct _fcb temp;
	if (!parsename(&temp, name)) return -1;
	if (1 & dos(0x13, &temp)) return -1;
	return 0;
 }

int rename(from, to)
 {	struct _fcb f1, f2;
	register char *cc, *dd;
	register i;
	for (cc = (char *) &f1, i = sizeof f1; i--; *cc++ = 0);
	if (!parsename(&f1, from) ||
	    !parsename(&f2, to)) return -1;	/* bad name syntax.	*/
	cc = ((char *) &f1) + 0x11;
	for (dd = (char *) (f2.filename), i = 11; i--;) *cc++ = *dd++;
	if (1&dos(0x17, &f1)) return -2;
	return 0;
 }

/* return length of an open file descriptor:				*/

long flength(fid)
 int fid;
 {	register struct _fcb *f;

	if ((fid >= 0) && (fid < NFILE))
	  switch ((f = &files[fid])->type) {

	    case F_USER: return 0L;

	    case F_TTY:	 return 0L;

	    case F_FILE: return f->RWSize;

	    case F_FREE:
	    default:	 return -1L;
	  }
	return(-101);			/* bad fid			*/
}


/* UNIX-like OPEN call... modes 0, 1 only.  1 does a create.		*/

int _open(name,mode,doscall)
  char *name;
  {	register struct _fcb *f;
	int fid;

	for (fid=0; fid < NFILE; fid++)
		if ((f = &files[fid])->type == F_FREE) goto gotone;
	return(-1);

gotone:
	if (strcmp(name,"tty:") == 0) f->type = F_TTY;
	else if (strcmp(name,"crt:") == 0) f->type = F_USER;
	else
	 { if (!parsename(f,name)) return -1;

	   f->currec = 0;		/* start at beginning of file */
	   LONG(f->ranrec) = 0;
	   SHORT(f->curblock) = 0;
	   SHORT(f->recsize) = RECSIZE;
	   LONG(f->filesize) = 0;
	   f->RWPointer = 0;

	   switch(mode)
	    { case 0:	if ((0xFF & dos(doscall, f)) != 0)
			 return -2;		/* Can't find file!	*/
			else break;		/* All OK.		*/
	      case 2:
	      case 1:   if ((0xFF & dos(doscall, f)) != 0)
			 return -3;		/* No room ???		*/
			else break;
	      default:	return -4;		/* No other modes.	*/
	    }

	   f->type = F_FILE;			/* Now, its in use!	*/

	 }
	f->RWSize = LONG(f->filesize);	/* Save, for closing.		*/
	return(fid);			/* return index for future use	*/
 }

int write(fid,buffer,nbytes)
 register char *buffer;
 {	register struct _fcb *f;

	if ((fid >= 0) && (fid < NFILE))
	  switch ((f = &files[fid])->type) {

	    case F_USER: return _user_out(buffer, nbytes);

	    case F_TTY:	 return _tty_out(buffer, nbytes);

	    case F_FILE: return _write(f, buffer, nbytes);

	    case F_FREE: return -102;	/* closed file.			*/

	    default:	 return(-1);
	  }
	return(-1);
}


/* Set up for a random file read/write.
 * Before calling, must preset _rwnb to nbytes, _rwextb to externally-
 *   supplied buffer adr.    Returns:
 *
 *	-1:	operation finished.
 *	0:	xfer an aligned block to/from *_rwsetp++.
 *	1:	read an aligned block into _rwbuf[...]; then
 *		grab _rwcnt bytes from *rwptr++, or write 
 *		_rwnb bytes to *rwptr++ and rewrite block.
 */

char _rwbuf[RECSIZE],	/* internal small buffer.			*/
     *_rwextb,		/* external buffer				*/
     *_rwptr;		/* Input/output pointer				*/

unsigned
    _rwcount,		/* number of bytes to grab from *rwptr++	*/
    _rwnb;		/* Number of bytes left.			*/

_rwsetup(f)
 register struct _fcb *f;
 {	register short rsize = SHORT(f->recsize), i;

	long mask,
	     ptr = f->RWPointer,
	     recno,
	     sptr, fptr;

	mask = rsize-1;

	/* KLUDGE to compute record number, viz RWPointer/recsize:	*/
	for (i = rsize, recno = ptr; i>1;) { i >>= 1; recno >>= 1; }
	LONG(f->ranrec) = recno;	/* RWPointer/recsize	*/

	if ((!(mask & ptr)) &&			/* aligned OK?		*/
	    (_rwnb >= SHORT(f->recsize)))
		{ f->RWPointer += SHORT(f->recsize);
		  _rwnb -= (_rwcount = SHORT(f->recsize));
		   dos(0x1A, _rwptr = _rwextb);	/* set transfer adr	*/
		  _rwextb += SHORT(f->recsize);	/* update it for next	*/
		  return 0;			/* request aligned xfer.*/
		}
	else if (!_rwnb) return -1;		/* done.		*/

	sptr = ptr & (~mask);			/* Beginning of block.	*/
	fptr = sptr + SHORT(f->recsize);	/* end of block.	*/
	_rwptr = _rwbuf + (ptr-sptr);		/* first byte to diddle	*/

	if ((fptr -= ptr) > _rwnb)		/* Extent of xfer?	*/
		_rwcount = _rwnb;		/* ... we'll finish.	*/

	else	_rwcount = fptr;		/* nope, past block end.*/

	_rwnb -= _rwcount;			/* adjust bytes left,	*/
	f->RWPointer += _rwcount;		/* read/write pointer.	*/

	dos(0x1A, _rwbuf);			/* preset transfer adr	*/
	clear(_rwbuf, rsize);			/* start clean.		*/
	return 1;				/* for unaligned	*/
 }

/* write, for files:							*/

_write(f, buffer, nbytes)
 register char *buffer;
 register struct _fcb *f;
 {	int i;
	long prev = f->RWPointer;	/* initial pointer, for return	*/

	_rwlen(f);			/* Lengthen, for randoms.	*/
	if (!nbytes) return 0;		/* in case of a touch.		*/

	_rwnb = nbytes;
	_rwextb = buffer;

	while ((i = _rwsetup(f)) >= 0)
	 { 
	   if (i == 0)			/* aligned transfer?		*/
		{
		  if (dos(0x22, f)) return -1;
		}
	   else	{
		  if ((i=dos(0x21, f)) == 2) return -1;  /* the read.	*/
		  while (_rwcount--) *_rwptr++ = *_rwextb++;
		  if (dos(0x22, f)) return -1;
		}
	 }
	_rwcheck(f);			/* adjust size.			*/
	return (int) ((f->RWPointer)-prev);
 }



int read(fid,buffer,nbytes)
 register char *buffer;
 {	register struct _fcb *f;
	register int count=0;


	if ((fid >= 0) && (fid < NFILE))
	  switch ((f = &files[fid])->type) {

	    case F_USER: return _user_in(buffer, nbytes);

	    case F_TTY:	 return _tty_in(buffer, nbytes);

	    case F_FILE: return _read(f, buffer, nbytes);

	    case F_FREE: return -102;	/* closed file.			*/
	    default:	 return(-100);	/* Bad file type.		*/
	  }

	return(-101);			/* bad fid			*/
}

/* read, for files:							*/

_read(f, buffer, nbytes)
 register char *buffer;
 register struct _fcb *f;
 {	register int i;
	long prev = f->RWPointer,	/* initial pointer, for return	*/
	     nleft;			/* number of bytes left in file.*/

	nleft = (f->RWSize) - prev;
	if (nbytes > nleft) nbytes = nleft;	/* don't read past EOF.	*/

	_rwnb = nbytes;
	_rwextb = buffer;

	while ((i = _rwsetup(f)) >= 0)
	 {
	   if (i == 0)			/* aligned transfer?		*/
		{
		  if (dos(0x21, f) == 2) return -1;
		}
	   else	{
		  if (dos(0x21, f) == 2) return -1;  /* the read.	*/

		  while (_rwcount--) *_rwextb++ = *_rwptr++;
		}
	 }
	return (int) ((f->RWPointer)-prev);
 }


int isatty(fid)
 {	register struct _fcb *f;


	if ((fid >= 0) && (fid < NFILE))
	  switch ((f = &files[fid])->type) {

	    case F_USER: return 1;

	    case F_TTY:	 return 1;

	    case F_FILE: return 0;

	    case F_FREE: return -102;	/* closed file.			*/
	    default:	 return(-100);	/* Bad file type.		*/
	  }

	return(-101);			/* bad fid			*/
}



/* Compare our internal file size to RWPointer, update former if nec */

_rwcheck(f)
 register struct _fcb *f;
 {
	if ((f->RWPointer) > (f->RWSize)) f->RWSize = f->RWPointer;
 }

/* Lengthen file, if necessary, by writing records of zeros until its
 * length is >=RWPointer.  Shouldnt be necessary if DOS did the
 * right thing.
 * NB: DOESN'T write the record containing RWPointer; it assumes that
 *  _write (who calls it) will fix up that record.
 * Returns 0 unless troubles.
 */

_rwlen(f)
 register struct _fcb *f;
 {	register short rsize = SHORT(f->recsize), i;
	short recno, lrec;
	recno = ((long)(f->RWSize)/rsize)+1;
	lrec = ((long)(f->RWPointer)/rsize);
	if (lrec <= recno) return 0;
	clear(_rwbuf, rsize);
	while (recno<lrec)
	 {
	   LONG(f->ranrec) = recno;	/* RWPointer/recsize	*/
	   dos(0x1A, _rwbuf);		/* preset transfer adr	*/
	   if (dos(0x22, f)) return -1;/* write.		*/
	   f->RWSize += rsize;
	   recno++;
	 }
	return 0;
 }

/* UNIX-like lseek... should do it all.					*/

long lseek(fid, offset, wh)
 int fid, wh;
 long offset;
 {	register struct _fcb *f;
	register int i;

	if ((fid >= 0) && (fid < NFILE))
	  switch ((f = &files[fid])->type) {

	    case F_TTY:
	    case F_USER: return -110;

	    case F_FILE:
			 _rwcheck(f);		/* adjust size		*/
			 switch(wh)
			  { case 1: offset += f->RWPointer; break;
			    case 2: offset += f->RWSize; break;
			    default: break;
			  }
			 return (f->RWPointer = offset);

	    case F_FREE: return -102;	/* closed file.			*/
	    default:	 return(-100);	/* Bad file type.		*/
	  }

	return(-101);			/* bad fid			*/
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

	if (fid>=0 && fid<NFILE && (f = &files[fid])->type==F_USER &&
	    request==TIOCGETP) return(0);
	prints("***ioctl call ignored\r\n");
	return(-1);
}

/* output messsage to console */
prints(p)
 register char *p;
 {	while (*p) _user_out(p++, 1);
 }

/* parse file name into fcb, return <>0 if successful */
parsename(f,name)
  struct _fcb *f;
  char *name;
  {	register char *p;
	register int i;

	/* make sure ':' only appears after one character drive name */

	p = name;
	if (p[1] == ':')
	 { if (*p >= 'a') i = *p - 'a';
	   else i = *p - 'A';
	   if (i<0 || i >= NDRIVE) return 0;
	   f->drive = i+1;
	   name += 2;
	 }
	else f->drive = 0;		/* default drive code */

drive:	for(p = f->filename, i = 0; i++ < 11; *p++ = ' ');

	/* copy in file name */
	for(p = f->filename, i = 0; *name && *name!='.'; name++)
	 { if (i++ == 8) return(0);	/* file name too long */
	   if (*name == '/')		/* flush directories	*/
	    { p = f->filename; i=0; continue; }
	   *p++ = *name;
	 }
	if (*name == '.') name++;	/* skip extension separator */

	/* copy in file extension */
	for(p = f->fileext, i = 0; *name; *p++ = *name++)
	  if (i++ == 3) return(0);	/* extension too long */

	return(1);
}
