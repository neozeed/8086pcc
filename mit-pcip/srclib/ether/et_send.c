/*  Copyright 1984, 1985 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <netbuf.h>
#include <ether.h>
#include "ether.h"
#include <stdio.h>

/* Transmit a packet. If it's an IP packet, calls ARP to figure out the
	ethernet address. Disables receiver, DMAs packet from memory to
	card, then transmits packet. Performs timeout checking on the
	DMA transfer and the packet send. Send timeout probably means a
	collision.
   There seems to be a problem losing interrupts on the PC/AT, so the
	transmit routine no longer depends on interrupts at all. Even
	so, we still lose receive interrupts, and we theorize that
	there's a small window at the beginning of the transmit routine
	where when we turn off the receiver we've got a race condition
	between turning it off and it giving us an interrupt because it
	received a packet. We couldn't fix the lost interrupt problem,
	so we finally had to add the task spawned by et_init which pokes
	the controller every 3 seconds.
   When we did do transmits without fixing up the receiver, the
	ethernet board got horribly confused and stopped giving us any
	interrupts at all. A general reset to the board seemed necessary
	to get it unconfused.
*/


/* 19-Aug-84 - changed et_send to send packets sent to x.x.x.0 to the
	ethernet broadcast address.
						<John Romkey>
   Jan-85 Fixed lost interrupts, changed send routine to never turn off
	DMA enable bit.				<Saltzer, Romkey>
*/

#define	DMREAD1		0x49	/* Demand mode, increment, no init, read */
#define	DMMSET1		0x01	/* Mask dma channel 1 on */
#define	DMRSET1		0x05	/* Mask dma channel 1 off */

#define	ETTIMEOUT	5	/* timeout in 1/18 secs for sending pkts */

extern unsigned ettxcmd;
extern unsigned etrcvcmd;

unsigned etrreset;

unsigned etdmstart;
unsigned ettmo = 0;

#ifdef	WATCH
unsigned etminlen;
#endif

extern long cticks;

et_send(p, prot, len, fhost)
	PACKET p;
	unsigned prot;
	unsigned len;
	in_name fhost; {
	register struct ethhdr *pe;
	unsigned temp;
	unsigned i;
	unsigned ettxstate = NONBUSY;
	long time;
	union {
		char _bytes[4];
		in_name _long;
		} _address;

	/* Set up the ethernet header. Insert our address and the address of
		the destination and the type field in the ethernet header
		of the packet. */
#ifdef	DEBUG
	if(NDEBUG & (INFOMSG|TRACE))
		printf("ET_SEND: p[%u] -> %a.\n", len, fhost);
#endif

	pe = (struct ethhdr *)p->nb_buff;
	etadcpy(_etme, pe->e_src);

	/* Setup the type field and the addresses in the ethernet header. */
	switch(prot) {
	case IP:
		_address._long = fhost;
		if(_address._bytes[3] == 0) {
			etadcpy(ETBROADCAST, pe->e_dst);
			}
		else if(ip2et(pe->e_dst, (in_name)fhost) == 0) {
#ifdef	DEBUG
			if(NDEBUG & (INFOMSG|NETERR))
				printf("ET_SEND: ether address unknown\n");
#endif
			return 0;
			}
		pe->e_type = ETIP;
		break;
	case ADR:
		etadcpy((char *)fhost, pe->e_dst);
		pe->e_type = ETADR;
		break;
	default:
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR|BUGHALT))
			printf("ET_SEND: Unknown prot %u.\n", prot);
#endif
		return 0;
		}

	len += sizeof(struct ethhdr);
#ifdef	WATCH
	if(len < etminlen)
		len = etminlen;
#endif
#ifndef	WATCH
	if(len < ETMINLEN)
		len = ETMINLEN;
#endif


/*	int_off();		/* just in case... */

	/* before touching anything, check & see if we've encountered
		an obscure condition where we seem to have lost a
		receive interrupt. If it looks like this has happened,
		reset the board.
	   ***Update. According to what I now know, the buffer status
		does not automatically switch from XMTRCV to RCV when the
		transmit completes. This means that this reset will
		occur whenever we send two packets without receiving one
		between the two sends. So this test is bogus, but if
		I remove it the code seems to fail sometimes.
	*/
/*	if((inb(EAUXSTAT) & 0x0C) == EXMTRCV) {
		etrreset++;
		outb(EAUXCMD, ERESET|EINTDMAENABLE|ESYSBUS);
		outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);
		}
*/

	/* Turn off the receiver
		Have to leave the Interrupt & DMA Enable bit set when
		doing this because of an obscure problem with systems
		with the expansion chassis. On these systems, if you
		just write a 0 to the AUXCMD register and later enable
		DMA at the same time as you do a DMA request, you lose
		and no DMA occurs. It's fine to write a 0 to the AUXCMD
		register otherwise. The whole reason we do this is to
		guarantee that the receiver will go off.
	*/

	outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);
/*	int_on();		*/

	/* Point the DMA controller at it. We use channel one */
	outb(DMBYTEP, 0);		/* byte pointer flip/flop */

	etdma(pe);
	outw2(DMCOUNT1, len);

	/* Set GP */
	etdmstart = 2048-len;
	outw(EGPPLOW, etdmstart);

	outb(DMMODE, DMREAD1);	/* Set the mode */
	outb(DMSMASK, DMMSET1);	/* Mask this channel active */

	outb(EAUXCMD, EINTDMAENABLE|EDMAREQ|ESYSBUS);

	/* We should now be transmitting the packet */
	time = cticks;

	while(!(inb(EAUXSTAT) & EDMADONE))
		if(cticks - time > ETTIMEOUT) {
			ettmo++;
			ettxstate = TIMEOUT;
			if(NDEBUG & (INFOMSG|PROTERR|NETERR))
				printf("et_send: dma tmo\n");

			/* reset the DMA controller & net interface */
			outb(DMSMASK, DMRSET1);

			break;
			}

	if(ettxstate != TIMEOUT) {
	time = cticks;

	outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);

#define	DMMRST1	0x05
	outb(DMSMASK, DMMRST1);
	outw(EGPPLOW, etdmstart);

	outb(EAUXCMD, EINTDMAENABLE|EXMTRCV);

	/* have to get things ready for the receiver here */
	outb(ECLRRP, 0);
/*	outb(ERCVCMD, etrcvcmd);	*/

	while(inb(EAUXSTAT) < 0) {
		if(cticks - time > ETTIMEOUT) {
			ettmo++;
			ettxstate = TIMEOUT;
			if(NDEBUG & (INFOMSG|PROTERR|NETERR))
				printf("et_send: net tmo\n");
			break;
			}
		}

/*	if(!(inb(EAUXSTAT) & ERCVBUSY)) {
		etrreset++;
		outb(EAUXCMD, ERESET|EINTDMAENABLE|ESYSBUS);
		outb(EAUXCMD, EINTDMAENABLE|ERECEIVE);
		}
*/

	}


	switch(ettxstate) {
	case TIMEOUT:
		ettxstate = NONBUSY;
		return 0;
	case NONBUSY:
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("ET_SEND: pkt sent\n");
#endif
		return len-sizeof(struct ethhdr);
	default:
#ifdef	DEBUG
		printf("ET_SEND: bad state: %u\n", ettxstate);
#endif
		return 0;
		}
	}
