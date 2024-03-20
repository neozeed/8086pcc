/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* This file contains the routines which implement the IBM PC multitasking
 * system. Only the (hopefully) machine-independent routines are included
 * herein; the machine dependent routines are in the files stack.c and
 * tk_util.a86. The following routines are included:
 *	tk_init	      initializes the tasking system and creates the first task
 *	tk_fork		creates additional tasks.  Returns 0 if no memory left.
 *	tk_block	blocks the current, goes through the circular list 
 *			of tasks task in round robbin order until it finds
 *			one that is awake and starts it running 
 *	tk_wake		awakens a task by seting its event flag 
 *	tk_exit		to kill your own task
 *	tk_kill		to kill another task
 *	tk_stats()	to print statistics on tasking
 *     
 * Tasks are allocated dynamically as needed; they may
 * be of any size but their size is fixed once they are allocated. They are
 * then uniquely and permanantly associate with a task control block.
 *
 * Tasks form a circular list that is strung together by pointers (tk_nxt).
 * Currently tasks are placed in the list when they are created and they never
 * change their position.
 * Tasks have three states: blocked, awake, and running. The running task is 
 * the task that is currently executing. Only one task runs at any given time
 * and its event flag (ev_flg) determines whether it will be blocked or awake
 * after it finishes running. If a task is not running, it is awake if its 
 * event flag is set, and it is blocked if its event flag is not set.
 * tk_block resets a task's event flag just before as it starts to run.
 * A running task may thus awaken itself. A task gives up control of the 
 * processor by calling tk_block. Tk_block used the tk_nxt pointer of the 
 * task that is giving up control, to find the next task in the circular list.
 * If this next task is awake, tk_block will set it running. If it is blocked,
 * tk_block will use its tk_nxt pointer to find the next element in the 
 * circular list. If it is awake, tk_block will set it running. If not,...etc.
 * The currently running task is identified by the global variable tk_cur.
 */

#include	<stdio.h> 
#include	<task.h>

#define	GUARDWORD	0x1234

task	*tk_cur;			/* a pointer to the current task */
unsigned TDEBUG = 0;			/* DEBUG flag */
long tk_wakeups = 0;
static unsigned death = 0;
static task *died;

/* Initialize the tasking system. Create the first task, and set its stack 
   pointer to the main program stack. The first task will always use the main
   program stack, even though tk_init sets aside space for a stack of size 
   stksiz.(?!) The circular list of tasks contains only the original task,
   so originally set its next task pointer to point to itself. This routine
   returns to the caller with a pointer to the first task.	*/

task *tk_init(stksiz)
	int stksiz; {	/* size of the stack that is never used--in bytes */
	task *tk;		/* pointer to the first task */

	/* create the first task */
	tk = (task *)stk_init(sizeof(task));

	tk_cur = tk;		/* It is the currently running task */
	tk->ev_flg = 0;		/* Since it is running it does not */
				/* need to be awakened. */
	tk->tk_name = "Main";
	tk->tk_nxt = tk;	/* It is the next task since it is */
				/* the only task.	*/
	tk->tk_count = 0;
	tk->tk_size = stksiz;

	/* fill the system stack with the guardword */
	tk->tk_guard = (unsigned *)_stk_sys_fill(stksiz);

	return(tk);
	}

/* Create a new task  with stksiz bytes of stack, and place it in the circular
   list of tasks after prev_tk. Awaken it so that it will run, and set it up
   so that when it does run, it will start runing  routine start. 
   This routine does not affect the execution of the currently running task. 
   It returns a pointer to the new task, or zero if there isn't enough
   memory for the new task */

