/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* DCtcp.c */


/* Translation of Dave Clark's Alto TCP into C.
 *
 * This version uses the new PC Internet.
 * This file contains a bare-bones minimal TCP, suitable only for user Telnet
 * and other protocols which do not need to transmit much data.  Its primary
 * weakness is that it does not pay any attention to the window advertised
 * by the receiver; it assumes that it will never be sending much data and
 * hence will never run out of receive window.  Its other major weakness
 * is that it will not handle out-of-sequence packets; any out-of-sequence
 * data received is ignored.
 *
 * This TCP requires the UNIX tasking package to run.  It runs as two tasks:
 * TCPsend, which handles all data transmission, and TCPprocess, which
 * processes the incoming data from the circular buffer. The data is placed
 * in the circular buffer by tcp_rcv() which is upcalled from internet on an
 * arriving packet.
 *
 * Tinkered to use different retransmit time on initial request and later
 * retransmissions; to supply trace info for line 25; to postpone acks only
 * if there has been one recently; and to use tick timer.
 * 							<J. H. Saltzer> 12/83.
 * Trace info extended to include retry counts.  <J. H. Saltzer> 1/84.
 *
 * 3/21/84 - corrected several places in the code where last_offset and
 * first_offset were referenced as last_off and first_off.
 *						<John Romkey>
 * 5/29/84 - changed tcp_init() to pass the stksiz argument to netinit().
 * 						<John Romkey>
 * 7/23/84 - made the "unsent data acked" condition dump more information
 *	and attempt to fix up the problem.	<John Romkey>
 * 8/13/84 - removed support for tcp_restore().
 *						<John Romkey>
 * 10/2/84 - changed free[] to be static to avoid a name conflict with
 *	the new i/o library.			<John Romkey>
 * 12/4/84 - added tcp_reset() entry.
 *						<J. H. Saltzer>
 */

typedef	long	time_t;			/* ugly! */

#include <tcp.h>
#include <timer.h>

#define ACKDALLY	15		/* wait between ACKs (ticks) */
#define	INITRT		8		/* retry time--initial request */
#define	CONNRT		2		/* retry time--after connected*/
#define MAXBUF		500		/* maximum output buffer size */
#define	ICPTMO		5		/* initial conn. timeout */
#define	TCPSTACK	2000		/* tcp tasks stack sizes */
#define	TCPWINDOW	1000		/* normal advertised window */
#define	TCPLOWIND	500		/* low water mark on window */
#define TOTALCHUNKS	20
#define BUFSIZE		0x0800		/* 2048 byte buffer  */
#define BUFMASK		0x07FF

int sndtrace = 0;		/* Trace all tcp sends */

int	conn_state;		/* connection state */
int	blk_inpt;		/* prevents new data after close req */
int	still_data;		/* There are still incoming pkts to process. */
PACKET	opbi;				/* ptr to output packet buffer */
timer	*tcptm;				/* The tcp timer */
timer	*tmack;			/* ack timer */

struct	tcp	*otp;			/* ptr to output pkt tcp hdr */
char *odp;		/* ptr to start of output pkt data */
struct	tcpph	*ophp;		/* tcp psuedo hdr for cksum calc */
struct  tcpph   *iphp;		/* tcp psuedo hdr for cksum calc */

unsigned	tcp_src_port;
unsigned	tcp_dst_port;
in_name		tcp_fhost;
unsigned	tcp_window;
unsigned	tcp_low_window;

unsigned	tcppsnt;		/* number of packets sent */
unsigned	tcpprcv;		/* number of packets received */
unsigned	tcpbsnt;		/* number of bytes sent */
unsigned	tcpbrcv;		/* number of bytes received */
unsigned	tcprack;		/* # of bytes received and acked */
unsigned	tcpsock;		/* # of packets not for my socket */
unsigned	tcpresend;		/* # of resent packets */
unsigned	tcprercv;		/* # of old packets received */
unsigned	tcpous;			/* # of out-of-sequence packets */
unsigned	bd_chk;		/* Number of pkts rcvd with bad checksums.*/
unsigned	ign_win;	/* Num pkts rcvd that ignored our window. */
unsigned	frn_win;		/* Size of foreign window. */
long	ack_time;			/* When last ACK was sent.  */
int	odlen;				/* bytes of data in output pkt */
int	dally_time;			/* time to delay ack (ticks) */
int	retry_time;			/* retransmission timeout (ticks)*/
int	newsend;			/* flag indicating new data to send */
int	resend;			/* flag indicating data must be resent */

