static	char sccsid[] = "@(#)ar.c 4.1 10/1/80";
/*
 * ar - portable (ascii) format version
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "_ar.h"
#ifdef SIGNAL
#include <signal.h>
#endif
#include <fcntl.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif

typedef	unsigned short ushort;
struct	stat	stbuf;
struct	ar_hdr	arbuf;
struct	lar_hdr {
	char	lar_name[16];
	long	lar_date;
	ushort	lar_uid;
	ushort	lar_gid;
	ushort	lar_mode;
	long	lar_size;
} larbuf;

#define	SKIP	1
#define	IODD	2
#define	OODD	4
#define	HEAD	8

char	*man	=	{ "mrxtdpq" };
char	*opt	=	{ "uvnbail" };

#ifdef SIGNAL
int	signum[] = {SIGHUP, SIGINT, SIGQUIT, 0};
#else
int	signum[] = {0};
#endif
int	sigdone();
long	lseek();
int	rcmd();
int	dcmd();
int	xcmd();
int	tcmd();
int	pcmd();
int	mcmd();
int	qcmd();
int	(*comfun)();
char	flg[26];
char	**namv;
int	namc;
char	*arnam;
char	*ponam;
#if 0
char	*artmpnam		=	{ "/tmp/vXXXXX" };
char	*tmp1nam	=	{ "/tmp/v1XXXXX" };
char	*tmp2nam	=	{ "/tmp/v2XXXXX" };
#else
char	artmpnam[16];
char	tmp1nam[16];
char	tmp2nam[16];
#endif
char	*tfnam;
char	*tf1nam;
char	*tf2nam;
char	*file;
char	name[16];
int	af;
int	tf;
int	tf1;
int	tf2;
int	qf;
int	bastate;
char	buf[BUFSIZ];

char	*trim();
char	*mktemp();
char	*ctime();

main(argc, argv)
char *argv[];
{
	register i;
	register char *cp;

#ifdef SIGNAL
	for(i=0; signum[i]; i++)
		if(signal(signum[i], SIG_IGN) != SIG_IGN)
			signal(signum[i], sigdone);
#endif
	if(argc < 3)
		usage();
	cp = argv[1];
	for(cp = argv[1]; *cp; cp++)
	switch(*cp) {
	case 'l':
	case 'v':
	case 'u':
	case 'n':
	case 'a':
	case 'b':
	case 'c':
	case 'i':
		flg[*cp - 'a']++;
		continue;

	case 'r':
		setcom(rcmd);
		continue;

	case 'd':
		setcom(dcmd);
		continue;

	case 'x':
		setcom(xcmd);
		continue;

	case 't':
		setcom(tcmd);
		continue;

	case 'p':
		setcom(pcmd);
		continue;

	case 'm':
		setcom(mcmd);
		continue;

	case 'q':
		setcom(qcmd);
		continue;

	default:
		fprintf(stderr, "ar: bad option `%c'\n", *cp);
		done(1);
	}
#if 0
	if(flg['l'-'a']) {
		artmpnam = "vXXXXX";
		tmp1nam = "v1XXXXX";
		tmp2nam = "v2XXXXX";
	}
#endif
	if(flg['i'-'a'])
		flg['b'-'a']++;
	if(flg['a'-'a'] || flg['b'-'a']) {
		bastate = 1;
		ponam = trim(argv[2]);
		argv++;
		argc--;
		if(argc < 3)
			usage();
	}
	arnam = argv[2];
	namv = argv+3;
	namc = argc-3;
	if(comfun == 0) {
		if(flg['u'-'a'] == 0) {
			fprintf(stderr, "ar: one of [%s] must be specified\n", man);
			done(1);
		}
		setcom(rcmd);
	}
	(*comfun)();
	//done(notfound());
}

setcom(fun)
int (*fun)();
{

	if(comfun != 0) {
		fprintf(stderr, "ar: only one of [%s] allowed\n", man);
		done(1);
	}
	comfun = fun;
}

rcmd()
{
	register f;

	init();
	getaf();
	while(!getdir()) {
		bamatch();
		if(namc == 0 || match()) {
			f = stats();
			if(f < 0) {
				if(namc)
					fprintf(stderr, "ar: cannot open %s\n", file);
				goto cp;
			}
			if(flg['u'-'a'])
				if(stbuf.st_mtime <= larbuf.lar_date) {
					close(f);
					goto cp;
				}
			mesg('r');
			copyfil(af, -1, IODD+SKIP);
			movefil(f);
			continue;
		}
	cp:
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	cleanup();
}

dcmd()
{

	init();
	if(getaf())
		noar();
	while(!getdir()) {
		if(match()) {
			mesg('d');
			copyfil(af, -1, IODD+SKIP);
			continue;
		}
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	install();
}

xcmd()
{
	register f;

	if(getaf())
		noar();
	while(!getdir()) {
		if(namc == 0 || match()) {
			f = creat(file, larbuf.lar_mode & 0777);
			if(f < 0) {
				fprintf(stderr, "ar: %s cannot create\n", file);
				goto sk;
			}
			setmode(f,O_BINARY);
			mesg('x');
			copyfil(af, f, IODD);
			close(f);
			continue;
		}
	sk:
		mesg('c');
		copyfil(af, -1, IODD+SKIP);
		if (namc > 0  &&  !morefil())
			done(0);
	}
}

pcmd()
{

	if(getaf())
		noar();
	while(!getdir()) {
		if(namc == 0 || match()) {
			if(flg['v'-'a']) {
				printf("\n<%s>\n\n", file);
				fflush(stdout);
			}
			copyfil(af, 1, IODD);
			continue;
		}
		copyfil(af, -1, IODD+SKIP);
	}
}

mcmd()
{

	init();
	if(getaf())
		noar();
//	tf2nam = mktemp(tmp2nam);
	sprintf(tmp2nam,"/tmp/v2%06d",_getpid());
	tf2nam = &tmp2nam;
	close(creat(tf2nam, 0600));
	tf2 = open(tf2nam, O_BINARY|O_RDWR);
	if(tf2 < 0) {
		fprintf(stderr, "ar: cannot create third temp\n");
		done(1);
	}
	setmode(tf2,O_BINARY);
	while(!getdir()) {
		bamatch();
		if(match()) {
			mesg('m');
			copyfil(af, tf2, IODD+OODD+HEAD);
			continue;
		}
		mesg('c');
		copyfil(af, tf, IODD+OODD+HEAD);
	}
	install();
}

tcmd()
{

	if(getaf())
		noar();
	while(!getdir()) {
		if(namc == 0 || match()) {
			if(flg['v'-'a'])
				longt();
			printf("%s\n", trim(file));
		}
		copyfil(af, -1, IODD+SKIP);
	}
}

qcmd()
{
	register i, f;

	if (flg['a'-'a'] || flg['b'-'a']) {
		fprintf(stderr, "ar: abi not allowed with q\n");
		done(1);
	}
	getqf();
#ifdef SIGNAL
	for(i=0; signum[i]; i++)
		signal(signum[i], SIG_IGN);
#endif
	lseek(qf, 0l, 2);
	for(i=0; i<namc; i++) {
		file = namv[i];
		if(file == 0)
			continue;
		namv[i] = 0;
		mesg('q');
		f = stats();
		if(f < 0) {
			fprintf(stderr, "ar: %s cannot open\n", file);
			continue;
		}
		tf = qf;
		movefil(f);
		qf = tf;
	}
}

init()
{
//	tfnam = mktemp(artmpnam);
	sprintf(artmpnam,"/tmp/v%06d",_getpid());
	tfnam=&artmpnam;
	close(creat(tfnam, 0600));
	tf = open(tfnam, O_CREAT|O_BINARY|O_RDWR);
	if(tf < 0) {
		fprintf(stderr, "ar: cannot create temp file\n");
		done(1);
	}
	setmode(tf,O_BINARY);
	if (write(tf, ARMAG, SARMAG) != SARMAG)
		wrerr();
}

getaf()
{
	char mbuf[SARMAG];

	af = open(arnam, O_BINARY|O_RDONLY);
	if(af < 0)
		return(1);
	//shouldnt be using strncmp! JASON
	if (read(af, mbuf, SARMAG) != SARMAG || strncmp(mbuf, ARMAG, SARMAG)) {
		fprintf(stderr, "ar: %s not in archive format\n", arnam);
		done(1);
	}
	return(0);
}

getqf()
{
	char mbuf[SARMAG];

	if ((qf = open(arnam, O_BINARY|O_RDWR)) < 0) {
		if(!flg['c'-'a'])
			fprintf(stderr, "ar: creating %s\n", arnam);
		if ((qf = creat(arnam, 0666)) < 0) {
			fprintf(stderr, "ar: cannot create %s\n", arnam);
			done(1);
		}
		setmode(qf,O_BINARY);
		if (write(qf, ARMAG, SARMAG) != SARMAG)
			wrerr();
	} else if (read(qf, mbuf, SARMAG) != SARMAG
		|| strncmp(mbuf, ARMAG, SARMAG)) {
		fprintf(stderr, "ar: %s not in archive format\n", arnam);
		done(1);
	}
}

usage()
{
	printf("usage: ar [%s][%s] archive files ...\n", opt, man);
	done(1);
}

noar()
{

	fprintf(stderr, "ar: %s does not exist\n", arnam);
	done(1);
}

sigdone()
{
	done(100);
}

done(c)
{

	if(tfnam)
		unlink(tfnam);
	if(tf1nam)
		unlink(tf1nam);
	if(tf2nam)
		unlink(tf2nam);
	exit(c);
}

notfound()
{
	register i, n;

	n = 0;
	for(i=0; i<namc; i++)
		if(namv[i]) {
			fprintf(stderr, "ar: %s not found\n", namv[i]);
			n++;
		}
	return(n);
}

morefil()
{
	register i, n;

	n = 0;
	for(i=0; i<namc; i++)
		if(namv[i])
			n++;
	return(n);
}

cleanup()
{
	register i, f;

	for(i=0; i<namc; i++) {
		file = namv[i];
		if(file == 0)
			continue;
		namv[i] = 0;
		mesg('a');
		f = stats();
		if(f < 0) {
			fprintf(stderr, "ar: %s cannot open\n", file);
			continue;
		}
		movefil(f);
	}
	install();
}

install()
{
	register i;
#ifdef SIGNAL
	for(i=0; signum[i]; i++)
		signal(signum[i], SIG_IGN);
#endif
	if(af < 0)
		if(!flg['c'-'a'])
			fprintf(stderr, "ar: creating %s\n", arnam);
	close(af);
	af = creat(arnam, 0666);
	if(af < 0) {
		fprintf(stderr, "ar: cannot create %s\n", arnam);
		done(1);
	}
	setmode(af,O_BINARY);
	if(tfnam) {
		lseek(tf, 0l, 0);
		while((i = read(tf, buf, BUFSIZ)) > 0)
			if (write(af, buf, i) != i)
				wrerr();
	}
	if(tf2nam) {
		lseek(tf2, 0l, 0);
		while((i = read(tf2, buf, BUFSIZ)) > 0)
			if (write(af, buf, i) != i)
				wrerr();
	}
	if(tf1nam) {
		lseek(tf1, 0l, 0);
		while((i = read(tf1, buf, BUFSIZ)) > 0)
			if (write(af, buf, i) != i)
				wrerr();
	}
}

/*
 * insert the file 'file'
 * into the temporary file
 */
