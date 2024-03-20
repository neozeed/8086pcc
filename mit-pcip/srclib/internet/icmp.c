/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>
#include <timer.h>
#include "redirtab.h"

/* 7/4/84 - fixed bug where IcEchoRequest() didn't free its packet if
	it couldn't allocate a timer.	<John Romkey>
   10/2/84 - changed random() to rand() in the new I/O library.
					<John Romkey>
*/

#define	ICMP	1		/* ICMP Protocol number */
#define ECHOREP	0		/* ICMP Echo reply */
#define	DESTIN	3		/* Destination Unreachable */
#define	SOURCEQ	4		/* Source quench */
#define	REDIR	5		/* Redirect */
#define	ECHOREQ	8		/* ICMP Echo request */
#define	TIMEX	11		/* Time exceeded */
#define	PARAM	12		/* Parameter problem */
#define	TIMEREQ	13		/* Timestamp request */
#define	TIMEREP	14
#define	INFO	15		/* Information request */
#define ICMPSIZE	sizeof(struct ping)
#define	ECHOTMO	6		/* Echo reply timeout period. */

struct ping {				/* ICMP Echo request/reply header */
	char ptype;
	char pcode;
	unshort pchksum;
	unshort pid;
	unshort pseq;
	};

/* structure of an icmp destination unreachable packet */

struct destun  {
	char dtype;
	char dcode;
	unshort dchksum;
	unshort dno1;
	unshort dno2;
	struct ip dip;
	char	ddata[8];
	};

/* structure of an icmp time exceeded packet */

struct timex  {
	char ttype;
	char tcode;
	unshort tchksum;
	unshort tno1;
	unshort tno2;
	struct ip tip;
	char	tdata[8];
	};

/* structure of a timestamp reply */

struct tstamp {
	char	ttype;
	char	tcode;
	unsigned txsum;
	unsigned tid;
	unsigned tseq;
	long	tstamp[3];
	};

/* structure of an icmp redirect */

struct redirect {
	char	rdtype;
	char	rdcode;
	unshort	rdchksum;
	in_name	rdgw;
	struct ip rdip;
	char	rddata[8];
	};


struct redent redtab[REDIRTABLEN];
int rednext = 0;

static IPCONN icmp;
static int pingstate = PGWAITING;		/* ICMP echo reply indicator */
static task *requested = 0;
static PACKET sent;
static unsigned snt_len;
static unshort pingseq = 1;

static int icmprcv();
static int wake_req(), pingtmo();

IcmpInit() {

	icmp = in_open(ICMP, icmprcv);

#ifdef	DEBUG
	if(icmp == 0 && (NDEBUG & (INFOMSG|PROTERR)))
		printf("ICMP: Can't open ip conn\n");
	else if(NDEBUG & INFOMSG)
		printf("ICMP: Opened ip conn\n");
#endif
	}

/* ICMP packet handler */

static icmprcv(p, len, host)
	PACKET p;			/* the incoming packet */
	int len;			/* packet length */
	in_name host; {			/* the foreign host */
	register struct ip *pip;
	register struct ping *e;
	struct redirect *rd;
	struct destun *pdp;
	unsigned osum, xsum;
	char *data1, *data2;
	int i;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("ICMP: p[%u] from %a\n", len, host);
#endif

	pip = in_head(p);
	e = (struct ping *)in_data(pip);

	osum = e->pchksum;
	e->pchksum = 0;

	if(len&1)
		((char *)e)[len] = 0;

	if((xsum = ~cksum(e, (len+1)>>1)) != osum) {
		e->pchksum = osum;
#ifdef	DEBUG
		if(NDEBUG & PROTERR)
			printf("ICMP: Bad xsum %04x should have been %04x\n",
						osum, xsum);
		if(NDEBUG & DUMP)
			in_dump(p);
#endif

		in_free(p);
		return;
		}

	e->pchksum = osum;
		
	switch(e->ptype) {
	case ECHOREQ:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("ICMP: echo reply to %a\n", host);
#endif
		e->ptype = ECHOREP;
		e->pchksum = 0;
		if(len&1)
			((char *)e)[len] = 0;

		e->pchksum = ~cksum(e, (len+1)>>1);
		pip->ip_src = pip->ip_dest;
		pip->ip_dest = host;
		if(in_write(icmp, p, len, host) <= 0) {
#ifdef	DEBUG
			printf("icmp: reply failed\n");
#endif
		}
		in_free(p);
		break;
	case DESTIN:
		pdp = (struct destun *)in_data(in_head(p));
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR)) {
			printf("ICMP: got dest unreachable type ");
			printf("%u on %a\n", pdp->dcode, pdp->dip.ip_dest);
			}
#endif
		in_free(p);
		break;
	case SOURCEQ:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR))
			printf("ICMP: source quench from %a\n", host);
		if(NDEBUG & DUMP) in_dump(p);
#endif
		in_free(p);
		break;
	case REDIR:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("ICMP: rcvd redirect\n");
#endif
		rd = (struct redirect *)e;
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("redirect for %a to %a\n", rd->rdip.ip_dest,
							rd->rdgw);
#endif

		for(i=0; i<REDIRTABLEN; i++)
			if(redtab[i].rd_dest == rd->rdip.ip_dest) {
				redtab[i].rd_to = rd->rdgw;
				in_free(p);
				return;
				}

		redtab[rednext].rd_dest = rd->rdip.ip_dest;
		redtab[rednext].rd_to = rd->rdgw;
		rednext++;
		rednext &= REDIRTABLEN-1;
		in_free(p);
		break;
	case ECHOREP:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("icmp: got echo reply\n");
