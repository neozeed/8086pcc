/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* Some useful things for Internet routines */
#include <stdio.h>
#include <types.h>
#include <netbuf.h>
#include <icmp.h>

/* IP header and internet names */
/*typedef long in_name;	*/

union _ipname {
	long in_lname;
	struct {
		byte in_net;
		byte in_nets;
		byte in_netss;
		byte in_host; } in_lst; };

/* Internet status. Keeps track of packets gone through and errors. */

struct ipstat {
	unsigned ip_rcv;	/* # ip packets received on interface */
	unsigned ip_snd;	/* # ip packets xmitted  on interface */
	unsigned ip_drop;	/* # ip packets dropped  on interface */
	unsigned ip_ver;	/* because of:  bad version */
	unsigned ip_len;	/*		bad length  */
	unsigned ip_dest;	/*		bad destination	*/
	unsigned ip_xsum;	/*		bad checksum */
	};

struct ip {
	int ip_ihl : 4;		/* Internet header length in 32 bit words */
	int ip_ver : 4;		/* Header version */
	byte ip_tsrv;		/* Type of service */
	unshort ip_len;		/* Total packet length including header */
	unshort ip_id;		/* ID for fragmentation */
	unshort ip_foff : 13;	/* Fragment offset */
	unshort ip_flgs : 3;	/* flags */
	byte ip_time;		/* Time to live (secs) */
	byte ip_prot;		/* protocol */
	unshort ip_chksum;	/* Header checksum */
	in_name ip_src;		/* Source name */
	in_name ip_dest; };	/* Destination name */

/* Internet connection information */

struct ip_iob {
	unshort	c_prot;		/* protocol */
	int	(*c_handle)();	/* upcalling packet handler */
	NET	*c_net;		/* net driver */
	};

typedef struct ip_iob *IPCONN;	/* The IP connection type */

/* Some useful definitions */
#define INETLEN	576		/* maximum size of internet packet (bytes) */
#define IPHSIZ	sizeof(struct ip) /* internet header size */
#define IP_VER	4		/* internet version */
#define IP_IHL	5		/* IN header length in longwords */
#define IP_TSRV	0		/* default type of service */
#define IP_ID	0		/* kernel fills in IN id */
#define IP_FLGS	0		/* no fragmentation yet */
#define	IP_FOFF	0		/* " " " */
#define	IP_TIME	255		/* maximum time to live */
#define IPXSUM	0		/* initial checksum */
#define	IPLEN	sizeof(struct ip)	/* internet header length */

/* Some goodly macros */
#define in_head(ppkt)	((struct ip *)(ppkt)->nb_prot)
#define in_data(pip)	((char *)(pip) + ((pip)->ip_ihl << 2))
#define in_options(pip)	((char *)(pip) + (IP_IHL << 2))
#define in_optlen(pip)	(((pip)->ip_ihl - IP_IHL) << 2)

#define min(x,y)	((x) < (y) ? (x) : (y))
#define max(x,y)	((x) > (y) ? (x) : (y))

/* some externals for internet et al */
extern	PACKET	in_alloc();	/* IN packet buffer allocator */
extern	PACKET	udp_alloc();	/* UDP packet allocator */
extern  IPCONN  in_open();	/* internet connectio open */
extern	in_name	resolve_name();	/* internet name/address handler */

extern	NET	*inroute();	/* Do internet/local net routing */
extern	in_name	in_mymach();	/* my internet address */

extern	long	lswap();	/* byteswap long words */
