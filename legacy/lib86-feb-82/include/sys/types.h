typedef	long		daddr_t;
typedef char *		caddr_t;
typedef	long		mem_t;
typedef	unsigned short	ino_t;
typedef	long		time_t;
typedef	long		label_t[13];	/* regs d2-d7, a2-a7, pc */
typedef	short		dev_t;
typedef	long		off_t;

	/* selectors and constructor for device code */
#define	major(x)  	(int)(((unsigned)x>>8))
#define	minor(x)  	(int)(x&0377)
#define	makedev(x,y)	(dev_t)((x)<<8|(y))
