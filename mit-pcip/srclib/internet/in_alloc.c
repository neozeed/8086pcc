/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>

/* Allocate and internet packet. Has to grunge around with local net header
	sizes to do the right thing. */

PACKET in_alloc(datalen, optlen)
	int optlen, datalen; {
	register PACKET p;
	register struct ip *pip;
	int len;

	optlen = (optlen + 3) & ~3;
	len = (IPHSIZ + optlen + datalen + 1) & ~1;

	if(datalen > INETLEN) {
#ifdef	DEBUG
		printf("IN_ALLOC: Packet size %u is too large.\n",datalen);
#endif
		return nullbuf;
		}

	p = a_getfree();
	if(p == nullbuf) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR))
		printf("IN_ALLOC: Couldn't allocate packet.\n");
#endif
		return nullbuf;
		}

	p->nb_prot = p->nb_buff + MaxLnh;	/* Generalized packet */
	pip = in_head(p);
	pip->ip_ihl = IP_IHL + (optlen >> 2);
	return p;
	}


/* Free up an internet packet */

extern PACKET buffers[];

in_free(buffer)
	PACKET buffer; {
	register PACKET p;
	int i;

	/* check if packet link is not zero */
	if(buffer->nb_elt)
		printf("in_free: packet buffer %04x is already in a queue\n",
								buffer);

	/* check if its one of the allocated packets */
	for(i=0; i<NBUF; i++)
		if(buffers[i] == buffer) goto okay;

	printf("in_free: bad packet buffer %04x\n", buffer);
	return;

okay:	/* check if the packet is already in a queue */
	for(p=(PACKET)freeq.q_head; p; p = (PACKET)p->nb_elt)
		if(p == buffer) {
			printf("buffer already enqueued in freeq\n");
			return;
			}

	putfree(buffer);
	}
