/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* declare the msdos 2.0 devive structure */

struct dev {
	long	d_link;
	unsigned d_attrib;
	int	(*d_strat)();
	int	(*d_int)();
	char	d_unit[8];
	};

#define	CHAR	0x0000
#define	BLOCK	0x8000
#define	IOCTL	0x4000
#define	IBM	0x0000
#define	NONIBM	0x2000
#define	CLOCK	0x0008
#define	NUL	0x0004
#define	STDOUT	0x0002
#define	STDIN	0x0001

struct BPB {
	unsigned b_secsize;	/* sector size in bytes */
	char	b_spalu;	/* sectors per allocation unit */
	unsigned b_rsvdsecs;	/* # of reserved sectors */
	char	b_nfats;	/* number of FATS */
	unsigned b_ndirent;	/* # of directory entries */
	unsigned b_nsecs;	/* total # of sectors */
	char	b_media;	/* media descriptor */
	unsigned b_fatlen;	/* # of bytes in FAT */
	};


struct BOOT {
	char	B_jmp[3];
	char	B_oem[8];
	struct BPB B_BPB;
	unsigned B_spt;		/* # of sectors per track */
	unsigned B_nheads;	/* # of heads */
	unsigned B_nhidsecs;	/* # of hidden sectors */
	};

/* operations */

#define	INIT		0
#define	MEDIACHK	1
#define	BUILDBPB	2
#define	IOCTLIN		3
#define	INPUT		4
#define	NONINPUT	5
#define	INSTAT		6
#define	INFLUSH		7
#define	OUTPUT		8
#define	OUTVER		9
#define	OUTSTAT		10
#define	OUTFLUSH	11
#define	IOCTLOUT	12

/*	struct req {
	char	s_len;		/* length of req 
	char	s_unit;		/* unit subcode
	char	s_command;	/* command 
	unsigned s_status;	/* status 
	char	s_rsvd[8];
	union {
		struct rdwr {
			char	r_media;
			unsigned r_off;
			unsigned r_seg;
			unsigned r_nsecs;
			unsigned r_startsec;
			} rdwr;
		struct media {
			char	m_desc;
			char	m_ret;
			} media;
		struct build {
			char	bd_media;
			long	bd_temp;
			unsigned bd_bpboff;
			unsigned bd_bpbseg;
			} build;
		struct init {
			char	i_nunits;
			unsigned i_lstoff;
			unsigned i_lstseg;
			unsigned i_bpboff;
			unsigned i_bpbseg;
			long	i_init;
			} init;
		} spc;
	};
*/

#define	NOERR		300
#define	BUSY		400
#define	WRITEPROT	0x00
#define	BADUNIT		0x01
#define	NOTREADY	0x02
#define	BADCMD		0x03
#define	CRCERR		0x04
#define	BADLEN		0x05
#define	SEEKERR		0x06
#define	UNKNMEDIA	0x07
#define	SECNOTFND	0x08
#define	NOPAPER		0x09
#define	WRFAULT		0x0A
#define	RDFAULT		0x0B
#define	GENFAIL		0x0C
