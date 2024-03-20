/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* redirect table definitions */

struct redent {
	in_name	rd_dest;
	in_name	rd_to;
	};

#define	REDIRTABLEN	16

extern struct redent redtab[REDIRTABLEN];
extern int rednext;
