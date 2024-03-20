/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 8/24/84 - sanitized the debugging messages.
					<John Romkey>
*/

#include <ip.h>

/* A tiny GGP which will respond to ggp echo requests.
*/

#define GGP	3
#define	ECHOREQ	8
#define	ECHOREP	0

static IPCONN ggp;
int ggprcv();

GgpInit() {

	ggp = in_open(GGP, ggprcv);
#ifdef	DEBUG
	if(ggp == 0 && (NDEBUG & (INFOMSG|PROTERR)))
		printf("GGP: can't open ip con\n");
	else if(NDEBUG & INFOMSG)
		printf("GGP: opened ip con\n");
#endif
	}

struct ggping {
	char gtype;
	char gcode;
	unshort gseq;
	};

/* GGP packet handler */

ggprcv(p, len, host)
	PACKET p;
	int len;
	in_name host; {
	register struct ip *pip;
	register struct ggping *e;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("GGP: pkt from %a\n", host);
#endif

	pip = in_head(p);
	e = (struct ggping *)in_data(pip);

	if(e->gtype == ECHOREQ) {
		e->gtype = ECHOREP;
		if(in_write(ggp, p, len, host) <= 0) {
#ifdef	DEBUG
			if(NDEBUG & (INFOMSG|PROTERR))
				printf("GGP: reply failed\n");
#endif
			}
#ifdef	DEBUG
		else if(NDEBUG & INFOMSG)
			printf("GGP: sent reply\n");
#endif
		}

#ifdef	DEBUG
	else if(NDEBUG & (PROTERR|INFOMSG))
	  printf("GGP: unhandled pkt %u\n", e->gtype);
#endif

	in_free(p);
	}
