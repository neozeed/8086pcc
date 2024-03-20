/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* timer.h */


/* This file contains the declarations for the timer management package. */

#define	TPS	 18			/*  Clock  ticks per second.  */

typedef	unsigned	nonce;		/* a timeout nonce */

typedef	struct	timer	{		/* a timer */
	struct	q_elt	tm_elt;		/* next element in lwa-queue */
	long		tm_time;	/* time the timer expires */
	nonce		tm_nonce;	/* nonce for this timer */
	int		(*tm_subr) ();	/* subroutine to call on timeout */
	char		*tm_arg;	/* arg to pass to subr */
} timer;

typedef	struct	time_q	{		/* the lwa-queue of active timers */
	timer	*tmq_head;		/* head of the timer lwa-queue */
	timer	*tmq_tail;		/* tail of the timer lwa-queue */
	int	tmq_len;		/* number of elements in lwa-queue */
	unsigned tmq_max;
	unsigned tmq_min;
	} time_q;


timer *tm_alloc();

extern unsigned TIMERDEBUG;

