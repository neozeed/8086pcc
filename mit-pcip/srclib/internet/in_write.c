/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 8/23/84 - changed debugging messages.
						<John Romkey>
*/

#include <ip.h>
#include "ipconn.h"

unsigned ipsnd = 0;

/* Fill in the internet header in the packet p and send the packet through the
	appropriate net interface. This will involve using routing. Packets for
	a certain connection are all routed at connection open time, but some
	facility should be provided to allow for later rerouting. */

static unsigned uid=1;

in_write(conn, p, datalen, fhost)
	register IPCONN conn;
	PACKET p;
	int datalen;
	in_name fhost; {
	in_name firsthop;
	register struct ip *pip;
	int len;
	
	if(datalen > LBUF) {
#ifdef	DEBUG
		printf("IN_WRITE: bad pkt len %u", datalen);
		if(NDEBUG & BUGHALT) exit(1);
#endif
		return -1;
		}

#ifdef	DEBUG
	if(NDEBUG & INTRACE)
		printf("in_write: pkt [%u] prot %u to %a\n", datalen,
							conn->c_prot, fhost);
#endif

	/* perform routing. Have to route on each and every packet going
		out because have to find first hop. */
	conn->c_net = inroute(fhost, &firsthop);
	if(conn->c_net == 0) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("in_write: can't route to %a\n", fhost);
#endif
		return 0;
		}

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("ip: pkt for %a routed to %a\n", fhost, firsthop);
#endif

	pip = in_head(p);
	pip->ip_ver = IP_VER;
	pip->ip_time = IP_TIME;

	pip->ip_flgs = IP_FLGS;
	pip->ip_foff = IP_FOFF;

	pip->ip_id = bswap(uid++);

	pip->ip_chksum = IPXSUM;
	pip->ip_src = conn->c_net->ip_addr;
	pip->ip_dest = fhost;

	len = (pip->ip_ihl <<2) + datalen;

	pip->ip_len = bswap(len);

	pip->ip_tsrv = 0;
	pip->ip_prot = conn->c_prot;

	pip->ip_chksum = ~cksum(pip, pip->ip_ihl << 1);

	ipsnd++;
	return (*conn->c_net->n_send)(p, IP, len, firsthop);
	}

