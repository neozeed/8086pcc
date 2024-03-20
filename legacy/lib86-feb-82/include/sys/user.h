/*
 * The user structure.
 * One allocated per process.
 * Contains all per process data
 * that doesn't need to be referenced
 * while the process is swapped.
 * The user block is USIZE*1K bytes
 * long; resides at virtual kernel
 * loc 0xFC000; contains the system
 * stack per user; is cross referenced
 * with the proc structure for the
 * same process.
 */

#define	EXCLOSE	01

struct	user
{
	label_t	u_rsav;			/* save info when exchanging stacks */
	char	u_segflg;		/* IO flag: 0:user D; 1:system; 2:user I */
	char	u_error;		/* return error code */
	short	u_uid;			/* effective user id */
	short	u_gid;			/* effective group id */
	short	u_ruid;			/* real user id */
	short	u_rgid;			/* real group id */
	struct proc *u_procp;		/* pointer to proc structure */
	int	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			int	R_val1;
			int	R_val2;
		} r_val;
#define r_val1	r_val.R_val1
#define r_val2	r_val.R_val2
		off_t	r_off;
		time_t	r_time;
	} u_r;
	caddr_t	u_base;			/* base address for IO */
	unsigned int u_count;		/* bytes remaining for IO */
	off_t	u_offset;		/* offset in file for IO */
	struct inode *u_cdir;		/* pointer to inode of current directory */
	struct inode *u_rdir;		/* root directory of current process */
	char	u_dbuf[DIRSIZ];		/* current pathname component */
	caddr_t	u_dirp;			/* pathname pointer */
	struct direct u_dent;		/* current directory entry */
	struct inode *u_pdir;		/* inode of parent directory of dirp */
	struct file *u_ofile[NOFILE];	/* pointers to file structures of open files */
	char	u_pofile[NOFILE];	/* per-process flags of open files */
	int	u_arg[5];		/* arguments to current system call */
	unsigned u_tsize;		/* text size (clicks) */
	unsigned u_dsize;		/* data size (clicks) */
	unsigned u_ssize;		/* stack size (clicks) */
	label_t	u_qsav;			/* label variable for quits and interrupts */
	label_t	u_ssav;			/* label variable for swapping */
	int	u_signal[NSIG];		/* disposition of signals */
	time_t	u_utime;		/* this process user time */
	time_t	u_stime;		/* this process system time */
	time_t	u_cutime;		/* sum of childs' utimes */
	time_t	u_cstime;		/* sum of childs' stimes */
	int	*u_ar0;			/* address of users saved R0 */
	struct {			/* profile arguments */
		short	*pr_base;	/* buffer base */
		unsigned pr_size;	/* buffer size */
		unsigned pr_off;	/* pc offset */
		unsigned pr_scale;	/* pc scaling */
	} u_prof;
	char	u_intflg;		/* catch intr from sys */
	struct tty *u_ttyp;		/* controlling tty pointer */
	dev_t	u_ttyd;			/* controlling tty dev */
	struct {			/* header of executable file */
		long	ux_mag;		/* magic number */
		long	ux_tsize;	/* text size */
		long	ux_dsize;	/* data size */
		long	ux_bsize;	/* bss size */
		long	ux_ssize;	/* symbol table size */
		long	ux_rtsize;
		long	ux_rdsize;
		long	ux_entloc;	/* entry location */
	} u_exdata;
	char	u_comm[DIRSIZ];
	time_t	u_start;
	char 	u_pnum;			/* process number for use in pagemap */
	char	u_acflag;
	short	u_fpflag;		/* unused now, will be later */
	short	u_cmask;		/* mask for file creation */
	short	u_fcode;		/* function code on bus errors */
	long	u_aaddr;		/* access address on bus errors */
	short	u_ireg;			/* instruction register on bus errors*/
	unsigned u_ptsize;		/* text size (clicks) for sureg */
	unsigned u_pdsize;		/* data size (clicks) for sureg */
	unsigned u_pssize;		/* stack size (clicks)for sureg */
	int	u_xrw;			/* rw flag for sureg */
	int	u_stack[1];
					/* kernel stack per user
					 * extends from u + USIZE*64
					 * backward not to reach here
					 */
};

#define u (*((struct user *)0xFF800))

/* u_error codes */
#define	EPERM	1
#define	ENOENT	2
#define	ESRCH	3
#define	EINTR	4
#define	EIO	5
#define	ENXIO	6
#define	E2BIG	7
#define	ENOEXEC	8
#define	EBADF	9
#define	ECHILD	10
#define	EAGAIN	11
#define	ENOMEM	12
#define	EACCES	13
#define	EFAULT	14
#define	ENOTBLK	15
#define	EBUSY	16
#define	EEXIST	17
#define	EXDEV	18
#define	ENODEV	19
#define	ENOTDIR	20
#define	EISDIR	21
#define	EINVAL	22
#define	ENFILE	23
#define	EMFILE	24
#define	ENOTTY	25
#define	ETXTBSY	26
#define	EFBIG	27
#define	ENOSPC	28
#define	ESPIPE	29
#define	EROFS	30
#define	EMLINK	31
#define	EPIPE	32
#define	EDOM	33
#define	ERANGE	34


/* flags to estabur */

#define RW	0		/* text segment r/w */
#define RO	1		/* text segment r/o */
