 /*
 * tunable variables
 */

#define	NBUF	15		/* size of buffer cache */
#define	NINODE	20		/* number of in core inodes */
#define	NFILE	20		/* number of in core file structures */
#define	NMOUNT	2		/* number of mountable file systems */
#define	MAXUPRC	25		/* max processes per user */
#define USRBASE	1		/* starting page of usr processes */
#define USRTOP	(0x80000/0x400)	/* page number of top of usr proc */
#define	MAXMEM	(USRTOP)	/* max core per process - first # is Kw */
#define	SSIZE	16		/* initial stack size (*1K bytes) */
#define	SINCR	16		/* increment of stack (*1K bytes) */
#define	NOFILE	20		/* max open files per process */
#define	CANBSIZ	256		/* max size of typewriter line */
#define	CMAPSIZ	20		/* size of core allocation area */
#define	SMAPSIZ	20		/* size of swap allocation area */
#define	NCALL	10		/* max simultaneous time callouts */
#define	NPROC	20		/* max number of processes */
#define	NTEXT	10		/* max number of pure texts */
#define	NCLIST	200		/* max total clist size */
#define	HZ	50		/* Ticks/second of the clock */
#define	TIMEZONE (5*60)		/* Minutes westward from Greenwich */
#define	DSTFLAG	1		/* Daylight Saving Time applies in this locality */
#define	MSGBUFS	128		/* Characters saved from error messages */
#define	NCARGS	5120		/* # characters in exec arglist */

/*
 * priorities
 * probably should not be
 * altered too much
 */

#define	PSWP	0
#define	PINOD	10
#define	PRIBIO	20
#define	PZERO	25
#define	NZERO	20
#define	PPIPE	26
#define	PWAIT	30
#define	PSLEP	40
#define	PUSER	50

/*
 * signals
 * dont change
 */

#include "../signal.h"

/*
 * fundamental constants of the implementation--
 * cannot be changed easily
 */

#define	NBPW	sizeof(int)	/* number of bytes in an integer */
#define	BSIZE	512		/* size of secondary block (bytes) */
/* BSLOP can be 0 unless you have a TIU/Spider */
#define	BSLOP	0		/* In case some device needs bigger buffers */
#define	NINDIR	(BSIZE/sizeof(daddr_t))
#define	BMASK	0777		/* BSIZE-1 */
#define	BSHIFT	9		/* LOG2(BSIZE) */
#define	NMASK	0177		/* NINDIR-1 */
#define	NSHIFT	7		/* LOG2(NINDIR) */
#define	USIZE	2		/* size of user block (*1K) */
#define	UBASE	(0x100000 - USIZE*0x400)	/* abs. addr of user block */
#define	NULL	0
#define	CMASK	0		/* default mask for file creation */
#define	NODEV	(dev_t)(-1)
#define	ROOTINO	((ino_t)2)	/* i number of all roots */
#define	SUPERB	((daddr_t)1)	/* block number of the super block */
#define	DIRSIZ	14		/* max characters per directory */
#define	NICINOD	100		/* number of superblock inodes */
#define	NICFREE	50		/* number of superblock free blocks */
#define	INFSIZE	138		/* size of per-proc info for users */
#define	CBSIZE	4		/* number of chars in a clist block */
#define	CROUND	07		/* clist rounding: sizeof(int *) + CBSIZE - 1*/

/*
 * Some macros for units conversion
 */
/* Core clicks (1K bytes) to segments and vice versa */
#define	ctos(x)	(x)
#define stoc(x) (x)

/* Core clicks (1K bytes) to disk blocks */
#define	ctod(x)	(x<<1)

/* inumber to disk address */
#define	itod(x)	(daddr_t)((((unsigned)x+15)>>3))

/* inumber to disk offset */
#define	itoo(x)	(int)((x+15)&07)

/* clicks to bytes */
#define	ctob(x)	(x<<10)

/* bytes to clicks */
#define	btoc(x)	((((unsigned)(x)+1023)>>10))

/* major part of a device */
#define	major(x)	(int)(((unsigned)x>>8))

/* minor part of a device */
#define	minor(x)	(int)(x&0377)

/* make a device number */
#define	makedev(x,y)	(dev_t)((x)<<8 | (y))

typedef	struct { int r[1]; } *	physadr;
typedef	long		daddr_t;
typedef char *		caddr_t;
typedef	long		mem_t;
typedef	unsigned short	ino_t;
typedef	long		time_t;
typedef	long		label_t[13];	/* regs d2-d7, a2-a7, pc */
typedef	short		dev_t;
typedef	long		off_t;

/*
 * Machine-dependent bits and macros
 */
#define	SMODE	0x2000		/* supervisor mode bit,note no user mode bit*/
#define	USERMODE(ps)	(((ps) & SMODE)==0)

#define	INTPRI	0x700		/* Priority bits */
#define	BASEPRI(ps)	(((ps) & INTPRI) != 0)


#define fuibyte(x)	fubyte(x)
#define fuiword(x)	fuword(x)
#define suibyte(x,y)	subyte(x,y)
#define suiword(x,y)	suword(x,y)
#define copyin(x,y,z)	copy(x,y,z)
#define copyiin(x,y,z)	copy(x,y,z)
#define copyout(x,y,z)	copy(x,y,z)
#define copyiout(x,y,z)	copy(x,y,z)

#define void int	/* so berkeley void coersions will work */
#define gsignal signal
#define MIN(a,b)	((a < b)?a:b)
