/* mcount uses an array like struct mcount mc[cntsiz] */
struct mcount
{
	long mc_addr;
	long mc_count;
};

/* file mon.out will have a header like: */
struct monhdr
{
	int (*lowpc)();
	int (*highpc)();
	long cntsiz;
};

monitor(lowpc, highpc, buf, bufsiz, cntsiz)
int (*lowpc)(), (*highpc)();
short *buf;
int bufsiz;	/* number of shorts in buf */
{
	int file;
	unsigned scale;
	int pbufsiz, pbufoffs;
	struct monhdr *mhp;
	static short *sbuf;
	static int ssiz;

	if (lowpc == 0) {
		profil(0, 0, 0, 0);
		file = creat("mon.out", 0666);
		write(file, sbuf, ssiz * sizeof(short));
		close(file);
		return;
	}

	ssiz = bufsiz;
	sbuf = buf;

	mhp = (struct monhdr *)buf;
	mhp->lowpc = lowpc;
	mhp->highpc = highpc;
	mhp->cntsiz = cntsiz;

	pbufoffs = sizeof(struct monhdr) + cntsiz * sizeof(struct mcount);
	pbufsiz = bufsiz * sizeof(short) - pbufoffs;
	if (pbufsiz <= 0) return;

	scale = pbufsiz << 16;
	scale /= (unsigned)((int)highpc - (int)lowpc);
	scale = scale - 1;

	profil(&((char *)buf)[pbufoffs], pbufsiz, (int)lowpc, (int)scale);
}