movefil(f)
{
	char buf[sizeof(arbuf)+1];
//STRINGS!
	sprintf(buf, "%-16s%-12ld%-6u%-6u%-8o%-10ld%-2s",
	   trim(file),
	   stbuf.st_mtime,
	   stbuf.st_uid,
	   stbuf.st_gid,
	   stbuf.st_mode,
	   stbuf.st_size,
	   ARFMAG);
	strncpy((char *)&arbuf, buf, sizeof(arbuf));
	larbuf.lar_size = stbuf.st_size;
	copyfil(f, tf, OODD+HEAD);
	close(f);
}

stats()
{
	register f;

	f = open(file, O_BINARY|O_RDONLY);
	if(f < 0)
		return(f);
	if(fstat(f, &stbuf) < 0) {
		close(f);
		return(-1);
	}
	return(f);
}

/*
 * copy next file
 * size given in arbuf
 */
copyfil(fi, fo, flag)
{
	register i, o;
	int pe;

	if(flag & HEAD) {
		for (i=sizeof(arbuf.ar_name)-1; i>=0; i--) {
			if (arbuf.ar_name[i]==' ')
				continue;
			else if (arbuf.ar_name[i]=='\0')
				arbuf.ar_name[i] = ' ';
			else
				break;
		}
		if (write(fo, (char *)&arbuf, sizeof arbuf) != sizeof arbuf)
			wrerr();
	}
	pe = 0;
	while(larbuf.lar_size > 0) {
		i = o = BUFSIZ;
		if(larbuf.lar_size < i) {
			i = o = larbuf.lar_size;
			if(i&1) {
				//buf[i] = '\n';
				buf[i] = '\r';	//I think this is screwing up
				if(flag & IODD)
					i++;
				if(flag & OODD)
					o++;
			}
		}
		if(read(fi, buf, i) != i)
			pe++;
		if((flag & SKIP) == 0)
			if (write(fo, buf, o) != o)
				wrerr();
		larbuf.lar_size -= BUFSIZ;
	}
	if(pe)
		phserr();
}

