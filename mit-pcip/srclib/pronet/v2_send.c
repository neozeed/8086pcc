/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <netbuf.h>
#include <stdio.h>
#include "v2.h"

/* this file contains the function which sends packets out over the v2 lni.
	For now it has a timeout function so that if the packet isn't sent
	within a reasonable length of time we'll notice and punt.
*/

#define	V2TIMEOUT	10

unsigned v2tmo = 0;
unsigned v2tx = 0;

extern long cticks;

v2_send(p, prot, len, fhost)
	PACKET p;
	unsigned prot;
	unsigned len;
	in_name fhost; {
	struct v2hdr *pv2;
	union {
		long _l;
		char _c[4];
		} foo;
	unsigned i;
	long time;
	unsigned stat;
	register char *data;

	/* Set up the ethernet header. Insert our address and the address of
		the destination and the type field in the ethernet header
		of the packet. */
#ifdef	DEBUG
	if(NDEBUG & (INFOMSG|TRACE))
		printf("V2_SEND: p[%u] -> %a\n", len, fhost);
#endif

	pv2 = (struct v2hdr *)p->nb_buff;

	/* Setup the type field and the addresses in the v2lni header. */
	if(prot != IP) {
		printf("V2_SEND: Bad packet type %u\n", prot);
		exit(1);
		}

	pv2->v2h_type = V2IP;
	foo._l = fhost;
	pv2->v2h_dst = foo._c[3]; 
	if(NDEBUG & INFOMSG)
		printf("V2_SEND: foreign host addr %u\n", pv2->v2h_dst);

	/* Now to send the packet. Copy the packet into the packet buffer,
		starting at the address -len. Then set the count to -len
		again. Then write 0x11 to the output CSR to send then
		packet, and check the status bits afterwards.
	*/
	v2tx++;
	len += sizeof(struct v2hdr);

	if(len&1) len++;

	if(NDEBUG & INFOMSG)
		printf("length  = %04x, -length = %04x\n", len, -len);
	outb(mkv2(V2OLCNT), (-len)&0xff);
	if(NDEBUG & INFOMSG)
		printf("lo byte count = %04x\n", inb(mkv2(V2OLCNT)));
	outb(mkv2(V2OHCNT), ((-len) >> 8) & 0x07);
	if(NDEBUG & INFOMSG)
		printf("hi byte count = %04x\n", inb(mkv2(V2OHCNT)));

	data = p->nb_buff;

	for(i=0; i<len; i++) outb(mkv2(V2OBUF), *data++);

	outb(mkv2(V2OLCNT), (-len)&0xff);
	outb(mkv2(V2OHCNT), ((-len) >> 8) & 0x07);
	outb(mkv2(V2OCSR), ORIGEN|INITRING);

	if(NDEBUG & INFOMSG)
		printf("packet send started\n");

	while(inb(mkv2(V2OCSR)) & ORIGEN) ;

	stat = inb(mkv2(V2OCSR));

	if((stat & REFUSED) && (NDEBUG & INFOMSG)) {
		printf("V2_SEND: refused\n");
		return 0;
		}

	if(NDEBUG & INFOMSG)
		printf("V2_SEND: output csr was %04x\n", stat);

	return len;
	}

/*
	switch(v2txstate) {
	case TIMEOUT:
#ifdef	DEBUG
		if(NDEBUG & NETERR|PROTERR)
			printf("V2_SEND: Timed out on send.\n");
#endif
		v2txstate = NONBUSY;
		return 0;
	case NONBUSY:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("V2_SEND: Done sending packet.\n");
#endif
		return len-sizeof(struct v2hdr);
	default:
#ifdef	DEBUG
		printf("V2_SEND: Invalid state: %u.\n", v2txstate);
#endif
		return 0;
		}
	}
*/
