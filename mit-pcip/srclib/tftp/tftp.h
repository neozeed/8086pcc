/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 10/1/84 - added definition of dos_eof as control Z since <stdio.h>
	no longer defines it.			<John Romkey>
   11/29/84 - changed round-trip time initial estimate to 2.5 sec
        to accomodate overloaded VAX unix systems.
                                                 <J. H. Saltzer>  
*/

#include <udp.h>
#include <timer.h>
#include <tftp.h>

/* TFTP header file contains structures of TFTP packet headers and such. */

/* read or write request packet */
struct tfreq {
	unsigned	tf_op;		/* would be 1 (read) or 2 (write) */
	char		tf_name[20];	};

/* data packet */
struct tfdata {
	unsigned	tf_op;		/* would be 3 */
	unsigned	tf_block;
	char		tf_data[512];	};

/* structure of an ack packet */
struct	tfack {
	unsigned	tf_op;		/* would be 4 */
	unsigned	tf_block;	};

/* error packet */
struct tferr {
	unsigned	tf_op;		/* would be 5 */
	unsigned 	tf_code;
	char		tf_err[1]; };

/* TFTP opcodes */
#define	RRQ	1		/* read  request */
#define	WRQ	2		/* write request */
#define	DATA	3		/* data packet */
#define	ACK	4		/* acknowledgement packet */
#define	ERROR	5		/* error packet */

/* TFTP error codes */
#define	ERRTXT		0	/* see the enclosed text */
#define	FNOTFOUND	1	/* file not found */
#define	ACCESS		2	/* access violation */
#define	DISKFULL	3	/* don't even ask. */
#define	ILLTFTP		4	/* illegal TFTP operation */
#define	BADTID		5	/* unkown transfer ID */
#define	FEXISTS		6	/* file already exists */
#define	NOUSER		7	/* no such user */

/* TFTP states */
#define	DATAWAIT	1
#define	ACKWAIT		2
#define	DEAD		3
#define	TIMEOUT		4
#define	RCVERR		5
#define	RCVACK		6
#define	RCVDATA		7
#define	RCVLASTDATA	8
#define	TERMINATED	9

#define	TFTPSOCK	69	/* TFTP's well known port */
#define	TFTPTRIES	20	/* # of retries on packet transmission */
#define	REQTRIES	4	/* # of retries on initial request */
#define	REQLEN		512	/* stupid, stupid... */
#define	NORMLEN		512	/* normal length of received packet */

#define	tftp_head(p)	udp_data(udp_head(in_head((p))))
#define	tfkill(cn)	{tm_clear(cn->tf_tm); (cn)->tf_state=DEAD; tk_wake((cn)->tf_task);}
#define	tftp_data(p)	((struct tfdata *)tftp_head((p)))->tf_data

/* The TFTP connection structure. Contains connection info, and data for
	timeout calculations. */

struct tfconn {
	UDPCONN		tf_udp;	/* udp connection for this transfer */
	FILE		*tf_fd;		/* file descriptor for xfer */
	PACKET		tf_outp;	/* last sent packet */
	unsigned	tf_lastlen;	/* length of last sent pkt */
	unsigned	tf_expected;	/* most recently processed block */
	unsigned	tf_fport;	/* foreign port */
	task		*tf_task;	/* main task for tftp connection */
	timer		*tf_tm;		/* our timer */
	unsigned	tf_state;	/* state of connection */
	unsigned	tf_tries;	/* # of retries already done */
	unsigned	tf_mode;	/* mode = IMAGE, [net]ASCII, ... */
	unsigned	tf_dir;		/* direction of the transfer */
	long		tf_size;	/* # of bytes transferred */
	unsigned	tf_rcv;		/* # of packets received */
	unsigned	tf_snt;		/* # of packets sent */
	unsigned	tf_ous;		/* # of out of sequence packets */
	unsigned	tf_tmo;		/* # of timeouts */
	unsigned	tf_rsnd;	/* # of resends */
	long		tf_trt;		/* round trip time */
	long		tf_rt;		/* current timeout */
	int		tf_NR;		/* number rexmissions of this pkt */
	int		tf_NR_last;	/* "	"	" of prev pkt */
	int		tf_K;		/* tuning constant */
	long		tf_sent;	/* time that pkt was sent */
	};

/*  Constants for round trip time estimation and retry timeout.
    All calculation is done in clock ticks (at a rate of 18/second) but
    only the initial estimate and the upper limit are specified in ticks;
    the rest of the algorithm uses dimensionless multipliers.  */

#define	Kinit	3	/* Initial divisor for (1+1/K) estimate multiplier. */
#define	Kinc	1	/* Reduce K by this if previous packet lost.  */
#define	T0	45	/* Initial value for round trip time estimate.  */
#define	MAXTMO	216	/* upper limit on retry timeout timer, in ticks.  */
#define	TMMULT  3	/* multiplier to get retry timeout from round trip
			   estimate.  */

#define	dos_eof	26	/* control Z */

int tftprcv();		/* packet receive routine */
int tftptmo();		/* timeout handler */
long tfcleanup(), tftpuse(), atol();
struct tfconn *tfmkcn();
extern char *tftplog;
extern long cticks;
