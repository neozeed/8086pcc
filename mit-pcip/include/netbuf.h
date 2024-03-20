/*  Copyright 1983 by the Massachusetts Institute of Technology  */

#include <net.h>

/* The buffer organization is somewhat intertwined with the queue organization.
	Each buffer is in a queue, either the free queue or the used queue
	(or the buffer is currently being used by a user program or the
	interrupt level routines, in which case it does not appear in a
	queue). When a buffer is in a queue, it has a pointer to the next
	buffer in the queue. If there is no next buffer, its pointer points
	at nullbuf. Also, each buffer knows its own length. */

/* This include file gives the structure of buffers. */

#define	NBUFINIT	10			/* A Goodly Number */
#define	LBUFINIT	620			/* A Goodly Length */
#define nullbuf	(struct net_buf *)0	/* The null buffer and/or queue
						entry */
extern int NBUF;			/* # of packet buffers */
extern int LBUF;			/* length of each packet buffers */

struct net_buf {
	q_elt	nb_elt;		/* queue link */
	char	*nb_prot;	/* beginning of contents of packet */
	long	nb_tstamp;		/* packet timestamp */
	unsigned nb_len;			/* Length of buffer */
	char	*nb_buff;		/* The buffer itself! */
	};

typedef struct net_buf *PACKET;