getdir()
{
	register char *cp;
	register i;

	i = read(af, (char *)&arbuf, sizeof arbuf);
	if(i != sizeof arbuf) {
		if(tf1nam) {
			i = tf;
			tf = tf1;
			tf1 = i;
		}
		return(1);
	}
	//printf("comparing [\n%s\n] to [\n%s\n] size %d\n",arbuf.ar_fmag,ARFMAG, sizeof(arbuf.ar_fmag));
	if (strncmp(arbuf.ar_fmag, ARFMAG, sizeof(arbuf.ar_fmag))) {
		fprintf(stderr, "ar: malformed archive (at %ld)\n", lseek(af, 0L, 1));
		done(1);
	}
	cp = arbuf.ar_name + sizeof(arbuf.ar_name);
	while (*--cp==' ')
		;
	*++cp = '\0';
	strncpy(name, arbuf.ar_name, sizeof(arbuf.ar_name));
	file = name;
	strncpy(larbuf.lar_name, name, sizeof(larbuf.lar_name));
	sscanf(arbuf.ar_date, "%ld", &larbuf.lar_date);
	sscanf(arbuf.ar_uid, "%hd", &larbuf.lar_uid);
	sscanf(arbuf.ar_gid, "%hd", &larbuf.lar_gid);
	sscanf(arbuf.ar_mode, "%ho", &larbuf.lar_mode);
	sscanf(arbuf.ar_size, "%ld", &larbuf.lar_size);
	return(0);
}

