/*  Copyright 1983 by the Massachusetts Institute of Technology  */


#include <q.h>

extern queue freeq;		/* The queue of unused packets */

/* Getting packets from the queue of received packets is an operation that is
	used at non-interrupt level, but the queue is modified at interrupt,
	so the operation must be atomic. On the other hand, adding a packet to
	this queue is only done at interrupt level, and thus cannot be further
	interrupted. This operation, then, does not need to be atomic.
	   However, both of the free queue operations need to be atomic because
	packets	can be allocated from interrupt level or from high level
	routines. But the interrupt level routines shouldn't have interrupts
	reenabled accidentally, so they have to have a nonatomic operation for
	getting a packet. Hence GETFREE and A_GETFREE. A_GETFREE should be used
	from high level routines; GETFREE from interrupt level routines. */


#define	putfree(p)	{ p->nb_prot = p->nb_buff+MaxLnh; \
				aq_addt(&freeq, (q_elt)p); }

#define	getfree()	(PACKET)q_deq(&freeq)

#define	a_getfree()	(PACKET)aq_deq(&freeq)
