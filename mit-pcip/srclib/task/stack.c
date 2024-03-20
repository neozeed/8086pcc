/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 10/1/84 - added declaration of calloc() so compiler wouldn't grumble.
						<John Romkey>
*/

#include <stdio.h>
#include <task.h>

char *calloc();

/* STK_INIT(size) is essentially something else... */

char *stk_init(size)
	unsigned size; {

	return calloc(1, size);
	}

/* STK_ALLOC(size) is sort of what it is. */

char *stk_alloc(size)
	unsigned size; {

	return calloc(1, size);
	}
	
_cdump() {
	
	printf("BUGHALT: task return\n");
	printf("a task has returned, but tasks should never do this.\n");
	printf("please report this problem to the maintainers along with the following information:\n");
	printf("task name: %s\n", tk_cur->tk_name);
	tk_stats();
	exit(1);
	}
