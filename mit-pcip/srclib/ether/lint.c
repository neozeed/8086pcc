/* declarations of functions & stuff to make lint happy with the
   ethernet driver
*/

/*ARGSUSED*/
et_patch(intvec)
	int intvec; { }

et_unpatch() { }

/*ARGSUSED*/
etdma(addr)
	char *addr; { }
