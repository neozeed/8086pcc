/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 8/23/84 - changed debugging messages slightly.
						<John Romkey>
*/

#include <udp.h>
#include "internal.h"

/* Fill in the udp header on a packet, checksum it and pass it to
	Internet. */


udp_write(u, p, len)
	UDPCONN u;
	PACKET p;
	int len; {
	register struct udp *pup;
	register struct ph php;
	int udplen;
	in_name host;

	host = u->u_fhost;

#ifdef	DEBUG
	if(NDEBUG & TCTRACE)
	    printf("UDP: pkt [%u] from %04x to %04x on %a\n", len, u->u_lport,
						u->u_fport, host);
#endif

	pup = udp_head(in_head(p));
	udplen = len + sizeof(struct udp);
	if(udplen & 1) ((char *)pup)[udplen] = 0;

	pup->ud_len = udplen;
	pup->ud_srcp = u->u_lport;
	pup->ud_dstp = u->u_fport;
	udpswap(pup);

	php.ph_src = in_mymach(host);
	php.ph_dest = host;
	php.ph_zero = 0;
	php.ph_prot = UDPPROT;
	php.ph_len = pup->ud_len;
	pup->ud_cksum = cksum(&php, sizeof(struct ph)>>1);
	pup->ud_cksum = ~cksum(pup, (udplen+1)>>1);

	return in_write(udp, p, udplen, host);
	}
