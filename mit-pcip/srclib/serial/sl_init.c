/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <netbuf.h>
#include <timer.h>

/* This file contains the initialization code for the serial line net for
the PC's.  SL_INIT() has to initialize the serial port, perhaps fill in
some of its net struct and fork the serial line packet to protocol
demultiplexor.  Changed 1/23/84 to use consistent error return codes
from sl_write, and to assign a distinctive IP address (177,177,177,177)
					<J. H. Saltzer>  */

NET	*sl_net;	/* Serial line net descriptor */
Task	SlDemux;	/* The packet-to-protocol demultiplexor. */
static Task slpreq;	/* requesting task */
extern int sl_demux();	/* "		"		" 	 */
extern int sl_slp();	/* Serial Line Protocol Handler */
static int wake_me();


sl_init(net, baud, dummy)
	NET *net;
	unsigned baud;
	unsigned dummy; {
	PACKET p;
	timer *tm;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Initializing Serial Port.\n");
#endif

	baud = custom.c_baud;
	if(baud == 0) {
		printf("The data rate has not been customized! You should ");
		printf("set it using the customizer\nand try again.\n");
		exit(1);
		}

	init_aux(baud);		/* initialize the serial port */

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Forking SLDEMUX. Routine addr = %u.\n", sl_demux);
#endif

	SlDemux = tk_fork(tk_cur, sl_demux, net->n_stksiz, "SLDEMUX");
	tk_yield();

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("SlDemux is %04x.\n", SlDemux);
#endif

	sl_net = net;
	net->n_demux = SlDemux;
	
	/* Now set up the code for the SLP (serial line protocol) handler
		so we can find out our address from the PC gateway and such.
	*/

	p = a_getfree();
	slpreq = tk_cur;
	tm = tm_alloc();
	if(tm == 0) {
		printf("SL_INIT: Couldn't allocate timer.\n");
		exit(1);
		}

	/* Zero out our address so that if someone already set it that won't
		screw us up. */

	sl_net->ip_addr = 0L;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("SL_INIT:  Sending SLP AddrReq to PC Gateway.\n");
#endif

	tm_tset(6*TPS, wake_me, 0, tm);
	if(sl_write(p, SLP, 0) == FAILURE) {
		printf("SL_INIT: Couldn't send Address Request packet.\n");
		net_stats(stdout);
		exit(1);
		}
#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("SL_INIT:  calling block. . .\n");
#endif
	tk_block();

	tm_clear(tm);
	tm_free(tm);

	if(sl_net->ip_addr == 0L)
		return 0;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("SL_INIT:  Received Address Reply.\n");
#endif

	return;
	}


/* Process an incoming SLP packet. If it contains data, use this as our
	address. If it has no data (len = 0) consider it a request
	and send out a phony address. */

sl_slp(p, len, n)
	PACKET p;
	int len;
	NET *n; {
	int num;
	long addr;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("SLP:  Received SLP packet of length %u\n", len);
#endif

	if(len == 4) {
		addr = *((long *)p->nb_prot);
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("SLP: IP addr is %a\n", addr);
#endif

		sl_net->ip_addr = addr;
		wake_me();
		putfree(p);
		}

	else if(len == 0) {
		p->nb_prot[0] = 0177;
		p->nb_prot[1] = 0177;
		p->nb_prot[2] = 0177;
		p->nb_prot[3] = 0177;

		sl_write(p, SLP, 4);
		putfree(p);
		}
	else {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG || NDEBUG & PROTERR)
			printf("SLP: Received bad packet length.\n");
#endif
		putfree(p);
		}

	}

static int wake_me() {
	tk_wake(slpreq);
	}

