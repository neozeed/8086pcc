/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <netbuf.h>
#include "ether.h"

extern unsigned etint, etunder, etcoll, etcollsx, etrdy, etfcs, etover;
extern unsigned etdribble, etshort, etrcv, etdmadone, etnotidle, etbadma;
extern unsigned etmulti, etdrop, etrcvdma, ettoobig, etref, ettmo, etrreset;

et_stat(fd)
	FILE *fd; {

	fd = stdout;
	fprintf(fd, "Ether Stats\n");
	fprintf(fd, "%u ints\t%u pkts rcvd\t%u ints lost\n",
			 etint, etrcv, etrreset);
	fprintf(fd, "%u underflows\t%u colls\t%u 16 colls\n",
						etunder, etcoll, etcollsx);
	fprintf(fd, "%u rdys\t%u FCS errs\t%u overflows\t%u dribbles\n",
					etrdy, etfcs, etover, etdribble);
	fprintf(fd, "%u shorts\t%u DMAs\t%u not idles\t%u bad DMAs\n",
				etshort, etdmadone, etnotidle, etbadma);
	fprintf(fd, "%u rcv DMAs\t%u pkts too big\t%u refused\n",
						etrcvdma, ettoobig, etref);

	fprintf(fd, "%u pkts dropped\t%u timeouts\n", etdrop, ettmo);
	fprintf(fd, "max q depth %u\n", et_net->n_inputq->q_max);

	etadstat(fd);		/* upcall to address resolution protocol */
#ifndef	WATCH
	in_stats(fd);		/* upcall to internet */
#endif
	}

#ifdef	DEBUG
et_dump(p)
	register PACKET p; {
	register char *data;
	int i;

	data = p->nb_buff;
	for(i=1; i<121; i++) {
		printf("%02x ", (*data++)&0xff);
		if(i%20 == 0) printf("\n");
		}
	puts("");
	}

#endif
