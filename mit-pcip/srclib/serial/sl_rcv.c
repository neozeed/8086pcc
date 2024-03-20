/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/*  Modified 1/16/84 to timestamp arriving packets.  <J. H. Saltzer>  */

#include <stdio.h>
#include <netbuf.h>
#include "llp.h"
#include "sl.h"
extern long cticks;

/* Some internals... */

char INBUSY = FALSE;	/* Are we inputting a packet? */
PACKET RBUFF;	/* The buffer */
char RACK = FALSE;		/* It is the higher level routine's
				responsibility to clear the Received Ack
				flag after it has used it. */

/* Statistics counters */
unsigned badtx = 0, serint = 0;		/* Counts of bad serial port events. */
unsigned slrcv = 0;		/* number of packets received */
unsigned sldrop = 0;		/* number of packets dropped */
unsigned slresc = 0;		/* number of llp ESCs received */
unsigned slrip  = 0;		/* number of llp REQs in packets */
unsigned slref  = 0;		/* number of packets refused from PC GW */


/* sl_bin() reads a packet from the serial port from the gateway into
	the buffer pointed to by RBUF. It sets the global variable RLEN
	to the length of the packet.	*/

sl_bin(rdchr)
	register char rdchr;{
	static char *buffpsn;
	static int len;
	static char escflag = FALSE;

	if(rdchr == REQ) {
		if(INBUSY) {
			slrip++;
			sldrop++;	}
		else if((RBUFF = getfree()) == nullbuf) {
			slref++;
			return;	}
		INBUSY = TRUE;
		buffpsn = RBUFF->nb_buff;	/* Find buffer */
		len = 0;
		_send_ack(); }

	else if(rdchr == ACK) RACK = TRUE;	/* Set the ACK flag...*/
	else if(!INBUSY) return;	/* But we were waiting for a REQ! */

	/* If chr = END then an entire packet has been received. Timestamp
		it, enqueue it and wake up the handler task. */

	else if(rdchr == END) {		/* All done */
		RBUFF->nb_len = len;
		RBUFF->nb_tstamp = cticks;
		q_addt(sl_net->n_inputq, (q_elt)RBUFF);
		INBUSY = FALSE;
		tk_wake(SlDemux);	/* Wake the demux task */
		slrcv++;	}
	else if(rdchr == ESC) {		/* Deal with the ESC char */
		escflag = TRUE;
		slresc++;	}
	else if(escflag) {
		*buffpsn++ = ESC + rdchr;
		len++; 
		escflag = FALSE; }

	else {
		*buffpsn++ = rdchr;
		len++; }

	return; }
