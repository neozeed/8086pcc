/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <netbuf.h>
#include "v2.h"

/* Process an incoming packet from the ring. The only protocol supported is
	Internet; I have no expectation for address resolution over V2 Rings
	yet.
*/

#define	TRUE	1

unsigned v2drop = 0;	/* # of packets dropped */
unsigned v2multi = 0;	/* # of times more than one packet on queue */
unsigned v2toosmall = 0;

v2_demux() {
	register PACKET p;
	register struct v2hdr *pv2;
	unsigned len;
	int i;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("V2DEMUX activated.\n");
#endif

	tk_block();

	while(1) {
#ifdef	DEBUG
/*	if(NDEBUG & INFOMSG)
		printf("V2DEMUX running.\n");	*/
#endif

	/* ok, kluge to run without interrupts. Check if there's a packet
		waiting to be copied; if there is then copy it in, re-enable
		receive and process the packet. Yes, we'll probably drop
		a lot of them.
	ACTUALLY, NOW THIS KLUGE IS GONE

	if(inb(mkv2(V2ICSR))&COPYEN) {
		tk_yield();
		continue;
		}

	len = (inb(mkv2(V2ILCNT)) & 0xff) + ((unsigned short)(inb(mkv2(V2IHCNT))&0x07) << 8);

	if(NDEBUG & INFOMSG) {
		printf("lo count byte = %04x, hi byte count = %04x\n",
				inb(mkv2(V2ILCNT)), inb(mkv2(V2IHCNT)));
		printf("V2_DEMUX: received packet of length %u\n", len);
		}

	p = getfree();
	if(p == 0) {
		printf("V2_DEMUX: couldn't get packet buffer!\n");
		outb(mkv2(V2ILCNT), 0);
		outb(mkv2(V2IHCNT), 0);
		outb(mkv2(V2ICSR), MODE2|MODE1|COPYEN);
		tk_yield();
		continue;
		}

	outb(mkv2(V2ILCNT), 0);
	outb(mkv2(V2IHCNT), 0);

	p->nb_len = len;
	for(i=0; i<len; i++)
		p->nb_buff[i] = inb(mkv2(V2IBUF));

	outb(mkv2(V2ILCNT), 0);
	outb(mkv2(V2IHCNT), 0);
	outb(mkv2(V2ICSR), MODE2|MODE1|COPYEN);
*/

	p = (PACKET)aq_deq(v2_net->n_inputq);

	if(p == 0) {
#ifdef	DEBUG
		if(NDEBUG & NETERR|PROTERR|INFOMSG)
		    printf("V2DEMUX: No packet to process!\n");
#endif
		tk_block();
		continue;
		}


	if(p->nb_len < V2MINLEN) {
#ifdef	DEBUG
		if(NDEBUG & (NETERR|PROTERR|INFOMSG))
			printf("V2DEMUX: p[%u ] too small\n", p->nb_len);
#endif
		v2toosmall++;
		putfree(p);
		tk_block();
		continue;
		}

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("V2DEMUX: Processing p[%u]\n", p->nb_len);
#endif

	/* Check on what protocol this packet is and upcall it. */
	p->nb_prot = p->nb_buff+sizeof(struct v2hdr);
	pv2 = (struct v2hdr *)p->nb_buff;

	switch(pv2->v2h_type) {
	case V2IP:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|TRACE))
			printf("V2DEMUX: Received IP packet.\n");
#endif
		indemux(p, p->nb_len-sizeof(struct v2hdr), v2_net);
		break;
	default:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("V2DEMUX: Unknown protocol %U.\n", pv2->v2h_type);
#endif
		v2drop++;
#ifdef	DEBUG
		if(NDEBUG & DUMP) in_dump(p);
#endif
		putfree(p);
	}

	if(v2_net->n_inputq->q_head) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("V2DEMUX: More packets, waking self.\n");
#endif
		v2multi++;
		tk_wake(tk_cur);
		}
	tk_block();
	}

}
