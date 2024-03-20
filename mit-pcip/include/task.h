/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* task.h
    4/17/84 - Added tk_sleep().
    5/23/84 - Added tk_exit(), guard words and context switch counting.
					<John Romkey>
*/

/* This file contains definitions for the IBM PC tasking package. */


typedef	int	stack;			/* type of data in task stacks */
typedef char	event;

typedef	struct	task	{	/* an IBM PC task - top of its stack */
	stack	*tk_fp;			/* task's current frame ptr */
	char	*tk_name;		/* the task's name */
	int	ev_flg;			/* flag set if task is scheduled */
	struct	task *tk_nxt;		/* pointer to next task */
	unsigned tk_count;		/* number of wakeups */
	unsigned *tk_guard;		/* pointer to lowest guardword */
	unsigned tk_size;		/* stack size */
	stack	tk_stack[1];		/* top of task's stack */
	}	*Task;

typedef struct task task;

extern	struct	task	*tk_cur;	/* currently running task */
extern	struct	task	*tk_init ();	/* initialize task system */
extern	struct	task	*tk_fork ();	/* fork a new task */
extern	unsigned TDEBUG;		/* tasking debugging */
extern	long tk_wakeups;

/* Useful macro */

#define tk_wake(tk)     { (tk)->ev_flg = TRUE; tk_wakeups++; (tk)->tk_count++; }
#define	tk_yield()	{ tk_wake(tk_cur); tk_block(); }

#define	tk_setef(t, e)	{ tk_wake((t)); *(e) = 1; }
#define	tk_sleep(t)	t->ev_flg = FALSE