int	taken;
int	avail;
int	first_offset;
int	last_offset;
int	start_offset;
int	end_offset;
int	fin_offset;
int 	fin_rcvd;
int	numchunks;
int	maxchunks;
int	lastchunk;
int	ceilchunk;
int	first[TOTALCHUNKS];
int	last[TOTALCHUNKS];
static int	free[TOTALCHUNKS];
char	circbuf [BUFSIZE];

IPCONN	tcpfd;			/* connection ID used when calling Internet */
in_name	lochost;			/* local host address */
task	*TCPsend;			/* tcp sending task */
task	*TCPprocess;			/* TCP data processing task */

int	TCPDB2;
int	TCPDB3;
int	(*tc_ofcn)();			/* user function called on open */
int	(*tc_dispose)();		/* user function to receive data */
int	(*tc_yield)();			/* Predicate set when user must run. */
int	(*tc_cfcn)();			/* user function called on close */
int	(*tc_tfcn)();			/* user function called on icp tmo */
int	(*tc_rfcn)();		/* user function called on icp resend */
int	(*tc_buff)();		/* user function to upcall when output buffer
					space is available */

int	tmhnd(), tcp_send(), tcp_rcv(), tcp_process(), tcp_ack();

extern unsigned cticks;

/* This routine is called to initialize the TCP.  It starts up the tasking
 * system, initiates the timer, TCPsend, and TCPrcv tasks, and sets up
 * the pointers to the up-callable user routines for open, received data, close
 * and timeout.  It does not attempt to open the connection; that function
 * is performed by tcp_open().  
 * When it returns, the caller is running as the first task, on the primary
 * process stack.
 */

tcp_init(stksiz, ofcn, infcn, yldfcn, cfcn, tmofcn, rsdfcn, buff)
	int	stksiz;			/* user task stack size (bytes) */
	int	(*ofcn)();		/* user fcn to call on open done */
	int	(*infcn)();		/* user fcn to process input data */
	int	(*yldfcn)();	      /* predicate set when user needs to run*/
	int	(*cfcn)();		/* user fcn to call on close done */
	int	(*tmofcn)(); 		/* user fcn to call on icp timeout */
	int	(*rsdfcn)(); 		/* user icp resend function */
	int	(*buff)(); {		/* user output buffer room upcall */

	tc_ofcn = ofcn;			/* save user fcn addresses */
	tc_dispose = infcn;
	tc_yield = yldfcn;
	tc_cfcn = cfcn;
	tc_tfcn = tmofcn;
	tc_rfcn = rsdfcn;
	tc_buff = buff;
	
	conn_state = CLOSED;		/* initially, conn. closed */
	retry_time = INITRT*TPS;
	TCPDB2 = 0;
	TCPDB3 = 0;
	
	Netinit(stksiz);
	in_init();				/* initialize internet*/
	IcmpInit();
	GgpInit();
	UdpInit();
	nm_init();

	/* start send and */
	tcptm = tm_alloc();
	if(tcptm == 0) {
		printf("TCP: Couldn't allocate timer.\n");
		exit(1);
		}

	tmack = tm_alloc();
	if(tmack == 0) {
		printf("TCP: Couldn't allocate ack timer.\n");
		exit(1);
		}

	TCPsend = tk_fork(tk_cur, tcp_send, TCPSTACK, "TCPsend");
	TCPprocess = tk_fork(tk_cur, tcp_process, TCPSTACK, "TCPprocess");

	/* let the other tasks get started */
	tk_yield();
	}


/* This routine forms the body of the TCP data receiver task.  It blocks
 * on the arrival of network input (as indicated by the arrival of a
 * SIGAIO signal).  It then attempts to read and process incoming packets.
 * The processing of each packet is divided into three phases:
 *	1) Processing acknowlegments.  This involves "shifting" the data
 *	   in the output packet to account for acknowledged data.
 *	2) Processing state information: syn's, fin's, urgents, etc.
 *	3) Processing the received data.  This is done by calling the
 *	   user's "input data" function(specified in his call to
 *	   tcp_init), passing it the address and length of the received
 *	   data.
 */

