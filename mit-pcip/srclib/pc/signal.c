/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/*  Modified to set alarm in ticks rather than seconds, 12/83. <J.H.Saltzer>*/
#include <signal.h>

#define	NUMSIGS	30

int (*funs[NUMSIGS])();
unsigned _tcount = 0;
long cticks = 0;

int (*_alrm)();

/* SIGINIT() must be called before attempting to use the signal package. */

siginit() {
	int i;

	for(i = 0; i<NUMSIGS; i++) funs[i] = 0;
	crock_init();
	}


int (*signal(s, f))()
	int s;
	int (*f)(); {

	if(s >= SIGHUP) {
		if(s == SIGALRM) _alrm = f;
		else funs[s] = f;
		return f; }

	return (int (*)())-1; }

alarm(t)  /*  set alarm fire time so  t < tfire <= t+1  ticks from now.  */
	unsigned t; {

	_tcount = t+1;   /*  get tfire into proper interval  */
	return; }

_sig(n)
	int n; {

	if((((int)funs[n]) == 0) || (((int)funs[n]) == 1)) return;
	else (*funs[n])(n);
	}
