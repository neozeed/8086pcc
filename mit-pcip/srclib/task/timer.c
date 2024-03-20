/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* This file contains the routines which make up the timer task and
 * its associated subroutines for setting and clearing timers.  The
 * timer task manages a queue of timers, each of which specifies a
 * subroutine to be called (in the context of the timer task) when the
 * timer goes off.
 * The following routines are included in this package:
 *	tm_set		set a timer to fire after number of seconds
 *	tm_mset		set a timer, argument in milliseconds
 *	tm_tset		set a timer, argument in clock ticks
 *	tm_reset	reset a timer to go off at a different time
 *	tm_clear	clear a previously set timer
 *	tm_main		the main routine of the timer task
 *	tm_init		init the timer system
 *	tm_off		turn off timer interrupts
 *	tm_on		turn timer interrupts back on
 */
/*  Addition of tm_tset and tm_mset, 12/83. <J.H. Saltzer>  */

#include	<stdio.h>
#include	<signal.h>
#include	<q.h>
#include	<task.h>
#include	<timer.h>

#define	TIMERHIWATER	30		/* number of free timers to keep */
#define	TIMERSTACK	500		/* only need a small stack */

static	task	*tm_task;		/* timer task's handle */
static	time_q	tm_queue;		/* queue of active timers */
static	time_q	freetmq;		/* queue of free timers */
static	nonce	tnonce;			/* timer nonce generator */
extern	int	tm_signal ();		/* alarm signal catching routine */
extern long cticks;
extern unsigned _tcount;

unsigned TIMERDEBUG = 0;

/* set timer in seconds  */

tm_set(nsecs, subr, arg, tm)
register int	nsecs;			/* timer expiration time, in secs */
int	(*subr)();			/* subroutine to call on expiration */
char	*arg;				/* arg to pass to subr. */
register timer	*tm;			/* place to return timer id */
{	tm_tset (nsecs*TPS, subr, arg, tm) ; }

/* set timer in milliseconds  */

tm_mset(msecs, subr, arg, tm)
long	msecs;				/* timer expiration time, in msecs */
int	(*subr)();			/* subroutine to call on expiration */
char	*arg;				/* arg to pass to subr. */
register timer	*tm;			/* place to return timer id */
{	tm_tset ((int)((msecs*TPS)/1000), subr, arg, tm) ; }


/* Set a timer to go off after nticks clock ticks.  When the timer goes
 * off, call the specified subroutine with the specified argument.
 * flag.  This routine runs in the context of the caller's task;
 * it just enqueues the timer and, if it is the first timer on the
 * queue, sets an alarm to awaken the timer task.
 */

tm_tset (nticks, subr, arg, tm)
register int	nticks;		/* timer expiration time */
int	(*subr)();		/* subroutine to call on expiration */
char	*arg;			/* arg to pass to subr. */
register timer	*tm;		/* place to return timer id */
	{
	register timer	**tmp;		/* temp for chaining */
	queue *qp;

#ifdef	DEBUG
	if(TIMERDEBUG) {
	printf("TM_SET:  setting timer %04x for %u ticks\n", tm, nticks);
			}
#endif

	q_del (&tm_queue, tm);		/*  make sure not already queued. */
	tm->tm_elt.qe_next = NULL;	/* no next element */
	tm->tm_time = cticks + nticks;	/* timer expiration time */
	tm->tm_nonce = ++tnonce;	/* nonce for this timer */
	tm->tm_subr = subr;		/* subroutine to call */
	tm->tm_arg = arg;		/* argument to pass */

	for (tmp = &tm_queue.tmq_head;	/* add to queue in timeout order */
	     *tmp != NULL && tm->tm_time >= (*tmp)->tm_time;
	     tmp = (timer **)(((timer *)tmp)->tm_elt.qe_next)) ;

	qp = (queue *)&tm_queue;
	q_adda(qp, (q_elt)tmp, (q_elt)tm);
	if (tm_queue.tmq_head == tm) 	/* first elt in queue? */
		alarm (nticks);		/* yes, wake up timer task then */

}	/*  end of tm_tset()  */


/* Reset a (running) timer to go off in nsecs seconds
 * instead of at the time it is currently set for.  If in fact the
 * timer is not already set, return FALSE; otherwise return TRUE.
 * Does not modify the upcall in the timer.
 */

tm_reset (nsecs, tm)

int	nsecs;				/* timer expiration time in seconds */
register timer	*tm;			/* timer id to reset*/
{
	register timer	**tmp;		/* temp for chaining */
	queue *qp;
	
	if (tm->tm_nonce == 0 || !q_del (&tm_queue, tm)) {
#ifdef	DEBUG
		if(TIMERDEBUG)
			printf("TIMER_RESET: timer already expired.\n");
#endif
		return (FALSE);		/* timer expired, give up */
		}

#ifdef	DEBUG
	if(TIMERDEBUG)
		printf("TIMER_RESET: timer reset for %u seconds.\n", nsecs);
#endif

	
	tm->tm_elt.qe_next = NULL;	/* no next element */
	tm->tm_time = cticks + nsecs*TPS;	/* timer expiration time */
	
	for (tmp = &tm_queue.tmq_head;	/* add to queue in timeout order */
	     *tmp != NULL && tm->tm_time >= (*tmp)->tm_time;
	     tmp = (timer **)(((timer *)tmp)->tm_elt.qe_next)) ;
	qp = (queue *)&tm_queue;
	q_adda(qp, (q_elt)tmp, (q_elt)tm);
	
	if (tm_queue.tmq_head == tm)	/* first elt in queue? */
		alarm (nsecs*TPS);	/* yes, wake up timer task then */

	return (TRUE);
}	/*  end of tm_reset()  */


