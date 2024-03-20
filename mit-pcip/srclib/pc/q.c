/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* q.c */

/* General-purpose queue manipulation routines.  Contains the following
 * routines:
 *	q_deq		dequeue and return first element from queue
 *	q_del		delete element from queue
 * All other general-purpose queue manipulation routines are macros
 * defined in "q.h".
 */

#include	<q.h>

#define	TRUE	1
#define	FALSE	0

q_elt q_deq (q)

/* Dequeue and return the first element of the specified queue.  Returns
 * a pointer to the first element if any, or 0 if the queue is empty.
 *
 * Arguments:
 */

register queue	*q;
{
	register q_elt	temp;		/* temp for result */
	
	if ((temp = q->q_head) == 0)	{	/* queue empty? */
		return (0); }	/* yes, show none */

	q->q_head = temp->qe_next;	/* else unlink */
	temp->qe_next = 0;		/* avoid dangling pointers */
	if (q->q_head == 0)		/* queue empty? */
		q->q_tail = 0;	/* yes, update tail pointer too */
	q->q_len--;			/* update queue length */
	if(q->q_len < q->q_min) q->q_min = q->q_len;
	return (temp);
}


q_del (q, elt)

/* Delete the specified element from the queue.  This requires scanning
 * the queue from the top to find and remove the element, so it takes
 * O(queue length) time to execute.  Note that this routine must not
 * run at interrupt level.
 */

register queue	*q;			/* the queue */
register q_elt	elt;			/* element to delete */
{
	register q_elt	*tmp;		/* temp for chaining */
	
	for (tmp = &q->q_head; *tmp != elt;
	     tmp = (q_elt *)(((q_elt)tmp)->qe_next))
		if (*tmp == 0)	/* find ptr. in queue to elt. */
			return FALSE;		/* if not in queue, punt */
			
	*tmp = (*tmp)->qe_next;		/* else link it out of the queue */

	if (q->q_tail == elt) {		/* at end of queue? */
		if (tmp == &q->q_head)	/* yes; if first elt, zero out tail */
			q->q_tail = 0;
		else			/* otherwise tail is previous elt */
			q->q_tail = (q_elt)tmp;
	}
	elt->qe_next = 0;		/* avoid dangling pointers */
	q->q_len--;			/* update element count */
	if(q->q_len < q->q_min) q->q_min = q->q_len;
	return TRUE;
}

q_elt aq_deq(q)
	queue *q; {
	register q_elt temp;

	int_off();
	temp = q_deq(q);
	int_on();
	return temp;	}

#define	NULL	0

queue *q_create() {
	register queue *q;

	q = (queue *)calloc(1, sizeof(queue));
	if(q == NULL) return q;

	q->q_head = NULL;
	q->q_tail = NULL;
	q->q_len = q->q_max = q->q_min = 0;
	return q;
	}
