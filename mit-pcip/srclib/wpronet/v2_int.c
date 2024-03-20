/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <netbuf.h>
#include <pro_match.h>
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

/* netwatch variables */
struct pkt pkts[MAXPKT];

int pproc = 0;
int prcv = 0;
long npackets = 0;

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
			register char *data = pkts[prcv].p_data;
			unsigned long type = 0;
			int i;

			if(((pproc - prcv) & PKTMASK) == 1)
				goto punt;

			len = (inb(mkv2(V2ILCNT)) & 0xff) +
			    ((unsigned short)(inb(mkv2(V2IHCNT))&0x07) << 8);

			outb(mkv2(V2ILCNT), 0);
			outb(mkv2(V2IHCNT), 0);

			pkts[prcv].p_dst = inb(mkv2(V2IBUF));
			pkts[prcv].p_src = inb(mkv2(V2IBUF));

			for(i=0; i<4; i++)
				type = (type << 8)+inb(mkv2(V2IBUF));

			pkts[prcv].p_type = type;

			pkts[prcv].p_len = len;

			for(i=0; i<MATCH_DATA_LEN; i++)
				*data++ = inb(mkv2(V2IBUF));

			prcv = (prcv+1) & PKTMASK;
	
	punt:		outb(mkv2(V2ILCNT), 0);
			outb(mkv2(V2IHCNT), 0);
			}
		}

		outb(IOCWR, v2_eoi);
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
