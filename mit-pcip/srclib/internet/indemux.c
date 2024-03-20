/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 7/9/84 - added crock to accept 4.2 broadcast packets. Probably should
	have a per net broadcast address in the net structure.
					<John Romkey>
*/

#include <ip.h>
#include "ipconn.h"

/* internet statistics */
unsigned ipdrop = 0;		/* ip packets dropped */
unsigned ipxsum = 0;		/* ip packets with bad checksums */
unsigned iplen = 0;		/* ip packets with bad lengths */
unsigned ipdest = 0;		/* ip packets with bad destinations */
unsigned ipttl = 0;		/* ip packets with time to live = 0 */
unsigned ipprot = 0;		/* no server for protocol */
unsigned ipver = 0;		/* bad ip version number */
unsigned iprcv = 0;		/* number of ip packets received */
unsigned ipfrag = 0;		/* number of fragments received */

/* This is the internet demultiplexing routine. It handles packets received by
	the per-net task, verifies their headers and does the upcall to
	the whoever should receive the packet. All the guts of demultiplexing
	is in this piece of code. If the packet doesn't belong to anyone,
	this gets logged and the packet dropped.	*/

indemux(p, len, nt)
	PACKET p;
	int len;
	NET *nt; {
	register struct ip *pip;	/* the internet header */
	register IPCONN conn;		/* an internet connection */
	int prot;			/* packet protocol */
	int i;
	unsigned csum;			/* packet checksum */
	char *pdata;

	iprcv++;

	pip = in_head(p);
	if(len < bswap(pip->ip_len)) {
#ifdef	DEBUG
		if(NDEBUG & PROTERR)
			printf("indemux: bad pkt len\n");
		if(NDEBUG & DUMP) in_dump(p);
#endif
		iplen++;
		ipdrop++;
		in_free(p);
		return;	}

	len = bswap(pip->ip_len);

	if(pip->ip_ver != IP_VER) {
#ifdef	DEBUG
		if(NDEBUG & PROTERR)
		    printf("indemux: bad version number\n");
		if(NDEBUG & DUMP) in_dump(p);
#endif
		ipver++;
		ipdrop++;
		in_free(p);
		return;	}

	csum = pip->ip_chksum;
	pip->ip_chksum = 0;
	if(csum != ~cksum(pip, pip->ip_ihl << 1)) {
		pip->ip_chksum = csum;
#ifdef
		if(NDEBUG & PROTERR)
			printf("indemux: bad xsum\n");
		if(NDEBUG & DUMP) in_dump(p);
#endif
		ipxsum++;
		ipdrop++;
		in_free(p);
		return;
		}

	pip->ip_chksum = csum;

	/* silly crock for catching 4.2 broadcast packets
		doesn't work correctly with subnet routing!
	*/
	if(pip->ip_dest != nt->ip_addr && (pip->ip_dest&0xff000000) != 0) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("indemux: got pkt not for me\n");
#endif
		ipdrop++;
		in_free(p);
		return;
		}

	/* Woe, oh silly crock */
	*(((unsigned *)&pip->ip_id)+1)=bswap(*(((unsigned *)&pip->ip_id)+1));

	if((pip->ip_foff != 0) || (pip->ip_flgs & 1)) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("indemux: got a frag\n");
		if(NDEBUG & DUMP) in_dump(p);
#endif
		ipfrag++;
		ipdrop++;
		in_free(p);
		return;
		}

	/* The packet is now verified; the header is correct. Now we have
		to demultiplex it among our internet connections. */

	prot = pip->ip_prot;
#ifdef	DEBUG
	if(NDEBUG & INTRACE)
		printf("ipdemux: pkt len %u prot %u\n", len-IPLEN, prot);
#endif

	for(i=0; i<nipconns; i++) {
		conn = ipconn[i];
		if(conn->c_prot == prot) 
			if(conn->c_handle == 0) break;
			else {
				conn->c_handle(p, len-IPLEN, pip->ip_src);
				return;	}
		}

	/* If we get here, we didn't manage to demultiplex the packet.
		We should drop it and go away. */
#ifdef	DEBUG
	if(NDEBUG & PROTERR)
		printf("ipdemux: unhandled prot %u\n", prot);
#endif

	icmp_destun(pip->ip_src, pip, DSTPROT);

	ipprot++;
	ipdrop++;
	in_free(p);
	return;
	}


/* pretty print the statistics */
extern unsigned ipsnd;		/* # packets sent */

in_stats(fd)
	FILE *fd; {

	fprintf(fd, "IP Stats ");
	fprintf(fd, "%4u pkts rcvd\t%4u pkts sent\n", iprcv, ipsnd);
	fprintf(fd, "%4u pkts dropped\n", ipdrop);
	fprintf(fd, "%4u bad xsums\t%4u bad protocols\n", ipxsum, ipprot);
	fprintf(fd, "%4u bad vers\t%4u bad lens\n", ipver, iplen);
	fprintf(fd, "%4u ttl expired\t%4u frags\n", ipttl, ipfrag);
	}