tcp_rcv(inpkt, len, fhost)
	PACKET  inpkt;		/* the packet itself */
	int	len;		/* length of packet */
	in_name fhost; {	/* source of packet */
	struct	tcp *itp;	/* input pkt tcp hdr */
	char *idp;		/* input pkt data ptr */
	int	needed_acking;	/* number of outstanding bytes */
	int	acked;		/* # outstanding bytes acked by this packet */
	int	data_acked; 	/* # outstanding bytes acked by this packet */
				/* number of previously received seq. numbers*/
	int	prev_rcvd; 		/* (data + fin bit) in current packet*/
	int	idlen;			/* len of input pkt in bytes */
	int	i;
	int 	limit;
	unsigned tempsum;		/* temp variable for a checksum */
	
	/* Process all available input packets */
	
	itp = (struct tcp *)in_data(in_head(inpkt));
	idp = (char *)itp + (itp->tc_thl << 2);

	/* get incoming data length */
	idlen = len - (itp->tc_thl << 2);
	tcpprcv++;			/* another packet received */

	/* If the user wishes to send data, give up the processor. */
	if((*tc_yield)()) {
		still_data = 1;	   /* There's still data to process. */
		tk_yield();
		still_data = 0;
		}

	/* compute incoming tcp checksum here... */
	if(idlen >= 0) *(idp +idlen)= 0;
	iphp->tp_len = idlen + (itp->tc_thl << 2);
	iphp->tp_len = swab(iphp->tp_len);
	iphp->tp_dst = in_mymach(fhost);
	iphp->tp_src = fhost;
	tempsum = itp->tc_cksum;
	itp->tc_cksum = cksum(iphp, sizeof(struct tcpph) >> 1, 0);
	itp->tc_cksum = ~cksum(itp, ((idlen + 1) >> 1) + (itp->tc_thl << 1),0);

	if(itp->tc_cksum != tempsum) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|NETERR|PROTERR))
			printf("TCP: Bad xsum was %4x should have been %4x.\n",
						tempsum, itp->tc_cksum);
#endif
		bd_chk ++;
		in_free(inpkt);
		return; 
		}

	tcp_swab(itp);

	if(itp->tc_dstp != tcp_src_port || itp->tc_srcp != tcp_dst_port) {
#ifdef	DEBUG
		if(NDEBUG & (PROTERR|NETERR|INFOMSG))
			printf("TCP: pkt for prt %u from %a\n",
							itp->tc_dstp, fhost);
#endif
		tc_clrs(inpkt, fhost);
		tcpsock++;
		in_free(inpkt);
		return;
		}

	if(itp->tc_rst) {	/* other guy's resetting me */
		cleanup("foreign reset");
		(*tc_cfcn)();
		in_free(inpkt);
		return;
		}
		
