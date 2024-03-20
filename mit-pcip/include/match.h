/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#define	MATCH_DATA_LEN	40

/* pkt data structure */
struct pkt {
	unsigned p_len;
	char	p_dst[6];
	char	p_src[6];
	unsigned p_type;
	char	p_data[MATCH_DATA_LEN];
	};

#define	MAXPKT		512
#define	PKTMASK		511

extern struct pkt pkts[];
extern int prcv, pproc;

extern unsigned start;

/* matching conditions for monitor */

#define	M_ALL	0
#define	M_TYPE	1
#define	M_SRC	2
#define	M_DST	3

extern unsigned match, mtype;
extern char maddr[];
