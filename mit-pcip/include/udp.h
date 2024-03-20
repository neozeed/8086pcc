/*  Copyright 1983 by the Massachusetts Institute of Technology  */

#include <ip.h>

/* UDP Header structure */

struct udp {
	unsigned ud_srcp;	/* source port */
	unsigned ud_dstp;	/* dest port */
	unsigned ud_len;		/* length of UDP packet */
	unsigned ud_cksum;	/* UDP checksum */
	};

/* The UDP Connection structure */
struct udp_conn {
	struct udp_conn *u_next;
	unsigned	u_lport;	/* local port */
	unsigned	u_fport;	/* foreign port */
	in_name		u_fhost;	/* foreign host */
	int		(*u_rcv)();	/* incoming packet handler */
	unsigned	u_data;		/* fooish thing */
	};

typedef	struct udp_conn *UDPCONN;

/* Some goodly constants, macros and an external */
#define	UDPPROT	17	/* UDP Internet protocol number */
#define	UDPLEN	sizeof(struct udp)

#define udp_head(pip)	((struct udp *)in_data((pip)))
#define udp_data(pup)	((char *)(pup) + sizeof(struct udp))

#define udp_free(pkt)	in_free((pkt))

/* Name user return codes */
#define	NAMETMO		1L	/* Name user timed out on all requests */
#define	NAMEUNKNOWN	0L	/* Name not known */

extern UDPCONN	udp_open();
extern unsigned udp_socket();
extern unsigned udpxsum();
extern PACKET udp_alloc();

extern UDPCONN firstudp;