/* incoming packet processing is dependent on the state of the connection */

	if(NOT_YET(conn_state, ESTAB)) {
		/* opening the connection; incoming packet must have syn */
		if(!itp->tc_syn) {
			printf("\nThe foreign host thinks there is still a ");
			printf("valid connection.\nI will try to close it.\n");
			tc_clrs(inpkt);
			in_free(inpkt);
			return;
			}

	/* Also, ack must be for our initial sequence number - namely, 1 */
		if(itp->tc_ack != 1) {
			printf("\nThe foreign host is trying to open ");
			printf("a connection\nto us at the same time as we ");
			printf("are to it. Perhaps you should quit.\n\n");
			tc_clrs(inpkt);
			in_free(inpkt);
			return;
			}

	/* Connection successfully opened.  Call the user and tell him. */

		otp->tc_syn = 0;
		otp->tc_rst = 0;
		otp->tc_fack = 1;
		otp->tc_psh = 1; 
		otp->tc_seq = 1;
		itp->tc_seq ++;	/* syn's take sequence no. space */
		otp->tc_ack = itp->tc_seq;
		tcpbrcv = 1;
		frn_win = itp->tc_win;
		conn_state = ESTAB;
		retry_time = CONNRT*TPS;
		(*tc_ofcn)();

	/* Our initial request was acknowledged  and there is not yet
		new data so reset the timer */
		newsend = TRUE;
		tm_clear(tcptm);
		tk_wake(TCPsend);
		} 

	/* This code updates things based on incoming ack value */

	if(itp->tc_fack) { 
		acked = (int)(itp->tc_ack - otp->tc_seq);
		needed_acking = odlen + (int)otp->tc_fin;
		data_acked = ((acked == needed_acking) ? (acked - (int)otp->tc_fin) : acked);
		/* Ack for unsent data prompts us to reset. */
		if(acked > needed_acking) {
/*			otp->tc_rst = 1;	*/
			printf("TCP_RCV: unsent data acked;");
			printf("opt seq = %08X, inp ack = %08X\n",
						otp->tc_seq, itp->tc_ack);
			printf("\ttrying to fix\n");
			otp->tc_seq = itp->tc_ack;
			tk_wake(TCPsend);
			in_free(inpkt);
			return;
			}

		if(acked > 0) {
		/* So now we have some free output buffer space. Upcall
			the client and tell him so. */
			if(odlen >= MAXBUF) (*tc_buff)();

		/* Account for ack of our urgent data by updating the
				urgent pointer */
			if(otp->tc_furg) {
				otp->tc_urg = otp->tc_urg - acked;
				if(otp->tc_urg <= 0) {
					otp->tc_urg = 0;
					otp->tc_furg = 0;
					}
				}
				
/* See if all our outgoing data is now acked.  If so, turn off resend timer */

			if(acked == needed_acking) {
				newsend = FALSE;
				resend = FALSE;
				tm_clear(tcptm);
				if(otp->tc_fin) {
					otp->tc_fin = 0;
					switch(conn_state) {
					case FINSENT:
						conn_state = FINACKED;
						break;
					case SIMUL:
						conn_state = TIMEWAIT;
						break;
					case R_AND_S:
						in_free(inpkt);
						(*tc_cfcn)();
						return;
					default:
						printf("tcp_rcv: unexpected state: %5d",conn_state);
					}
				}
			}

/* Otherwise, shift the output data in the packet to account for ack */
			odlen -= data_acked;
			shift(odp+data_acked, odp, odlen);
			odp[odlen] = 0;

/* update the sequence number */
			frn_win = itp->tc_win;
			otp->tc_seq += acked;
		}
	}

/* Now process the received data */

/* Check if the incoming data is over the top of our window */
	if((itp->tc_seq + idlen) > (otp->tc_ack + otp->tc_win)) {
		ign_win++;
		/* ignore excess data */
		idlen -= (itp->tc_seq + idlen - (otp->tc_ack + otp->tc_win));
	}
/* Calculate the number of previously received sequence numbers in the current packet */
		
	start_offset = -(prev_rcvd = otp->tc_ack - itp->tc_seq);
	end_offset = start_offset + idlen - 1;
	if(end_offset >= sizeof(circbuf)) 
		end_offset = sizeof(circbuf) - 1;
	if(itp->tc_fin == 1) {
		fin_rcvd = TRUE;
		fin_offset = end_offset;
	}

	if(end_offset < avail) { /*If all incoming info is old.*/
		tcprercv++;
		in_free(inpkt);
		return;
		}

	for(i = max(0, prev_rcvd); i < idlen; i++)
		circbuf[(taken + 1 + start_offset + i) & BUFMASK] = *(idp+i);

	if(numchunks == 0) {
		if(start_offset <= avail+1) 	/* DDC CHANGE */
			avail = end_offset;
		else {
			first[0] = start_offset;
			last[0]  = end_offset;
			free[0]  = FALSE;
			numchunks = 1;
			maxchunks = 0;
			lastchunk = 0;
			}
		}
	else {
		if((lastchunk > -1) && (last[lastchunk] + 1 == start_offset))
			last[lastchunk] = end_offset;
		else {
			for(i = 0; i <= maxchunks; i++) {
				if(free[i]) continue;
				if(last[i]+1 < start_offset) continue;
				if(first[i] > end_offset + 1) continue;
				end_offset = max(end_offset, last[i]);
				start_offset = min(start_offset,first[i]);
				free[i] = TRUE;
				numchunks--;
				if(numchunks == 0) {
					maxchunks = -1;
					break;
					}
				}

			if(free[lastchunk]) lastchunk = -1;
			if(start_offset <= 0) avail = end_offset;
			else {
				limit = min(maxchunks+1, TOTALCHUNKS-1);
				for(i=0; i<=limit; i++) {
					if(!free[i]) continue;
					free [i] = FALSE;
					first[i] = start_offset;
					last [i] = end_offset;
					numchunks++;
					if(i == maxchunks + 1) maxchunks = i;
					if(i > ceilchunk) ceilchunk = i;

					if(lastchunk == -1)
						lastchunk = i;
					break;
					}
				}
			}
		}
	
	/* If there's data to process, wake up the data processing task. */
	if(avail >= 0 || fin_offset == -1)
		tk_wake(TCPprocess);

	in_free(inpkt);
	return;
	}

