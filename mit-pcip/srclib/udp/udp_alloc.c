/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>

PACKET udp_alloc(datalen, optlen)
	int datalen;
	int optlen; {
	int len;

	len = (datalen + sizeof(struct udp) + 1) & ~1;
	return in_alloc(len, optlen);
	}