#endif

		if(e->pseq != pingseq-1) {
			if(NDEBUG & NETERR|PROTERR|INFOMSG)
				printf("icmp_rcv: Bad echo reply seq #\n");
			in_free(p);
			return;
			}

		data1 = in_data(in_head(p))+ICMPSIZE;
		data2 = in_data(in_head(sent))+ICMPSIZE;
		for(i=0; i < snt_len; i++)
			if(*data1++ != *data2++) {
#ifdef	DEBUG
				if(NDEBUG & (PROTERR|NETERR))
					printf("bad icmp data at byte %u\n",
									i);
				if(NDEBUG & DUMP) {
					printf("ICMP: Bad data in echo rep\n");
					printf("sent:\n");
					in_dump(sent);
					printf("rcvd:\n");
					in_dump(p);
					}
#endif

				pingstate = PGBADDATA;
				wake_req();
				in_free(p);
				return;
				}

		pingstate = PGSUCCESS; 
		wake_req();
		in_free(p);
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("icmp: got icmp echo reply.\n");
#endif
		break;

	case TIMEX:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR)) {
			struct timex *pt = (struct timex *)e;

			printf("ICMP: timex msg from %a about %a\n",
				in_head(p)->ip_src, pt->tip.ip_dest);
			}

		if(NDEBUG & DUMP) in_dump(p);
#endif
		in_free(p);
		break;
	case PARAM:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR))
			printf("ICMP: got param problem message\n");
		if(NDEBUG & DUMP) in_dump(p);
#endif
		in_free(p);
		break;
	case TIMEREQ:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR))
		       printf("ICMP: got timestamp request\n");
#endif
		e->ptype = TIMEREP;
		e->pchksum = 0;
		e->pchksum = ~cksum(e, sizeof(struct tstamp)>>1);
		pip->ip_src = pip->ip_dest;
		pip->ip_dest = host;
		if(in_write(icmp, p, sizeof(struct tstamp), host) <= 0) {
#ifdef	DEBUG
			printf("icmp: can't send timestamp reply\n");
#endif
		}
		in_free(p);
		break;
	case INFO:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|NETERR))
			printf("icmp: got info request\n");
#endif
		in_free(p);
		break;
	default:
#ifdef	DEBUG
		if(NDEBUG & (PROTERR|INFOMSG))
			printf("icmp: unhandled type %u\n", e->ptype);
		if(NDEBUG & DUMP) in_dump(p);
#endif
		in_free(p);
		}
	}


/* ICMP Echo Request - returns 1 if host replies, 0 if timeout or error */

IcEchoRequest(host, length)
	in_name host;
	unsigned length;	{
	PACKET p;
	register struct ping *e;
	register char *data;
	timer *tm;
	int i;

	p = in_alloc(40, 0);
	if(p == 0) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("icmp: can't alloc packet\n");
#endif
		return PGNOSND;
		}

	e = (struct ping *)in_data(in_head(p));
	e->ptype = ECHOREQ;
	e->pcode = 0;
	e->pid = 0;
	e->pseq = pingseq++;

	/* Put 256 random numbers in the packet. */
	data = in_data(in_head(p)) + ICMPSIZE;
	for(i=0; i<length; i++) *data++ = rand();

	/* Calculate the checksum */
	e->pchksum = 0;
	if((ICMPSIZE+length)&1)
		*data = 0;

	e->pchksum = ~cksum(e, (ICMPSIZE+length+1)>>1);

	pingstate = PGWAITING;
	requested = tk_cur;
	sent = p;
	snt_len = length;
	tm = tm_alloc();
	if(tm == 0) {
#ifdef	DEBUG
		printf("icmp: can't alloc timer\n");
#endif
		in_free(p);
		return PGNOSND;
		}

	tm_set(ECHOTMO, pingtmo, 0, tm);

	if(in_write(icmp, p, ICMPSIZE+length, host) <= 0) {
#ifdef	DEBUG
		if(NDEBUG & PROTERR)
			printf("icmp: can't send echo request\n");
#endif
		tm_clear(tm);
		tm_free(tm);
		in_free(p);
		return PGNOSND;
		}
#ifdef	DEBUG

		if(NDEBUG & DUMP) in_dump(p);
#endif
	
	while(pingstate == PGWAITING) tk_block();
	tm_clear(tm);
	tm_free(tm);
	in_free(p);
	sent = 0;
	requested = 0;
	return pingstate;
	}

static char *dsts[] = {
	"net", "host", "protocol", "port", "fragmentation needed",
	"source route failed" };

icmp_destun(host, ip, type)
	in_name host;
	register struct ip *ip;
	unsigned type; {
	PACKET p;
	register struct destun *d;
	int i;

#ifdef	DEBUG
	if(NDEBUG & PROTERR)
		printf("icmp: sending %s dest unreachable to %a\n",
							dsts[type], host);
#endif

	p = in_alloc(512, 0);
	if(p == NULL) {
		if(NDEBUG & PROTERR)
			printf("icmp: can't alloc pkt\n");
		return 0;
		}

	d = (struct destun *)in_data(in_head(p));

	d->dtype = DESTIN;
	d->dcode = type;
	for(i=0; i<sizeof(struct ip)+8; i++)
		((char *)&d->dip)[i] = ((char *)ip)[i];

	d->dchksum = 0;
	d->dchksum = ~cksum(d, sizeof(struct destun)>>1);

	if((i = in_write(icmp, p, sizeof(struct destun), host)) <= 0) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|NETERR|PROTERR))
			printf("ICMP: Can't send dest unreachable\n");
#endif
		}

	in_free(p);
	return;
	}


static pingtmo() {
	pingstate = PGTMO;
	if(requested) tk_wake(requested);
	}

static int wake_req() {
	if(requested) tk_wake(requested);

	}