/* Process some received data for tcp. This is the task counterpart of
	tcp_rcv. */

tcp_process() {
	int i;
	unsigned endlen;

	/* forget about the initial wakeup */
	tk_block();

	while(1) {

	if((avail >= 0) || (fin_offset == -1)) {
		if(in_more(tcpfd)) {
			tk_yield();
			continue;
			}

		last_offset = avail;
		first_offset = 0;

		last_offset = min(avail, first_offset+20);
		for(i=first_offset; i<=last_offset; i++)
			(*tc_dispose)(circbuf+((taken+1+i)&BUFMASK), 1, 0);

		first_offset = last_offset + 1;
		if(last_offset != avail) tk_wake(tk_cur);

		for(i = maxchunks; i >=0; i--) {
			if(free[i]) {
				if(i == maxchunks) maxchunks--;
				continue;
				}
			first[i] -= last_offset + 1;
			last[i]  -= last_offset + 1;
			}

		if(fin_rcvd) fin_offset = fin_offset-last_offset-1;

		taken = (taken + last_offset + 1) & BUFMASK;
		avail -= last_offset + 1;

		if(fin_rcvd) {
			if(fin_offset == -1) {
		/* foreign close request prompts connection state change */
				switch(conn_state) {
				case ESTAB:
					conn_state = FINRCVD;
					otp->tc_fin = 1;
					conn_state = R_AND_S;
					break;
				case FINSENT:
					conn_state = SIMUL;
					break;
				case FINACKED:
					conn_state = TIMEWAIT;
					break;
				default:
					printf("TCP_RCV: bad state: %5d\n",
								conn_state);
				}

				/* ack foreign close request */	
				otp->tc_ack++; /*fin's take up seq. no. space*/
				tcpbrcv ++;
				tk_wake(TCPsend);	
				}
			}
		otp->tc_ack += last_offset + 1;
		tcpbrcv     += last_offset + 1;
		otp->tc_win -= last_offset + 1;
		if(otp->tc_win < tcp_low_window) tk_wake(TCPsend);

		if(avail == last_offset && !in_more(tcpfd)) tk_wake(TCPsend);

		tm_clear(tmack);
		dally_time = ack_time - cticks;
		if (dally_time > 0) tm_tset(dally_time, tcp_ack, 0, tmack);
			       else tk_wake(TCPsend);
		}
		tk_block();
		}
	}


tcp_ack() {
	tm_clear(tmack);
	tk_wake(TCPsend);
	}
		
/* Just shift the data in a buffer, moving len bytes from from to to. */

shift(from, to, len)
	register char *to;		/* destination */
	register char *from;		/* source */
	int len; {			/* length in bytes */

	if(len < 0) {
		printf("tcp: shift: bad arg--len < 0");
		return;
		}

	while(len--)
		*to++ = *from++;
	}



/* Swap the bytes in a tcp packet. */

tcp_swab(pkt)
	struct	tcp *pkt; {	/* ptr to tcp packet to be swapped */

	pkt->tc_srcp = swab(pkt->tc_srcp);
	pkt->tc_dstp = swab(pkt->tc_dstp);
	pkt->tc_seq  = lswap(pkt->tc_seq);
	pkt->tc_ack  = lswap(pkt->tc_ack);
	pkt->tc_win  = swab(pkt->tc_win);
	pkt->tc_urg  = swab(pkt->tc_urg);
	}



/* This routine forms the main body of the TCP data sending task.  This task
 * is awakened for one of two reasons: someone has data to send, or a resend
 * timeout has occurred and a retransmission is called for.  
 * This routine in either case finishes filling in the header of the
 * output packet, and calls in_write() to send it to the net.
 */

