/*  Copyright 1983 by the Massachusetts Institute of Technology  */
/*  define's for levels of tracing and for success and failure
    added, January 1984.     <J. H. Saltzer>
	8/12/84 - added the custom structure for each net, and RARP
					<John Romkey>
	10/3/84 - changed the Q to a queue * to make structure
		initialization simpler.	<John Romkey>
*/

/* need to define this before dragging in custom.h */
typedef long in_name;

#include <task.h>
#include <netq.h>
#include <custom.h>

/* This is the net structure. This idea is based somewhat on Noel's C
	Gateway. For the IBM's, we need an easy way to make up a net
	program that can use any of the three potential nets that the
	IBM can be interfaced to: the Serial Line net w/PC Gateway, the
	10 Mb Ethernet or a Packet Radio setup.
	   The code for each net has to supply a routine to send packets
	over the net, a routine to receive packets and a routine to
	do net initialization. The init routine finds the IBM's address
	on that net. All addresses are internet addresses; any local
	addresses can be kept in statics by the net drivers.
	   To use this, the user creates a C program which just consists
	of the global structure called nets which is an array of net structs.
	The netinit routine will do the right thing with all the info provided
	when called, and will call each net's initialization routines.
	The user must include a variable, int Nnet, in this file with the
	statement:
		int Nnet = sizeof(nets)/sizeof(struct net);
	This provides a way for netinit to tell how many nets there are. */


#define	IP	0
#define	CHAOS	1
#define	PUP	2
#define	SLP	3
#define	ADR	4	/* Dave Plummer's Address Resolution Protocol */
#define	RARP	5	/* reverse address resolution protocol */

/* The NET struct has all the actual interface characteristics which are
	visible at the internet level and has pointers to the interface
	handling routines.	*/

struct net {
	char	*n_name;	/* 00 the net's name in ascii */
	int	(*n_init)();	/* 02 the net initialization routine */
	int	(*n_send)();	/* 04 the packet xmit routine */
	int	(*n_open)();	/* 06 the protocol open routine */
	int	(*n_close)();	/* 08 the protocol close routine */
	task	*n_demux;	/* 22 packet demultiplexing task to protocol */
	queue	*n_inputq;	/* 24 the queue of received packets */
	unsigned n_initp1;	/* 30 initialization parameter one */
	unsigned n_initp2;	/* 32	"	     "	    two */
	int	n_stksiz;	/* 34 net task initial stack size */
	int	n_lnh;		/* 36 the net's local net header  size */
	int	n_lnt;		/* 38 the net's local net trailer size */
	in_name	ip_addr;	/* 3A the interface's internet address */
	int	(*n_stats)();	/* ... per net statistics */
	in_name n_defgw;	/* the default gateway for this net */
	struct custom	*n_custom;	/* net's custom structure */
	};


typedef struct net NET;

/* Here are the debugging things. DEBUG is a global variable whose value
	determines what sort of debugging messages will be displayed by
	the system. */

extern unsigned NDEBUG;		/* Debugging options */
extern int MaxLnh;		/* Largest local net header size */
extern unsigned version;	/* program version number */

#define	BUGHALT		1	/* BUGHALT on a gross applications level error
					that is detected in the network code */

#define	DUMP		2	/* Do a dump of all bad incoming packets */

#define	INFOMSG		4	/* Print informational messages such as packet
					received, etc. */

#define	NETERR		8	/* Display net interface error messages */

#define	PROTERR		16	/* Display protocol level error messages */

#define	TRACE		32	/* Trace packet through protocol layers */

#define NETRACE		32	/* Trace packet in link level net layer */

#define INTRACE		32	/* Trace packet in internet layer  */

#define TCTRACE		32	/* Transmission control (UDP, TCP) trace */

#define APTRACE		128	/* Trace packet through application */

#define	TMO		64	/* Print message on timeout */

#define	ROUTE		128	/* turn on routing tracing */

#define	TCPTRACE	256

/*  The following two definitions provide a standard way for one net level
to tell the next that things worked out as hoped or that they didn't.  */

#define SUCCESS		1

#define FAILURE		-1
