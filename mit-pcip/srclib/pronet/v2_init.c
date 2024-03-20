/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <stdio.h>
#include <net.h>
#include "v2.h"

/* This C routine does as much of the initialization at a high level as
	it can. It uses the following routines from the standard i/o library
	(some standard routines these are!) for accessing ports:
		char inb(port)	to input a byte;
		unsigned inw(port)	to input a word. Gets the low byte
					from port, high byte from port+1;
		outb(port, byte)	to output a byte to a port;
		outw(port, word)	outputs low byte to port, hi byte to
					port+1;

	These routines work well with the ethernet controller but not with
	the 8237A DMA chip which wants a different method of handling words.
	The routines outw2() and inw2() are for use with it.
*/

char _v2me;		/* my ethernet address */
task *V2Demux;		/* ethernet packet demultiplexing task */
NET *v2_net;		/* my net pointer */
unsigned v2_eoi;

int v2_demux();		/* the routine which is the body of the demux task */

v2_init(net, options, dummy)
	NET *net;
	unsigned options;
	unsigned dummy; {
	char temp;
	char initb;
	int i;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Forking V2DEMUX.\n");
#endif

	V2Demux = tk_fork(tk_cur, v2_demux, net->n_stksiz, "V2DEMUX");
	v2_net = net;
	v2_net->n_demux = V2Demux;

	/* here we should patch into the interrupt vector and initialize
		the v2lni
	*/
	int_off();
	v2_eoi = 0x60 + custom.c_intvec; /* calculate the eoi command code */
	v2_patch(custom.c_intvec<<2);
	i = custom.c_intvec;	/* silly broken compiler */
	outb(IIMR, inb(IIMR) & ~(1 << i));
	int_on();

	/* reset the lni */
/*	outb(mkv2(V2ICSR), INRST);	*/
	outb(mkv2(V2ICSR), INRST|MODE1|MODE2);
	outb(mkv2(V2OCSR), OUTRST);
	outb(mkv2(V2ILCNT), 0);
	outb(mkv2(V2IHCNT), 0);
	outb(mkv2(V2ICSR), MODE2|MODE1|COPYEN|ININTEN);

	if(NDEBUG & INFOMSG)
	printf("v2lni inited; icsr = %02x, ocsr = %02x\n", inb(mkv2(V2ICSR)),
							inb(mkv2(V2OCSR)));

	/* We should also figure out our local ring address and use this to
		calculate our ring address. This is not trivial: we have to
		put ourself in loopback mode and broadcast a packet to
		ourself and look at the source address.
	*/
/*
	v2_me = 0;

	outb(mkv2(V2OLCNT), (-2)&0xff);
	outb(mkv2(V2OHCNT), ((-2)>>8)&0x07);
	outb(mkv2(V2OBUF), 0xff);
	outb(mkv2(V2OLCNT), (-2)&0xff);
	outb(mkv2(V2OHCNT), ((-2)>>8)&0x07);
*/
	/* assert mode2 */
/*	outb(
	outb(mkv2(V2OCSR), ORIGEN|INITRING|
*/
	tk_yield();	/* Give the per net task a chance to run. */
	return;
	}