match()
{
	register i;

	for(i=0; i<namc; i++) {
		if(namv[i] == 0)
			continue;
		if(strcmp(trim(namv[i]), file) == 0) {
			file = namv[i];
			namv[i] = 0;
			return(1);
		}
	}
	return(0);
}

bamatch()
{
	register f;

	switch(bastate) {

	case 1:
		if(strcmp(file, ponam) != 0)
			return;
		bastate = 2;
		if(flg['a'-'a'])
			return;

	case 2:
		bastate = 0;
//		tf1nam = mktemp(tmp1nam);
		sprintf(tmp1nam,"/tmp/v1%06d",_getpid());
		tf1nam = &tmp1nam;
		close(creat(tf1nam, 0600));
		f = open(tf1nam, O_BINARY|O_RDWR);
		if(f < 0) {
			fprintf(stderr, "ar: cannot create second temp\n");
			return;
		}
		tf1 = tf;
		tf = f;
	}
}

phserr()
{

	fprintf(stderr, "ar: phase error on %s\n", file);
}

mesg(c)
{

	if(flg['v'-'a'])
		if(c != 'c' || flg['v'-'a'] > 1)
			printf("%c - %s\n", c, file);
}

char *
trim(s)
char *s;
{
	register char *p1, *p2;

	for(p1 = s; *p1; p1++)
		;
	while(p1 > s) {
		if(*--p1 != '/')
			break;
		*p1 = 0;
	}
	p2 = s;
	for(p1 = s; *p1; p1++)
		if(*p1 == '/')
			p2 = p1+1;
	return(p2);
}

#define	IFMT	060000
#define	ISARG	01000
#define	LARGE	010000
#define	SUID	04000
#define	SGID	02000
#define	ROWN	0400
#define	WOWN	0200
#define	XOWN	0100
#define	RGRP	040
#define	WGRP	020
#define	XGRP	010
#define	ROTH	04
#define	WOTH	02
#define	XOTH	01
#define	STXT	01000

longt()
{
	register char *cp;

	pmode();
	printf("%3d/%1d", larbuf.lar_uid, larbuf.lar_gid);
	printf("%7ld", larbuf.lar_size);
	cp = ctime(&larbuf.lar_date);
	printf(" %-12.12s %-4.4s ", cp+4, cp+20);
}

int	m1[] = { 1, ROWN, 'r', '-' };
int	m2[] = { 1, WOWN, 'w', '-' };
int	m3[] = { 2, SUID, 's', XOWN, 'x', '-' };
int	m4[] = { 1, RGRP, 'r', '-' };
int	m5[] = { 1, WGRP, 'w', '-' };
int	m6[] = { 2, SGID, 's', XGRP, 'x', '-' };
int	m7[] = { 1, ROTH, 'r', '-' };
int	m8[] = { 1, WOTH, 'w', '-' };
int	m9[] = { 2, STXT, 't', XOTH, 'x', '-' };

int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

pmode()
{
	register int **mp;

	for (mp = &m[0]; mp < &m[9];)
		arselect(*mp++);
}

arselect(pairp)
int *pairp;
{
	register int n, *ap;

	ap = pairp;
	n = *ap++;
	while (--n>=0 && (larbuf.lar_mode&*ap++)==0)
		ap++;
	putchar(*ap);
}

wrerr()
{
	perror("ar write error");
	done(1);
}