tcp_send() {
	int	sndlen;		 /* bytes to send */

	for(;;) {
	tk_block();			/* wait to be awakened */

	if(conn_state == CLOSED)	/* if no connection open, punt */
		continue;

	/* If this is not a timeout, clear the resend timer */

	if(newsend) { 			/* if there is new data to send */
		tm_clear(tcptm);
		tm_tset(retry_time, tmhnd, 0, tcptm); /* set resend timer */
		}

	if(resend) {			/* if the resend timer has gone off */
		tcpresend++;
		if(resend >= 8) {
			if(NOT_YET(conn_state, ESTAB))
				(*tc_tfcn)();
			else {		/* put timeout code here */
				}
			}
		else if(NOT_YET(conn_state, ESTAB))
				(*tc_rfcn)();

		tm_clear(tcptm);
		tm_tset(retry_time, tmhnd, 0, tcptm); /* set resend timer */
		}

	/* make sure to avoid silly window syndrome */
	if(!still_data && (otp->tc_win < tcp_low_window)) {
		otp->tc_win = tcp_window;
		}

	tcp_swab(otp);			/* swap the bytes */

	/* Calculate the number of bytes to send, keeping in mind the
		foreign host's window. If the connection isn't yet open,
		send no data at all. */

	sndlen = NOT_YET(conn_state, ESTAB) ? 0 : odlen;

	/* Finish filling in the TCP header */
	ophp->tp_len = swab(sndlen + sizeof(struct tcp));
	otp->tc_cksum = cksum(ophp, sizeof(struct tcpph) >> 1, 0);
	otp->tc_cksum = ~cksum(otp, (sndlen + sizeof(struct tcp) + 1) >> 1,0);

	/* Send it */
	in_write(tcpfd, opbi, sndlen + sizeof(struct tcp), tcp_fhost);
	ack_time = cticks + ACKDALLY;

	tcp_swab(otp);			/* swap it back so we can use it */
	tcppsnt ++;
	tcpbsnt = (unsigned)otp->tc_seq + sndlen;
	tcprack = tcpbrcv;

	
	if(otp->tc_rst) {		/* were we resetting? */
		cleanup("aborted\n");
		(*tc_cfcn)();		/* call the user close routine */
		}

	if(conn_state ==  TIMEWAIT)  	/* if we sent the last ack */
		(*tc_cfcn)();
	}
	}


/* Stuff a character into the send buffer for transmission, but do not
 * wake up the TCP sending task.  This assumes that more data will
 * immediately follow.
 */

tc_put(c)
	char c;	 {		/* character to send */

	if(!blk_inpt) {
		odp[odlen] = c;
		odlen++;
		odp[odlen] = 0;
		if(odlen >= MAXBUF) return 1;
		else return 0;
		}
	else return 1;
	}

/* Stuff a character into the send buffer for transmission, and wake
 * up the TCP sender task to send it.
 */

tc_fput(c)
	char c;	 {		/* character to send */

	if(!blk_inpt) {
		odp[odlen] = c;
		odlen++;
		odp[odlen] = 0;
		if(odlen > MAXBUF) return 1;
		}
	else return 1;

	newsend = TRUE;
	tk_wake(TCPsend);
	return 0;
	}

/* Indicate the presence of urgent data.  Just sets the urgent pointer to
 * the current data length and wakes up the sender.
 */

tcpurgent() {
	otp->tc_urg = odlen;
	otp->tc_furg = 1;
	tk_wake(TCPsend);
	}

/* Initiate the TCP closing sequence.  This routine will return immediately;
 * when the close is complete the user close function will be called.
 */

tcp_close() {
	switch(conn_state) {
	case CLOSED:
		(*tc_cfcn)();
		return;
	case ESTAB:
		conn_state = FINSENT;
	case SYNSENT:
		otp->tc_fin = 1;
		blk_inpt = 1;
		newsend = TRUE;
		tk_wake(TCPsend);
		}
	}

/*  Initiate close/reset.  This routine just sets flags and wakes
 *  the TCP send task, which does the actual dirty work. 
 */

tcp_reset() {
	otp->tc_rst = 1;	/*  Notify send that we want a reset.*/
	newsend = TRUE;		/*  Restart timer sequences.  */
	avail = 0;		/*  Reset processing of arriving data.*/
	blk_inpt = 1;		/*  Don't accept more departing data. */
	tk_wake(TCPsend);	/*  Caller should yield when ready for
				    the reset to go out.  */
	}

/* Actively open a tcp connection to foreign host fh on foreign socket
 * fs.  Get a unique local socket to open the connection on.  Returns
 * FALSE if unable to open an internet connection with the specified
 * hosts and sockets, or TRUE otherwise.
 * Note that this routine does not wait until the connection is
 * actually opened before returning.  Instead, the user open function
 * specified as ofcn in the call to tcp_init() (which must precede
 * this call) will be called when the connection is successfully opened.
 * This routine also sets a timer to call the user timeout routine
 * on ICP timeout.
 */

