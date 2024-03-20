/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <netbuf.h>
#include "llp.h"
#include "llexterns.h"
#include "sl.h"

/* Variables which form the interface between sl_write() and the interrupt
	handling code in sl_bout(). */

char PSEND = FALSE;		/* Global availability flag */
char DOSEND = FALSE;		/* do send the character */
char TACK = FALSE;		/* Xmit acknowledge flag for llp */
char TREQ = FALSE;		/* Xmit request */
static int plen = 0;		/* Data length */
static char *buffpsn;		/* Current buffer position */

/* Statistics counters */
unsigned slsnd = 0;		/* packets transmitted */
unsigned slsesc = 0;		/* how many llp ESC's sent */
unsigned sltmo = 0;		/* number of timeouts to PC Gateway */
unsigned slreq=0, slack=0;	/* # of acks & reqs sent */
unsigned slnotx = 0;		/* # of tx interrupts w/nothing to do */
unsigned slbusy = 0;		/* # of attempts to send while busy */
unsigned slptmo = 0;		/* timeouts during packet send */
extern unsigned badtx;

/* This routine is used at interrupt level and is called to handle outputing
	bytes to the C-Gateway. For efficiency's sake, the buffer that
	has the packet in it is passed to the routine which initiates the
	output by the Internet package. Also, to avoid a copy or munging on
	that packet, this routine does LLP encoding on the fly. The local
	net header is assumed to have already been built.

	SL_BOUT() returns the character which is to be transmitted, or
	a 0xFFFF to indicate that there should be no character transmitted,
	since it is possibe for us to get a txmitter interrupt when we
	don't really want one. */


unsigned sl_bout() {
	static char push = 0;
	static char pushf = 0;
	char wrchr;

	DOSEND = TRUE;

	/* This HAS to be first because it would be very bad to send an ESC
		and then an ACK or REQ, so this piece of code has to be before
		the ACK & REQ handling code */
	if(pushf) {
		wrchr = push;
		pushf = FALSE;
		plen--; }

	else if(TACK) {
		wrchr = ACK;
		slack++;
		TACK  = FALSE; }

	else if(TREQ) {
		wrchr = REQ;
		slreq++;
		TREQ = FALSE; }

	else if(PSEND) {
		if(!plen) {
			wrchr = END;
			PSEND = FALSE;
			slsnd++;
			return wrchr;
			}

		if(*buffpsn >= ESC && *buffpsn <= END) {
			wrchr = ESC;
			pushf = TRUE;
			push  = *buffpsn - ESC;
			slsesc++;
			}
		else {
			wrchr = (*buffpsn);
			plen--;
			}
		buffpsn++;
		}
	else {
		slnotx++;
		DOSEND = FALSE;
		}

	return wrchr;
	}


/* SL_WRITE(buffer, protocol) constructs the low-level header for the packet
	in buffer and then attempts to transmit it. Timeouts are checked
	on. If the attempt was successful, length is returned; otherwise 0. */

extern long cticks;

int sl_write(p, protocol, len)
	PACKET p;		/* packet buffer */
	unsigned protocol;	/* protocol = IP, chaos, etc. */
	unsigned len; {		/* number of bytes in packet */
	long time;
	int retries;
	char *data;

	data = p->nb_buff;
	p->nb_len = len + 4;
#ifdef 	DEBUG
	if (NDEBUG & NETRACE) 
		printf("SL:  Sending packet in protocol %u, length %u\n",
					protocol, p->nb_len);
#endif
	/* Construct local net header for this protocol HERE. */
	switch(protocol) {
	case 0:
		data[0] = 2;
		data[1] = 1;
		data[2] = 0;
		data[3] = 0;
		break;
	case 3:
		data[0] = 2;
		data[1] = 3;
		data[2] = 0;
		data[3] = 0;
		break;
	default:
	     if(NDEBUG & NETERR) 
	     	printf("SL_WRITE:  Not sending packet of unknown protocol %u",
					protocol);
	     return FAILURE;
		}

	/* Wait for the output routines to become unbusy */
	if(PSEND) {
#ifdef	DEBUG
		if(NDEBUG & (NETERR|PROTERR|INFOMSG))
			printf("SL_WRITE: Serial Line was BUSY!!!\n");
#endif
		slbusy++;
		}

     for (retries = 0; retries <= SLRETRIES ; retries++) {
	RACK = FALSE;
	PSEND = FALSE;
	send_req();
	plen = 0;
	time = cticks;
	while(cticks - time < SLTIMEOUT)
		if(RACK) {
			RACK = FALSE;
			plen = len+4;
			buffpsn = p->nb_buff;
			PSEND = TRUE;
			_wake_serial();
			time = cticks;
			while(PSEND)
				if(cticks - time > PTIMEOUT) {
#ifdef	DEBUG
					if(NDEBUG & (PROTERR|NETERR|TMO))
					     printf("SL_WRITE: ptimeout\n");
#endif
					slptmo++;
					PSEND = FALSE;
					return FAILURE;
					}
			return SUCCESS;
			}
	sltmo++;			/* bump timeout counter */
     }
#ifdef	DEBUG
	if(NDEBUG & (TMO|NETERR))
	     printf("SL_WRITE:  PC Gateway timeout, discarding packet.\n");
#endif
	return FAILURE;			/* Indicate timeout */

	}  /*  end of sl_write() */


/* Send an ACK by setting the ACK flag and kicking the transmitter */
_send_ack() {
	TACK = TRUE;
	_wake_serial();
	}

/* Send a REQ by setting the REQ flag and kicking the transmitter */
static send_req() {
	TREQ = TRUE;
	_wake_serial(); }
