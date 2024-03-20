/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* Low Level Protocol (LLP) header. Gives definitions for the various
	interesting things that have to do with the llp, such as the
	special characters and escape sequences. Note that the llp has
	been modified somewhat to include another sequence which will
	cause the C-Gateway to respong with a four byte packet which
	contains the IBM's internet address. */

#define ESC	(char)242		/* Escape character */
#define REQ	(char)243		/* Request to send */
#define ACK	(char)244		/* Acknowledgement */
#define	END	(char)245		/* End of packet */

/* The timeout period for acks from the PC gateway. */
#define SLTIMEOUT	9	/*  (18ths of a second.)  */
#define	PTIMEOUT	130	/* long enough for ~600 bytes at 1200 baud */
#define	SLRETRIES	10

/* Each protocol has an internal number assigned to it. They are:
	Internet	0
	Chaos		1
	PUP		2
	SLP		3	(Serial Line Protocol)

 An entry in this list does not necessarily mean that a protocol layer for that
protocol exists or will ever exist. The following array gives conversions
between the internal rep and the actual local net header version. */

#define	SLPROTS	{ 2,1,0,0,	0,0,0,0,	0,0,0,0,	2,3,0,0}
