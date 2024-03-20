/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>
#include "internal.h"

/* This routine handles incoming UDP packets. They're handed to it by the
	internet layer. It demultiplexes the incoming packet based on the
	local port and upcalls the appropriate routine. */

/* 7/3/84 - fixed changed checksum computation to use length from UDP
	header instead of length passed up from internet.
						<John Romkey>
   7/12/84 - "fixed" the demultiplexor to not send destination
	unreachables in response to packets sent to the 4.2 ip
	broadcast address.			<John Romkey>
*/

extern UDPCONN firstudp;

udpdemux(p, len, host)
	PACKET p;
	int len;
	in_name host;	{
	struct ip *pip;
	register struct udp *pup;
	struct ph php;
	register UDPCONN con;
	unsigned osum, xsum;
	char *data;
	unsigned plen;

	/* First let's verify that it's a valid UDP packet. */
#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("UDP: pkt len %u from %a\n", len, host);
#endif

	pip = in_head(p);
	pup = udp_head(pip);
	plen = bswap(pup->ud_len);

	if(plen > len) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("UDP: bad len pkt: rcvd: %u, hdr: %u.\n",
					len, bswap(pup->ud_len) + UDPLEN);
#endif

		in_free(p);
		return;
		}

	osum = pup->ud_cksum;
	if(osum) {
		if(len & 1) ((char *)pup)[plen] = 0;
		php.ph_src = host;
		php.ph_dest = pip->ip_dest;
		php.ph_zero = 0;
		php.ph_prot = UDPPROT;
		php.ph_len  = pup->ud_len;
	
		pup->ud_cksum = cksum(&php, sizeof(struct ph)>>1);
		xsum = ~cksum(pup, (plen+1)>>1);
		pup->ud_cksum = osum;
		if(xsum != osum) {
#ifdef	DEBUG
			if(NDEBUG & (INFOMSG|PROTERR))
			printf("UDPDEMUX: bad xsum %04x right %04x from %a\n",
							osum, xsum, host);
			if(NDEBUG & DUMP)
				in_dump(p);
#endif
			in_free(p);
			return;
			}
		}

	udpswap(pup);

#ifdef	DEBUG
	if(NDEBUG & TCTRACE)
		printf("UDP: pkt to port %04x from port %04x, host %a\n",
					pup->ud_dstp, pup->ud_srcp, host);
#endif
		
	/* ok, accept it. run through the demux table and try to upcall it */

	for(con = firstudp; con; con = con->u_next) {
		if(con->u_lport && con->u_lport != pup->ud_dstp) continue;

		if(con->u_rcv)
			(*con->u_rcv)(p, plen-UDPLEN, host, con->u_data);
		return;
		}

	/* what a crock. check if the packet was sent to the (4.2) ip
		broadcast address. If it was, don't print a message or
		send a destination unreachable.
	*/

	if(pip->ip_src & 0xff000000) {
		udp_free(p);
		return;
		}

#ifdef	DEBUG
	if(NDEBUG & (INFOMSG|PROTERR))
		printf("UDP: nobody on port %04x\n", pup->ud_dstp);
	if(NDEBUG & DUMP)
		in_dump(p);
#endif

	/* send destination unreachable */
	icmp_destun(host, in_head(p), DSTPORT);

	udp_free(p);
	return;
	}
