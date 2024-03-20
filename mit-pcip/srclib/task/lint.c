#include <task.h>

/* tasking package lint definitions
	all from tk_util.a86
*/

/*ARGSUSED*/
tk_frame(tk, stack, start, arg)
	task *tk;
	char *stack;
	int (*start)();
	unsigned arg; { }

/*ARGSUSED*/
tk_swtch(tk)
	task *tk; { }

