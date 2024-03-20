/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 7/10/84 - moved some variables definitions into et_int.c.
					<John Romkey>
   7/16/84 - changed debugging level on short packet message to only
	INFOMSG.			<John Romkey>
*/

#include <netbuf.h>
#include <ether.h>
#include "ether.h"

#define	TRUE	1		/* crock */

/* Process an incoming ethernet packet. Upcall the appropriate protocol
	(Internet, Chaos, PUP, NS, Plummers, ...). This does not check on
	my address and does not support multicast. It may in the future
	attempt to do more of the right thing with broadcast. */

extern unsigned etwpp, etdrop, etmulti;

et_demux() {
	register PACKET p;
	unsigned type;
	register struct ethhdr *pet;

	tk_block();

	while(1) {
	p = (PACKET)aq_deq(et_net->n_inputq);

	if(p == 0) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
		    printf("ETDEMUX: no pkt\n");
#endif
		etwpp++;
		tk_block();
		continue;
		}

	if(p->nb_len < ETMINLEN) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("ETDEMUX: pkt [%u] too small\n", p->nb_len);
#endif
		putfree(p);
		tk_block();
		continue;
		}

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("ET_DEMUX: pkt [%u]\n", p->nb_len);
#endif

	/* Check on what protocol this packet is and upcall it. */
	p->nb_prot = p->nb_buff+sizeof(struct ethhdr);
	pet = (struct ethhdr *)p->nb_buff;
	type = pet->e_type;
	switch(type) {
	case ETIP:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|TRACE))
			printf("ETDEMUX: got ip pkt\n");
#endif
#ifndef	WATCH
		indemux(p, p->nb_len-sizeof(struct ethhdr), et_net);
#endif
		break;
	case ETADR:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|TRACE))
			printf("ETDEMUX: got ARP pkt\n");
#endif
		etarrcv(p, p->nb_len-sizeof(struct ethhdr));
		break;
	default:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("ETDEMUX: bad protocol %u\n", type);
#endif
		etdrop++;
#ifdef	DEBUG
		if(NDEBUG & DUMP) et_dump(p);
#endif
		putfree(p);
	}

	if(et_net->n_inputq->q_head) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("ETDEMUX: More pkts; yielding\n");
#endif
		etmulti++;
		tk_wake(tk_cur);
		}
	tk_block();
	}
}
