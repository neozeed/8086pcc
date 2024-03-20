/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* Define some ICMP messages */
#define	PGNOSND		0	/* Couldn't send pkt */
#define	PGTMO		1	/* timedout */
#define	PGBADDATA	2	/* rcved bad data back */
#define	PGWAITING	3	/* waiting for rcpt of packet */
#define	PGSUCCESS	4	/* success */

#define	DSTNET		0
#define	DSTHOST		1
#define	DSTPROT		2
#define	DSTPORT		3
#define	DSTFRAG		4
#define	DSTSRC		5

