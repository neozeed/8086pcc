/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <net.h>
#include <ether.h>
#include <timer.h>
#include "ether.h"

/* This C routine does as much of the initialization at a high level as
	it can. It uses the following routines from the -lpc library:

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

/* Jan-85 Added task to keep the ethernet controller alive. See
	comments in et_send.c.		<Saltzer, Romkey>
*/

/* define some convenient constants */
/* receive all packets */
#define	RCVALL	ERCVALLADDR | EACCDRIBBLE | EACCGOODFRAMES | EDTFCSERR | EDTSHORTFRAMES | EOVERFLOW | EDTNOOVERFLOW
/* Receive multicast packets */
#define	RCVMULT	ERCVMULTI | EACCDRIBBLE | EACCGOODFRAMES | EDTSHORTFRAMES | EOVERFLOW
/* Receive broadcast, detect all errors */
#define	RCVNORM	ERCVBROAD | EACCDRIBBLE | EACCGOODFRAMES | EDTSHORTFRAMES | EOVERFLOW
/* loopback and DMA/interrupts */
#define	TXLOOP	EINTDMAENABLE | ELOOPBACK

/* storage for lots of things like my ethernet address, the ethernet broadcast
	address and my task and net pointers */
char ETBROADCAST[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
char _etme[6];		/* my ethernet address */
task *EtDemux;		/* ethernet packet demultiplexing task */
NET *et_net;		/* my net pointer */
char etrcvcmd;		/* receiver command byte */
unsigned et_eoi;

int et_demux();		/* the routine which is the body of the demux task */
static int et_poke();
static int et_keepalive();
static task *e_rtk;
static timer *e_rtm;
extern unsigned etrreset;
extern unsigned etint;

et_init(net, options, dummy)
	NET *net;
	unsigned options;
	unsigned dummy; {
	char temp;
	char initb;
	int i;
	union {
		long ulong;
		char uc[4];
		} myaddr;
	int vec;	/* crock to circumvent compiler bug */

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Forking ETDEMUX.\n");
#endif

	EtDemux = tk_fork(tk_cur, et_demux, net->n_stksiz, "ETDEMUX");
	if(EtDemux == NULL) {
		printf("can't fork ether task\n");
		exit(1);
		}

	et_net = net;
	et_net->n_demux = EtDemux;

	tk_yield();

	int_off();		/* Disable interrupts. */

	/* Reset the ethernet controller. */
	outb(EAUXCMD, ERESET);

	/* patch in the new interrupt handler - rather, call the routine to
		do this. This routine saves the old contents of the vector.
	*/
	et_eoi = 0x60 + custom.c_intvec;
	et_patch(custom.c_intvec<<2);

	/* setup interrupts for the specified line */
	vec = custom.c_intvec;
	outb(IIMR, inb(IIMR) & ~(1 << vec));

	/* We need to setup our ethernet address. Do this by reading the
		address from the PROM and writing it back to the controller.
	*/

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("PC Ethernet address = ");
#endif

	switch(custom.c_seletaddr) {
	case HARDWARE:
		for(i=0; i<6; i++) {
			outw(EGPPLOW, i);
			temp = inb(EADDRWIN);
#ifdef	DEBUG
			if(NDEBUG & INFOMSG) printf("%02x", temp&0xff);
#endif
			_etme[i] = temp;
			outb(EADDR+i, temp);
			}
		break;
	case ETINTERNET:
		myaddr.ulong = 	et_net->ip_addr;
		for(i=3; i != -1; i--) {
			_etme[i+2] = myaddr.uc[i];
			outb(EADDR+i+2, myaddr.uc[i]);
			}
		_etme[0] = 0;
		outb(EADDR, 0);
		_etme[1] = 0;
		outb(EADDR+1, 0);

#ifdef	DEBUG
		if(NDEBUG & INFOMSG) {
			for(i=0; i<6; i++)
				printf("%02x", _etme[i]);
			printf("\n");
			}
#endif
		break;
	case ETUSER:
		for(i=0; i<6; i++) {
			_etme[i] = custom.c_myetaddr.e_ether[i];
			outb(EADDR+i, _etme[i]);
#ifdef	DEBUG
			if(NDEBUG & INFOMSG)
				printf("%02x",_etme[i]);
#endif
			}
		break;
	default:
		printf("invalid ethernet address selection option\n");
	}

#ifdef	DEBUG
	if(NDEBUG & INFOMSG) printf("\n");
#endif

	/* turn interrupts on to try to eliminate an insidious race
	   condition. */
	int_on();

	outb(EAUXCMD, EINTDMAENABLE|ESYSBUS);

	/* Initialize the transmitter to not interrupt us at all */
	outb(ETXCMD, 0);

	/* Initialize the receiver to detect no errors and accept
		good packets and dribble errors. */
	if(options & ALLPACK) etrcvcmd = RCVALL;
	else if(options & MULTI) etrcvcmd = RCVMULT;
	else etrcvcmd = RCVNORM;

	/* set up the receiver once */
	outb(ERCVCMD, etrcvcmd);
	inb(ERCVCMD);		/* staleify the receiver status */
	outb(ECLRRP, 0);

	outb(EAUXCMD, EINTDMAENABLE|ERECEIVE);

	/* start up a task which periodically kicks the receiver to
		keep it alive
	*/
	e_rtm = tm_alloc();
	if(e_rtm == NULL) {
		printf("can't alloc ether timer\n");
		exit(1);
		}

	e_rtk = tk_fork(tk_cur, et_keepalive, 400, "Keepalive");
	if(e_rtk == NULL) {
		printf("can't fork ether keepalive task\n");
		exit(1);
		}

	/* Now everything is initialized. The DMA channel should only be
		initialized on demand, so it's not necessary to touch it
		now.
	*/
	tk_yield();	/* Give the per net task a chance to run. */

	/* init arp */
	etainit();
	return;
	}

static et_poke() {
	tk_wake(e_rtk);
	}

static et_keepalive() {
	unsigned int_cnt;
	while(1) {
		tm_set(3, et_poke, NULL, e_rtm);
		tk_block();
		int_cnt = etint;
		et_ihnd();
		if (int_cnt != etint) ++etrreset;
		}
	}

