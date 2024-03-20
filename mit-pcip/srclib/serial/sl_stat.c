/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <netbuf.h>
#include "sl.h"

/* print some pretty statistics */

extern unsigned slsnd, slsesc, sltmo, slrcv, sldrop, slresc, slrip, slprot;
extern unsigned slslp, slmulti, badtx, slref, slwpp;
extern unsigned slreq, slack, slint, sltint, slrint;
extern unsigned	sllstat,slmstat,intcomp,sliir;
extern unsigned slnotx, slbusy, slptmo;

int int_hnd();

sl_stat(fd)
	FILE *fd; {

	fprintf(fd, "Serial Line Statistics:\n");
	fprintf(fd, "%4u packets rcvd %4u sent %4u dropped %4u refused\n",
					slrcv, slsnd, sldrop, slref);
	fprintf(fd, "%4u PCGW timeouts %4u protocol, %4u req in packet\n",
							sltmo, slprot, slrip);
	fprintf(fd, "%4u ESCs sent\t%4u ESCs received\n", slsesc, slresc);
	fprintf(fd, "%4u SLP packets rcvd %4u reqs sent %4u acks sent\n",
						slslp, slreq, slack);
	fprintf(fd, "ints %4u, condx:  line %4u, rcv %4u, tx %4u, mdm %4u\n",
				slint, sllstat, slrint, sltint, slmstat);
	fprintf(fd, "ints w/out condx:  null %4u, rcv %4u, tx %4u\n",
						sliir, intcomp, badtx);
/*	fprintf(fd, "interrupt vector = %04x = %04x\n", getint(), int_hnd); */
	fprintf(fd, "%4u timeouts during packet send %4u extra wakeups\n",
							 slptmo, slwpp);
	fprintf(fd, "tbusy is %4u\tnot tx is %4u\n",
							slbusy, slnotx);
	fprintf(fd, "Multiple packets on net queue %4u\tMax depth = %4u\n",
					slmulti, sl_net->n_inputq->q_max);
	fprintf(fd, "End Serial Line Statistics.\n\n");
	in_stats(fd);
	}