task *tk_fork(prev_tk, start, stksiz, name, arg)
	task	*prev_tk;  	/* predecessor (sp?) to the new task */
	int	(*start)();	/* Where the new task starts execution. */
	int	stksiz;		/* The size of the stack of the new task. */
	char	*name;		/* The task's name as a string */
	unsigned arg; {		/* argument to the task */
	task 	*tk;		/* a pointer to the new task */
	int	size;		/* size of the new task */
	
	/* create the new task */
	if (!(tk = (task *)stk_alloc (stksiz + sizeof(task)))) {
	    return(0);		/* if no memory left, return null */
	}
	_stk_fill(stksiz, tk+1);

	/* set it up to run */
	tk_frame (tk, &tk->tk_stack[(stksiz / sizeof (stack))], start, arg);

	tk->ev_flg = 1;			/* Schedule the task to run. */
	tk->tk_nxt = prev_tk->tk_nxt;     /* Fit it in after prev_tk. */
	prev_tk->tk_nxt = tk;
	tk->tk_name = name;		/* Set its name */
	tk->tk_count = 0;		/* init the count */
	tk->tk_guard = (unsigned *)(tk+1);
	tk->tk_size = stksiz;

	return (tk);
	}
	

/* Block the currently running task and run the next task in the circular list
   of tasks that is awake. Before returning, see if any cleanup has to be done
	for another task.
*/

tk_block () {
	register task *tk = tk_cur;	/* the next task to run */
	char *name;

	name = tk->tk_name;	/* careful, this isn't quite what you think */

	/* check if the guard word is still intact */
	if(*(tk->tk_guard) != GUARDWORD) {
		printf("TASK STACK OVERFLOW on task %s\n", tk->tk_name);
		tk_stats();
		exit();
		}

#ifdef	DEBUG
	if(TDEBUG)
		printf("TASK: Task %s blocking.\n", tk->tk_name);
#endif

	while (!((tk = tk->tk_nxt)->ev_flg));   /* find the next awake task */
	
	tk->ev_flg = 0;		/* Reset its event flag before it runs */

	tk_swtch (tk);				/* Run the next task. */

#ifdef	DEBUG
	if(TDEBUG)
		printf("TASK: Task %s running\n", name);
#endif

	if(death) {
		/* free up the task */
		death = FALSE;
		cfree(died);
		}
	}


/* tk_exit() : destroy the current task. Accomplished by setting a flag
	indicating that an exit has occured and then entering the scheduler
	loop. When tk_block() returns for some other task and finds this
	flag set, it deallocates the task which exited. This is necessary
	because we still need a stack to run on.

	The task removes itself from the circular list of tasks in the system
	so that it cannot be awoken after it has exited. Otherwise, the exit
	might be done in the context of the task itself, which would prove
	disastrous.

	Yes, this routine never returns (not really).
*/

tk_exit() {
	register task *tk;

	/* hunt for the task which tk_cur is the successor of */
	for(tk = tk_cur; tk->tk_nxt != tk_cur; tk = tk->tk_nxt) ;

	/* now patch around tk_cur */
	tk->tk_nxt = tk_cur->tk_nxt;

	death = TRUE;
	died = tk_cur;
	tk_block();
	printf("Disaster: tk_exit() returning!!!\n");
	}

/* tk_kill(task) kill a task.  A special case is if you're killing
 * yourself, you die the instant your task blocks.
*/

tk_kill(tk_to_die)
	register task *tk_to_die; {
	register task *tk;

	/* hunt for the task which tk_to_die is the successor of */
	for(tk = tk_cur; tk->tk_nxt != tk_to_die; tk = tk->tk_nxt) ;

	/* now patch around tk_to_die */
	tk->tk_nxt = tk_to_die->tk_nxt;

	/* If I'm killing myself, die the next time I block */
	if (tk_cur == tk_to_die) {
		death = TRUE;
		died = tk_cur;
	}
	else cfree(tk_to_die);
	}

/* print out status */

tk_stats() {
	task *tk;

	printf("tasking status:");
	printf("task wakeups: %D\n", tk_wakeups);
	printf("wakeups\talloced\tused\ttasks:\n");

	tk = tk_cur;
	do {
		unsigned *up;
		int i;

		up = tk->tk_guard;

		for(i = tk->tk_size/sizeof(stack)-1; i; i--)
			if(*up++ != GUARDWORD) break;

		printf("%u\t%u\t%u\t%s\n", tk->tk_count, tk->tk_size,
						i*sizeof(stack), tk->tk_name);
		tk = tk->tk_nxt;
		} while(tk != tk_cur);
	}
