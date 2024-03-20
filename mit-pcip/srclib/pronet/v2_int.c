/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <netbuf.h>
#include "v2.h"

#define	TRUE	1

/* This code services the ethernet interrupt. It is called by an assembly
	language routines which saves all the registers and sets up the
	data segment. */

unsigned v2badfmt = 0;
unsigned v2int = 0;
unsigned v2parity = 0;
unsigned v2overrun = 0;
unsigned v2toobig = 0;
unsigned v2punted = 0;
unsigned v2rcv = 0;
unsigned v2ref = 0;

extern long cticks;

v2_ihnd() {
	unsigned icsr;
	unsigned ocsr;
	unsigned len;
	int i;
	PACKET p;
	register char *data;
	unsigned change=1;

	v2int++;

	while(change) {
	change = 0;

	icsr = inb(mkv2(V2ICSR));
	if(icsr & ININTSTAT) {
		change = 1;
		if(icsr & BADFMT) v2badfmt++;
		else {
		if(icsr & OVERRUN) v2overrun++;
		if(icsr & PARITY) v2parity++;
		if(!(icsr & COPYEN)) {
			len = (inb(mkv2(V2ILCNT)) & 0xff) +
			    ((unsigned short)(inb(mkv2(V2IHCNT))&0x07) << 8);

			if(len > LBUF) v2toobig++;
			else if((p = getfree()) != 0) {
				v2rcv++;
				outb(mkv2(V2ILCNT), 0);
				outb(mkv2(V2IHCNT), 0);

				p->nb_tstamp = cticks;
				p->nb_len = len;
				data = p->nb_buff;
				for(i=0; i<len; i++)
					*data++ = inb(mkv2(V2IBUF));

				q_addt(v2_net->n_inputq, (q_elt)p);
				tk_wake(V2Demux);
				}
			else v2punted++;
	
			outb(mkv2(V2ILCNT), 0);
			outb(mkv2(V2IHCNT), 0);
			}
		}

		outb(IOCWR, v2_eoi);
/*		outb(mkv2(V2ICSR), ININTRES); */
		outb(mkv2(V2ICSR), MODE2|MODE1|ININTRES);
		outb(mkv2(V2ICSR), MODE2|MODE1|COPYEN|ININTEN);
		}

		ocsr = inb(mkv2(V2OCSR));
		if(ocsr & OUTINTSTAT) {
			change = 1;
			if(ocsr & REFUSED) v2ref++;

			outb(IOCWR, v2_eoi);
			outb(mkv2(V2OCSR), OUTINTRES);
			outb(mkv2(V2OCSR), OUTINTEN);
			}
		}
	}