/* Clear the timer specified by the passed timer identifier.  The timer
 * identifier gives a pointer to the timer to be cleared.
 * Free the timer's storage
 * (into the free list up to TIMERHIWATER elements).
 * If it was the only timer on the queue, reset the pending alarm.
 * Returns FALSE if the specified alarm was not found in the queue
 * TRUE otherwise.
 */

tm_clear(tm)

register timer *tm;			/* identifier of timer to clear */
{
	
	if (tm->tm_nonce == 0) {
#ifdef	DEBUG
		if(TIMERDEBUG)
		printf("TIMERCLEAR: timer %04x already expired.\n", tm);
#endif
		return FALSE;
		}

#ifdef	DEBUG
	if(TIMERDEBUG)
		printf("TIMERCLEAR: clearing timer %04x\n", tm);
#endif

	tm->tm_nonce = 0;
	if(!q_del (&tm_queue, tm)) {
		return (FALSE);		/* timer expired, give up */
		}
	
	if (tm_queue.tmq_head == NULL)	/* last elt on queue? */
		alarm (-1);		/* yes, turn off alarm */
		
	return (TRUE);			/* success */

}	/*  end of tm_clear();  */


/* This routine forms the body of the timer management task.  Its
 * job is simply to dequeue expired timers from the timer queue
 * and call the subroutine specified therein, passing it the argument
 * specified therein.  This task is normally blocked on
 * the alarm signal.  It is created by the tm_init() routine.
 * Note that this task needs the alarm signal, so all timer
 * management in the process must be via this task - no one else
 * may use the alarm() calls!
 */

tm_main ()

{
	register timer	*tm;		/* temp for holding timer */
	queue *qp;
	
	for (;;) {			/* never exit */

#ifdef	DEBUG
		if(TIMERDEBUG)
			printf("Timer task running.\n");
#endif


		while ((tm = tm_queue.tmq_head) != NULL &&
			cticks >= tm->tm_time) { /* for all expired timers */

			tm = (timer *)q_deq (&tm_queue);   /* dequeue it */

			/* if the timer is expired, ignore it */
			if(tm->tm_nonce == 0){
#ifdef	DEBUG
				if(TIMERDEBUG)
			printf("TIMER: ignoring fired timer %04x.\n", tm);
#endif
					 continue;}

#ifdef	DEBUG
			if(TIMERDEBUG)
				printf("TIMER: timer %04x has fired.\n", tm);
#endif

			tm->tm_nonce = 0; /* show timer expired... */
			(*tm->tm_subr) (tm->tm_arg); /* call the routine */

		}
		
		if ((tm = tm_queue.tmq_head) != NULL) /* outstanding timers? */
			alarm ((int)(tm->tm_time - cticks)); /* yes, set alarm */
		tk_block ();		/* sleep 'til a timer expires */
	}
}	/*  end of tm_main()  */


/* Initialize the timer package.  Fork off the timer task.  Also set
 * up the alarm signal-processing routine tm_signal() and the global
 * tm_task which it uses.
 */

tm_init ()

{
	register task	*tk;		/* timer task's handle */

	/* start him up */	
	tm_task = tk = tk_fork(tk_cur, tm_main, TIMERSTACK, "Timer");
	tnonce = 1;
	signal (SIGALRM, tm_signal);	/* catch timer signal */
}


tm_signal (signo)

/* Signal handler for the SIGALRM signal.  Just wake up the timer task,
 * reenable catching the alarm signal, and leave.
 *
 * Arguments:
 */

int	signo;				/* signal number */
{
	tk_wake (tm_task);		/* wake up timer task */
	signal (SIGALRM, tm_signal);
}


/* Turn off timer interrupts.  This is useful while writing data to
 * the terminal.
 */

tm_off ()	{	alarm (-1);	}


/* Turn timer interrupts back on.  This is done by waking up the timer
 * task to process any events that went off while timer interrupts were
 * disabled.  If anything else is queued, that task will set a new alarm.
 */

tm_on ()	{	tk_wake (tm_task);	}

/* Allocate a timer and return a pointer to it */

timer *tm_alloc() {
	timer *t;

	if((t = (timer *)q_deq(&freetmq)) == NULL &&
	   (t = (timer *)calloc(1, sizeof(timer))) == NULL) return NULL;

	t->tm_elt.qe_next = NULL;
	return t;	}

/* Free up a timer. Returns true if successful, false otherwise */

tm_free(t)
	register timer *t; {
	register timer **tmp;
	queue *qp;

	/* Check if the timer is enqueued */

	for(tmp = &tm_queue.tmq_head; *tmp != NULL;
		tmp = (timer **)(((timer *)tmp)->tm_elt.qe_next))
			if(*tmp == t) {
#ifdef	DEBUG
				printf("Tried to free active timer.\n");
#endif
				return FALSE;
				}

	if(freetmq.tmq_len < TIMERHIWATER) {
		qp = (queue *)&freetmq;
		q_addh(qp, (q_elt)t);
		}
	else	cfree(t);

	return TRUE;
	}

/* Dump some information about the timer queue to the display. Used for
 * debugging. Takes a timer * as an argument and tells if it's in the queue.
 */

tm_qdump()
	{
	printf("Nonce = %u\n", tnonce);
	printf("timer queue:\n");
	printf("\thead %04x\ttail %04x\n", tm_queue.tmq_head,
							tm_queue.tmq_tail);
	printf("\tlength %u\n", tm_queue.tmq_len);
	}

/*  For debugging, dump the interesting parts of a timer. */
tm_dump(t)
	register timer *t; {

	printf("dump of timer %04x:\n", t);
	printf("  fire time is %U, nonce is %u, next q element is %04x\n",
			t->tm_time, t->tm_nonce, t->tm_elt.qe_next);

	}
