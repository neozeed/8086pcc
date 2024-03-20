/*  Copyright 1984, 1985 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/*  Modifed 1/16/84 to timestamp incoming packets.  <J. H. Saltzer>
	1/25/84 - added code to support netwatch. <John Romkey>
	7/6/84 -  changed code to handle runt packets and not go deaf
		to the net; moved some variable declarations from et_demux
		to here.			<John Romkey>
	7/9/84 - changed driver to handle runt packet condition and
		reset the controller properly. Involved switching the packet
		buffer to the bus and then back to receive to get the
		receiver going again. Driver should no longer go deaf
		to the net.			<John Romkey>
*/

#include <netbuf.h>
#include <ether.h>
#include "ether.h"

#ifdef	WATCH
#include <match.h>
#endif

#define	TRUE	1
#define	NULL	0

extern 	long	cticks;

/* This code services the ethernet interrupt. It is called by an assembly
	language routines which saves all the registers and sets up the
	data segment. */

unsigned etint = 0;
unsigned etunder = 0;
unsigned etcoll  = 0;
unsigned etcollsx = 0;
unsigned etrdy = 0;
unsigned etfcs = 0;
unsigned etover = 0;
unsigned etdribble = 0;
unsigned etshort = 0;
unsigned etrcv = 0;
unsigned etdmadone = 0;
unsigned etrcvdma = 0;
unsigned etnotidle = 0;
unsigned etbadma = 0;
unsigned etref = 0;
unsigned ettoobig = 0;

/*unsigned ettxstate = NONBUSY;	/* xmitter current state variable */
/*unsigned etrcvstate = NONBUSY;	/* receiver current state variable */

long etlastpkttime = 0;


#ifdef	WATCH
struct pkt pkts[MAXPKT];

int pproc = 0;
int prcv = 0;
long npackets = 0;

#endif

extern unsigned etdmstart;	/* starting location on ET of packet */
extern unsigned etrreset;

unsigned etwpp = 0;	/* number of times awakened w/o packet to process */
unsigned etdrop = 0;	/* # of packets dropped */
unsigned etmulti = 0;	/* # of times more than one packet on queue */

extern unsigned etrcvcmd;

#define	DMMRST1	0x05
#define	DMMSET1	0x01
#define	DMWRITE1 0x45

#define	IIMR2	0xa1	/* AT's second interrupt controller */

et_ihnd() {
	char rcv;
	char orcv;
	unsigned len;


	while(!((rcv = inb(ERCVSTAT)) & ESTALESTAT)) {
		etint++;
		if(rcv & EOVERFLOW) {
			etover++;
			goto rcv_fixup;
			}
		else if(rcv & EDRIBBLEERR) etdribble++;
		else if(rcv & EFCSERR) etfcs++;
		else if(rcv & ESHORTFRAME) {
			etshort++;
	rcv_fixup:	outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);
			outb(EAUXCMD, EINTDMAENABLE|ERECEIVE);
/*			outb(ERCVCMD, etrcvcmd);	*/
			outb(ECLRRP, 0);
/*			inb(ERCVSTAT);			*/
			}
		else if(rcv & EGOODPACKET) {
#ifdef	WATCH
			register char *data = pkts[prcv].p_dst;
			int i;

			etlastpkttime = cticks;
			etrcv++;
			outw(EGPPLOW, 0);
			outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);
			if(((pproc - prcv) & PKTMASK) != 1) {
				for(i=0; i<14; i++)
					*data++ = inb(EBUFWIN);

				pkts[prcv].p_len = inw(ERBPLOW);
				outw(EGPPLOW, start);
	
				data = pkts[prcv].p_data;
				for(i=0; i<MATCH_DATA_LEN; i++)
					*data++ = inb(EBUFWIN);

				prcv = (prcv+1)&PKTMASK;
				tk_wake(EtDemux);
				}

			outb(EAUXCMD, EINTDMAENABLE|ERECEIVE);
			outb(ERCVCMD, etrcvcmd);
			outb(ECLRRP, 0);
#endif

#ifndef	WATCH
			PACKET et_inp;

			etrcv++;

			outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);

			len = inw(ERBPLOW);
			if(len > LBUF) {
				ettoobig++;
		punt_rcv:	outb(EAUXCMD, EINTDMAENABLE|ERECEIVE);
				outb(ERCVCMD, etrcvcmd);
				outb(ECLRRP, 0);
				continue;
				}

			et_inp = getfree();
			if(et_inp == NULL) {
				etref++;
				goto punt_rcv;
				}

			etdma(et_inp->nb_buff);
			outw2(DMCOUNT1, len);
			outw(EGPPLOW, 0);
			et_inp->nb_len = len;
			et_inp->nb_tstamp = -1;
			outb(DMMODE, DMWRITE1);
			outb(DMSMASK, DMMSET1);

			outb(EAUXCMD, EINTDMAENABLE|EDMAREQ|ESYSBUS);

			while(!(inb(EAUXSTAT) & EDMADONE)) ;

			etdmadone++;
			etrcvdma++;
			outb(DMSMASK, DMMRST1);
			outb(EAUXCMD, EINTDMAENABLE|ERECEIVE);
/*			outb(ERCVCMD, etrcvcmd);	*/
			outb(ECLRRP, 0);
			et_inp->nb_tstamp = cticks;
			q_addt(et_net->n_inputq, (q_elt)et_inp);
			tk_wake(EtDemux);
#endif
			}
		}
	}