tcp_open(fh, fs, ls, win, lowwin)
	in_name	*fh;		/* foreign host address */
	unsigned fs;		/* foreign socket */
	unsigned ls;		/* local socket */
	int	win;		/* window to advertise */
	int	lowwin;	 {	/* low water mark for window */
	int	i;
	
	tcp_fhost = *fh;
	tcp_dst_port = fs;

	if((win<1) || (win>TCPMAXWIND) || (lowwin<1)||(lowwin>TCPMAXWIND)){
		tcp_window = TCPWINDOW;
		tcp_low_window = TCPLOWIND;
		}
	else {
		tcp_window = win;
		tcp_low_window = lowwin;
		}

	if((tcp_src_port = ls) == 0) {
		tcp_src_port = cticks;
		if(tcp_src_port < 1024) tcp_src_port += 1024;
		} 
	
	for(i = 0; i < TOTALCHUNKS; i++) free[i] = TRUE;
	blk_inpt = 0;
	conn_state = SYNSENT;		/* syn-sent */
	odlen = 0;			/* no output data yet */

	if((tcpfd = in_open(TCPPROT, tcp_rcv)) == 0) {
#ifdef	DEBUG
		printf("TCP_OPEN: can't open ip con\n");
#endif
		(*tc_cfcn)();
		return;
		}
			
        /* alloc and set up output pkt */
	if((opbi = in_alloc(INETLEN, 0)) == NULL) {
#ifdef	DEBUG
		printf("TCP_OPEN: can't alloc pkt\n");
#endif
		(*tc_tfcn)();
		return;
		}

	otp = (struct tcp *)in_data(in_head(opbi));
	odp = (char *)otp + sizeof(struct tcp);

	
	/* allocate and set up psuedoheader for outgoing packet */
	ophp = (struct tcpph *)calloc(1, sizeof(struct tcpph));
	if(ophp == NULL) {
		printf("tcp_open: can't alloc output pseudohdr\n");
		return;
		}

	ophp->tp_src = in_mymach(tcp_fhost);
	ophp->tp_dst = *fh;
	ophp->tp_zero = 0;
	ophp->tp_pro = TCPPROT;

	/* allocate and set up psudoheader for incoming packets */
	iphp = (struct tcpph *)calloc(1, sizeof(struct tcpph));
	if(iphp == NULL) {
		printf("tcp_open: can't allocate input pseudohdr\n");
		return;
		}

	iphp->tp_zero = 0;
	iphp->tp_pro = TCPPROT;

	otp->tc_thl  = sizeof(struct tcp) >> 2;	/* fill in output tcp hdr */
	otp->tc_srcp = tcp_src_port;
	otp->tc_dstp = tcp_dst_port;
	otp->tc_seq = 0;
	otp->tc_ack = 0;
	otp->tc_uu1 = 0;
	otp->tc_fin = 0;
	otp->tc_syn = 1;
	otp->tc_rst = 0;
	otp->tc_psh = 0;
	otp->tc_fack = 0;
	otp->tc_furg = 0;
	otp->tc_uu2 = 0;
	otp->tc_win = tcp_window;	
	otp->tc_urg = 0;
	tcppsnt = 0;			/* initially, no pkts sent */
	tcpprcv = 0;			/* or received */ 
	tcpbsnt = 0;			/* no bytes sent */
	tcpbrcv = 0;			/* foreign host sent no bytes */
	tcprack =0;			/* we acked no bytes of foreign host*/
	tcpresend = 0;
	bd_chk = 0;
	ign_win = 0;
	taken = -1;
	avail = -1;
	newsend = TRUE;
	tk_wake(TCPsend);
	}

cleanup(why)
	char *why; {

	printf("Closed: %s\n", why);
	}

/*  Supply TCP data for line 25 display  */

tcp_state(s)
	char *s; {sprintf(s,"Sent: %u/%u/%u Rcvd: %u/%u/%u Wind: %u      ",
			tcpbsnt, tcpbsnt-(unsigned)otp->tc_seq, tcpresend,
			tcpbrcv, tcpbrcv-tcprack, tcprercv,
			otp->tc_win);}

