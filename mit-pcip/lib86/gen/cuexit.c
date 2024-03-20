
static int (*closers[30])();
static int nclosers = 0;

exit_hook(func)
	int (*func)(); {

/*	printf("exit_hook: adding func %04x\n", func);	*/

	closers[++nclosers] = func;
	}

exit(code) {
	int n;

	for(n=nclosers; n; n--) {
/*		printf("exit: calling func %04x\n", closers[n]);	*/
		(*closers[n])();
		}

	_exit(code);
 }
