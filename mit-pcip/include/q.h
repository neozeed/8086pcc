/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* Definitions for general-purpose queue manipulation package.
   Modified from Larry Allen's queue package from CSR Unix for TCP and
   tasks. */


typedef	struct	q_elt	{		/* queue element: cast to right type */
	struct	q_elt	*qe_next;	/* it's just a pointer to next elt */
} *q_elt;

typedef	struct	queue	{		/* queue header */
	q_elt	q_head;			/* first element in queue */
	q_elt	q_tail;			/* last element in queue */
	int	q_len;			/* number of elements in queue */
	int	q_max;			/* maximum length */
	int	q_min;			/* minimum length */
} queue;

extern	q_elt	q_deq ();
extern queue *q_create();

/* The following macros implement most of the common queue operations */

/* Add an element to the head of the queue */

#define	q_addh(q, elt) 	{ \
	if ((q)->q_head == 0) (q)->q_tail = (elt); \
	(elt)->qe_next = (q)->q_head; \
	(q)->q_head = (elt); \
	(q)->q_len++; \
	if(++((q)->q_len) > (q)->q_max) (q)->q_max = (q)->q_len; \
}

/* Add an element to the tail of a queue */

#define	q_addt(q, elt)	{ \
	(elt)->qe_next = 0; \
	if ((q)->q_head == 0) { \
		(q)->q_head = (elt); \
	} else { \
		(q)->q_tail->qe_next = (elt); \
	} \
	(q)->q_tail = (elt); \
	if(++((q)->q_len) > (q)->q_max) (q)->q_max = (q)->q_len; \
}

/* Add an element after a specified element in the queue.  If prev == */
/* &q->q_head, can be used to add an element to the head of the queue */

#define	q_adda(q, prev, new)	{ \
	if ((q)->q_tail == (prev) || (q)->q_tail == 0) { \
		(q)->q_tail = (new); \
	} \
	(new)->qe_next = (prev)->qe_next; \
	(prev)->qe_next = (new); \
	if(++((q)->q_len) > (q)->q_max) (q)->q_max = (q)->q_len; \
}

/* Delete an element from a queue, given a pointer to the preceeding element */
/* Will delete the first element if prev == &q->q_head */

#define	q_dela(q, elt, prev)	{ \
	if ((q)->q_tail == (elt)) { \
		if ((q)->q_head == (elt)) \
			(q)->q_tail = 0; \
		else \
			(q)->q_tail = (prev); \
	} \
	(prev)->qe_next = (elt)->qe_next; \
	(elt)->qe_next = 0; \
	if(--((q)->q_len) < (q)->q_min) (q)->q_min = (q)->q_len; \
}


#define	aq_addt(q, elt)	{int_off(); q_addt((q), (elt)); int_on(); }

#define	aq_addh(q, elt)	{int_off(); q_addh((q), (elt)); int_on(); }