/* Display some tcp statistics and a few lines of unacked data. Should be
	revised and integrated in with the normal logging system. */


tc_status() {
	int	i;	 /* loop index */
	int crseen = 0;
	
	printf("Connection State: ");

	switch(conn_state) {
	case CLOSED: 	printf("Closed\n");
			break;
	case SYNSENT:	printf("Trying to Open\n");
			break;
	case SYNRCVD:	printf("SYNRCVD\n");
			break;
	case ESTAB:	printf("Established\n");
			break;
	default:	printf("(%d) Closing\n",conn_state);
	}

	printf("Packets Sent: %5u\tPackets Received: %u\n",
						tcppsnt, tcpprcv);
	printf("Bytes Sent: %5u\tAcked: %u\n",
					tcpbsnt, (unsigned)otp->tc_seq);
	printf("Bytes Received: %5u\tAcked: %u\n", tcpbrcv, tcprack);
	printf("Bad TCP xsums: %5u\tWindow ignored: %u\n",bd_chk,ign_win);
	printf("Packets not for me: %5u\n", tcpsock);
	printf("Resends: %u\tRereceived: %u\n", tcpresend, tcprercv);
	printf("Local Win: %5u\tLocal Low Win: %5d\tForeign Win: %8u\n",
						otp->tc_win,tcp_low_window,frn_win);

	printf("Ack #: %08X, Seq #: %08X\n", otp->tc_ack, otp->tc_seq);
	printf("Output Flags: ");
	if(otp->tc_syn)  printf(" SYN");
	if(otp->tc_fack) printf(" ACK");
	if(otp->tc_psh)  printf(" PSH");
	if(otp->tc_furg) printf(" URG");
	if(otp->tc_fin)  printf(" FIN");
	if(otp->tc_rst)  printf(" RST");
	if(odlen) printf("\noutput data:\n");
	else {
		putchar('\n');
		return;
		}

	i=0;
	while(1) {
		if(odp[i] == '\n') crseen++;
		putchar(odp[i]);
		if(crseen > 3 || ++i > 100 || i > odlen) break;
		}

	printf("\n");
	if(i <= odlen) printf("***MORE DATA***\n\n");

	}


/* expedite (resend and push) a packet */
tcp_ex() {
	tk_wake(TCPsend);
	otp->tc_psh = 1;
	return;
	}

/* Handle a timer upcall. */
tmhnd() {
	resend++;
	tk_wake(TCPsend);
	}

tc_q() {
	}

/* Close and reset a tcp connection. */

tc_clrs(p, fhost)
	PACKET p;
	in_name fhost; {
	long ltemp;
	unsigned myport;
	register struct tcp *ptp;
	struct tcpph php;

	ptp = (struct tcp *)in_data(in_head(p));

	/* swap port numbers */
	myport = ptp->tc_dstp;
	ptp->tc_dstp = ptp->tc_srcp;
	ptp->tc_srcp = myport;

	/* fill in the rest of the header */
	ltemp = ptp->tc_seq;
	ptp->tc_seq = ptp->tc_ack;
	ptp->tc_ack = ltemp;
	ptp->tc_thl = sizeof(struct tcp) >> 2;
	ptp->tc_uu1 = 0;
	ptp->tc_fin = 0;
	ptp->tc_syn = 0;
	ptp->tc_rst = 1;	/* The RESET bit */
	ptp->tc_psh = 0;
	ptp->tc_fack = 0;
	ptp->tc_furg = 0;
	ptp->tc_uu2 = 0;
	ptp->tc_win = 0;
	tcp_swab(ptp);

	/* Set up the tcp pseudo header */
	php.tp_src = in_mymach(fhost);
	php.tp_dst = fhost;
	php.tp_zero = 0;
	php.tp_pro = TCPPROT;
	php.tp_len = swab(sizeof(struct tcp));

	/* checksum the packet */
	ptp->tc_cksum = cksum(&php, sizeof(struct tcpph)>>1, 0);
	ptp->tc_cksum = ~cksum(ptp, sizeof(struct tcp)>>1, 0);

	in_write(tcpfd, p, sizeof(struct tcp), fhost);
	}


#define	GETTIME	0x2c

long get_dosl();

tcp_sock() {
	long temp;

	temp = get_dosl(GETTIME);
	temp &= 0xffff;
	if(temp < 1000) temp += 1000;
	return (unsigned)temp;
	}
