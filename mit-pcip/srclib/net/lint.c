/* C style variable and procedure declarations for lint
*/

/* from netcrt.a86
*/
unsigned version;

/*ARGSUSED*/
exit(code)
	int code; { }

char *_memtop;

/* from net_utils.a86
*/
brk_init() { }
brk_c() { }

/*ARGSUSED*/
unsigned swab(i)
	int i; { return 0; }

/*ARGSUSED*/
unsigned bswap(i)
	int i; { return 0; }

/*ARGSUSED*/
unsigned cksum(buf, len, start)
	char *buf;
	int len;
	int start; { return 0; }

/*ARGSUSED*/
long lswap(l)
	long l; { return 0l; }

/*ARGSUSED*/
long wswap(l)
	long l; { return 0l; }
