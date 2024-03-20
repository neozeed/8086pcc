/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>

/* Return the address of our machine relative to a certain foreign host. */

in_name in_mymach(host)
	in_name host; {
	in_name temp;
	NET *tnet;

	tnet = inroute(host, &temp);
	if(tnet == 0) {
#ifdef	DEBUG
		if(NDEBUG & (PROTERR|INFOMSG))
			printf("IP: Couldn't route to %a\n", host);
#endif
		return 0L;
		}

	return tnet->ip_addr;
	}
