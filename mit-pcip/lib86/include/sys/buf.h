/*
 * Each buffer in the pool is usually doubly linked into 2 lists:
 * the device with which it is currently associated (always)
 * and also on a list of blocks available for allocation
 * for other use (usually).
 * The latter list is kept in last-used order, and the two
 * lists are doubly linked to make it easy to remove
 * a buffer from one list when it was found by
 * looking through the other.
 * A buffer is on the available list, and is liable
 * to be reassigned to another disk block, if and only
 * if it is not marked BUSY.  When a buffer is busy, the
 * available-list pointers can be used for other purposes.
 * Most drivers use the forward ptr as a link in their I/O
 * active queue.
 * A buffer header contains all the information required
 * to perform I/O.
 * Most of the routines which manipulate these things
 * are in bio.c.
 */
struct buf
{
	short	b_flags;		/* see defines below */
	struct	buf *b_forw;		/* headed by d_tab of conf.c */
	struct	buf *b_back;		/*  "  */
	struct	buf *av_forw;		/* position on free list, */
	struct	buf *av_back;		/*     if not BUSY*/
	dev_t	b_dev;			/* major+minor device name */
	long b_bcount;			/* transfer count */
	union {
	    caddr_t b_addr;		/* low order core address */
	    long *b_words;		/* words for clearing */
	    struct filsys *b_filsys;	/* superblocks */
	    struct dinode *b_dino;	/* ilist */
	    daddr_t *b_daddr;		/* indirect block */
	} b_un;
	struct dkparam
	{
		long dkp_nxadr;		/* address for next command */
		short dkp_nxc1;		/* first cmd word for next command */
		short dkp_nxc2;		/* second cmd word for next command */
		short dkp_nxc3;		/* third cmd word for next command */
		short dkp_nxctl;	/* control word for next command */
		char dkp_sretry;	/* number of soft error retries left */
		char dkp_fretry;	/* number of drive fault retries left*/
	}	b_dkp;		/* disk parameter block */
	daddr_t	b_blkno;		/* block # on device */
	char	b_error;		/* returned after I/O */
	long b_resid;			/* words not transferred after error */
};

extern struct buf buf[];		/* The buffer pool itself */
extern struct buf bfreelist;		/* head of available list */

/*
 * These flags are kept in b_flags.
 */
#define	B_WRITE	0	/* non-read pseudo-flag */
#define	B_READ	01	/* read when I/O occurs */
#define	B_DONE	02	/* transaction finished */
#define	B_ERROR	04	/* transaction aborted */
#define	B_BUSY	010	/* not on av_forw/back list */
#define	B_PHYS	020	/* Physical IO potentially using UNIBUS map */
#define	B_MAP	040	/* This block has the UNIBUS map allocated */
#define	B_WANTED 0100	/* issue wakeup when BUSY goes off */
#define	B_AGE	0200	/* delayed write for correct aging */
#define	B_ASYNC	0400	/* don't wait for I/O completion */
#define	B_DELWRI 01000	/* don't write till block leaves available list */
#define	B_TAPE 02000	/* this is a magtape (no bdwrite) */

/*
 * special redeclarations for
 * the head of the queue per
 * device driver.
 */
#define	b_actf	av_forw
#define	b_actl	av_back
#define	b_active b_bcount
#define	b_errcnt b_resid
