/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* Some UDP internals */

struct ph {
	in_name ph_src;		/* source address */
	in_name ph_dest;	/* dest address */
	char	ph_zero;	/* zero (reserved) */
	char	ph_prot;	/* protocol */
	unshort	ph_len;	};	/* udp length */

extern IPCONN udp;		/* The UDP Internet connection */

