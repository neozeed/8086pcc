/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 7/10/84 - moved some variables definitions into et_int.c.
					<John Romkey>
   7/16/84 - changed debugging level on short packet message to only
	INFOMSG.			<John Romkey>
   10/11/84 - moved display code from et_demux.t.c to display.c.
					<John Romkey>
*/

#include <ip.h>
#include <ether.h>

#include <match.h>
#include <attrib.h>
#include "modes.h"

/* Process an incoming ethernet packet. Upcall the appropriate protocol
	(Internet, Chaos, PUP, NS, Plummers, ...). This does not check on
	my address and does not support multicast. It may in the future
	attempt to do more of the right thing with broadcast. */

#define	mkbyte(x)	((x)&0377)

/* structure used for describing protocols & numbers */
struct nameber {
	unsigned n_number;
	char *n_name;
	int (*n_parse)();
	};

/* layer parsing routines */
int prs_ip(), prs_arp(), prs_chaos();
int prs_udp(), prs_tcp(), prs_icmp(), prs_rvd();
int prs_rfc();

struct nameber ether_prots[] = {
	{ 0x200, "PUP", 0},
	{ 0x600, "XNS", 0},
	{ 0x800, "IP", prs_ip},
	{ 0x804, "Chaos", prs_chaos},
	{ 0x806, "ARP", prs_arp},
	{ 0, 0, 0}
};

struct nameber ip_prots[] = {
	{ 1, "ICMP", prs_icmp},
	{ 3, "GGP", 0},
	{ 6, "TCP", prs_tcp},
	{ 8, "EGP", 0},
	{ 9, "IGP", 0},
	{ 17, "UDP", prs_udp},
	{ 66, "RVD", prs_rvd},
	{ 0, 0, 0}
};

struct nameber udp_prots[] = {
	{ 7, "echo", 0},
	{ 9, "discard", 0},
	{ 19, "chargen", 0},
	{ 22, "netlog", 0},
	{ 37, "time", 0},
	{ 42, "name", 0},
	{ 53, "domain", 0},
	{ 69, "tftp", 0},
	{ 300, "nms", 0},
	{ 513, "rwho", 0},
	{ 517, "talk", 0},
	{ 520, "route", 0},
	{ 0, 0, 0}
};
struct nameber tcp_prots[] = {
	{ 7, "echo", 0},
	{ 9, "discard", 0},
	{ 11, "systat", 0},
	{ 13, "daytime", 0},
	{ 15, "netstat", 0},
	{ 17, "quotd", 0},
	{ 19, "chargen", 0},
	{ 21, "ftp", 0},
	{ 23, "telnet", 0},
	{ 25, "smtp", 0},
	{ 37, "time", 0},
	{ 42, "name", 0},
	{ 43, "whois", 0},
	{ 53, "domain", 0},
	{ 57, "mtp", 0},
	{ 101, "hostnames", 0},
	{ 77, "rje", 0},
	{ 79, "finger", 0},
	{ 87, "ttylink", 0},
	{ 95, "supdup", 0},
	{ 115, "write", 0},
	{ 512, "rexec", 0},
	{ 513, "rlogin", 0},
	{ 514, "rsh", 0},
	{ 0, 0, 0}
};

struct nameber icmp_prots[] = {
	{ 0x000, "echo reply", 0},
	{ 0x300, "net unreachable", 0},
	{ 0x301, "host unreachable", 0},
	{ 0x302, "protocol unreachable", 0},
	{ 0x303, "port unreachable", 0},
	{ 0x304, "fragmentation needed", 0},
	{ 0x305, "source route failed", 0},
	{ 0x400, "source quench", 0},
	{ 0x500, "net redirect", 0},
	{ 0x501, "host redirect", 0},
	{ 0x502, "tos & net redirect", 0},
	{ 0x503, "tos & host redirect", 0},
	{ 0x800, "echo request", 0},
	{ 0xb00, "ttl exceeded", 0},
	{ 0xb01, "frag reassembly time exceeded", 0},
	{ 0xc00, "parameter problem", 0},
	{ 0xd00, "timestamp request", 0},
	{ 0xe00, "timestamp reply", 0},
	{ 0xf00, "info request", 0},
	{ 0x1000, "info reply", 0},
	{ 0, 0, 0}
};

struct nameber rvd_types[] = {
	{ 1, "spinup", 0},
	{ 2, "spindown", 0},
	{ 3, "read", 0},
	{ 4, "write-block", 0},
	{ 17, "spinup-ack", 0},
	{ 18, "error", 0},
	{ 19, "spindown-ack", 0},
	{ 20, "read-block", 0},
	{ 21, "write-ack", 0},
	{ 0, 0, 0}
};

struct nameber chaos_opcodes[] = {
	{ 1, "RFC", prs_rfc},
	{ 2, "OPN", 0},
	{ 3, "CLS", 0},
	{ 4, "FWD", 0},
	{ 5, "ANS", 0},
	{ 6, "SNS", 0},
	{ 7, "STS", 0},
	{ 010, "RUT", 0},
	{ 011, "LOS", 0},
	{ 012, "LSN", 0},
	{ 013, "MNT", 0},
	{ 014, "EOF", 0},
	{ 015, "UNC", 0},
	{ 0, 0, 0}
};

struct nameber *lookup();

extern struct pkt pkts[MAXPKT];

extern int pproc;
extern int prcv;
extern long npackets;

extern char *header;
extern unsigned etwpp, etdrop, etmulti;

pkt_display() {
	unsigned type;
	register char *data;
	int i;
	int y = 1;
	char buffer[200];

	while(1) {
	tk_yield();
	if(prcv == pproc) continue;
	
	type = bswap(pkts[pproc].p_type);

	switch(match) {
	case M_ALL:
		break;
	case M_TYPE:
		if(type == mtype) break;
		goto check;
		break;
	case M_SRC:
		for(i=0; i<6; i++)
			if(maddr[i] != pkts[pproc].p_src[i]) goto check;
		break;
	case M_DST:
		for(i=0; i<6; i++)
			if(maddr[i] != pkts[pproc].p_dst[i]) goto check;
		break;
		}

	data = pkts[pproc].p_dst;

	switch(prot_mode) {
	case MD_SYMBOLIC: {
		struct nameber *n;

		n = lookup(ether_prots, type);
		if(n) {
			sprintf(buffer, "%s: ", n->n_name);
			wr_string(buffer, y, 0, NORMAL);
			if(n->n_parse) {
				(*n->n_parse)(pkts[pproc].p_data, y, strlen(buffer));
				break;
				}
			}
		}

	case MD_NORMAL:

	sprintf(buffer, "%02x%02x%02x%02x%02x%02x ", mkbyte(data[0]),
		mkbyte(data[1]), mkbyte(data[2]), mkbyte(data[3]),
					mkbyte(data[4]), mkbyte(data[5]));
	wr_string(buffer, y, 0, NORMAL);
	data += 6;

	sprintf(buffer, "%02x%02x%02x%02x%02x%02x ", mkbyte(data[0]),
		mkbyte(data[1]), mkbyte(data[2]), mkbyte(data[3]),
					mkbyte(data[4]), mkbyte(data[5]));
	wr_string(buffer, y, 13, NORMAL);
	data += 6;

	sprintf(buffer, "  %04x %4u ", type, pkts[pproc].p_len);
	wr_string(buffer, y, 26, NORMAL);
	data += 4;

	sprintf(buffer, "  %02x  %02x  %02x  %02x  %02x  %02x  %02x  %02x", 
		mkbyte(data[0]), mkbyte(data[1]), mkbyte(data[2]),
		mkbyte(data[3]), mkbyte(data[4]), mkbyte(data[5]),
				 mkbyte(data[6]), mkbyte(data[7]));
	wr_string(buffer, y, 38, NORMAL);
	break;
	}

	if(y++ == 23) y = 1;

	clear_lines(y, 1);

	npackets++;

	sprintf(buffer, "%8U packets", npackets);
	wr_string(buffer, 24, 50, INVERT);

check:
	pproc = (pproc+1)&PKTMASK;
	}
}

/* lookup a protocol name */
struct nameber *lookup(table, num)
	register struct nameber *table;
	unsigned num; {

	while(table->n_name)
		if(table->n_number == num) return table;
		else table++;

	return 0;
	}

struct udp {
	unsigned ud_srcp;	/* source port */
	unsigned ud_dstp;	/* dest port */
	unsigned ud_len;		/* length of UDP packet */
	unsigned ud_cksum;	/* UDP checksum */
	};

prs_udp(pup, y, x)
	register struct udp *pup;
	int x, y; {
	char buffer[40];
	struct nameber *src, *dst;

	src = lookup(udp_prots, bswap(pup->ud_srcp));
	dst = lookup(udp_prots, bswap(pup->ud_dstp));

	if(src && dst)
		sprintf(buffer, " %6s -> %6s  %4u", src->n_name, dst->n_name,
							bswap(pup->ud_len));
	else if(src && !dst)
		sprintf(buffer, " %6s -> %6u  %4u", src->n_name,
				bswap(pup->ud_dstp), bswap(pup->ud_len));
	else if(!src && dst)
		sprintf(buffer, " %6u -> %6s  %4u", bswap(pup->ud_srcp),
						dst->n_name, bswap(pup->ud_len));
	else
		sprintf(buffer, " %6u -> %6u  %4u", bswap(pup->ud_srcp),
				bswap(pup->ud_dstp), bswap(pup->ud_len));

	wr_string(buffer, y, x, NORMAL);
	return x+22;
	}


struct	tcp	{			/* a tcp header */
	unshort	tc_srcp;		/* source port */
	unshort	tc_dstp;		/* dest port */
	long	tc_seq;			/* sequence number */
	long	tc_ack;			/* acknowledgement number */
	int	tc_uu1 : 4;		/* unused */
	int	tc_thl : 4;		/* tcp header length */
	int	tc_fin : 1;		/* fin bit */
	int	tc_syn : 1;		/* syn bit */
	int	tc_rst : 1;		/* reset bit */
	int	tc_psh : 1;		/* push bit */
	int	tc_fack : 1;		/* ack valid */
	int	tc_furg : 1;		/* urgent ptr. valid */
	int	tc_uu2 : 2;		/* unused */
	unshort	tc_win;			/* window */
	unshort	tc_cksum;		/* checksum */
	unshort	tc_urg;			/* urgent pointer */
	};

prs_tcp(ptp, y, x)
	register struct tcp *ptp;
	int x, y; {
	char buffer[40];
	struct nameber *src, *dst;

	src = lookup(tcp_prots, bswap(ptp->tc_srcp));
	dst = lookup(tcp_prots, bswap(ptp->tc_dstp));

	if(src && dst)
		sprintf(buffer, " %s -> %s ", src->n_name, dst->n_name);

	else if(src && !dst)
		sprintf(buffer, " %s -> %6u ", src->n_name,
						bswap(ptp->tc_dstp));
	else if(!src && dst)
		sprintf(buffer, " %u -> %6s ", bswap(ptp->tc_srcp),
								dst->n_name);
	else
		sprintf(buffer, " %6u -> %6u ", bswap(ptp->tc_srcp),
							bswap(ptp->tc_dstp));

	wr_string(buffer, y, x, NORMAL);
	x += strlen(buffer);
	sprintf(buffer, "a:%08X s:%08X w:%u", lswap(ptp->tc_ack),
		lswap(ptp->tc_seq), bswap(ptp->tc_win));
	wr_string(buffer, y, x, NORMAL);
	x += strlen(buffer);
	return x;
	}

struct icmp {
	unsigned i_type;	/* type & code fields */
	};

prs_icmp(pip, y, x)
	register struct icmp *pip;
	int x, y; {
	char buffer[40];
	struct nameber *type;

	type = lookup(icmp_prots, bswap(pip->i_type));

	if(type)
		sprintf(buffer, " %s", type->n_name);
	else
		sprintf(buffer, " %6u", bswap(pip->i_type));

	wr_string(buffer, y, x, NORMAL);
	return x+12;
	}

struct rvd {
	char	 r_type;	/* type & code fields */
	char	 r_mode;
	};

prs_rvd(prp, y, x)
	register struct rvd *prp;
	int x, y; {
	char buffer[40];
	struct nameber *type;

	type = lookup(rvd_types, prp->r_type);

	/* lazy kluge */
#define	RVD_ERROR	18
	if(prp->r_type == RVD_ERROR)
		sprintf(buffer, "error %d\n", prp->r_mode);
	else {
	if(type)
		sprintf(buffer, " %s", type->n_name);
	else
		sprintf(buffer, " %3u", prp->r_type);
	}

	wr_string(buffer, y, x, NORMAL);
	return x+10;
	}

struct chaos {
	char	c_rsvd;
	char	c_opcode;
	int	c_flen:12;
	int	c_fcount:4;
	unsigned	c_dest;
	unsigned	c_dindx;
	unsigned	c_src;
	unsigned	c_sindx;
	unsigned	c_pnumber;
	unsigned	c_ack;
	};

prs_chaos(pcp, y, x)
	register struct chaos *pcp;
	int y;
	int x; {
	char buffer[100];
	struct nameber *type;

	sprintf(buffer, "%06o.%06o -> %06o.%06o ", pcp->c_src, pcp->c_sindx,
						pcp->c_dest, pcp->c_dindx);
	wr_string(buffer, y, x, NORMAL);
	x += 31;

	type = lookup(chaos_opcodes, pcp->c_opcode);
	if(type)
		sprintf(buffer, "%s  ", type->n_name);
	else
		sprintf(buffer, "0%o", pcp->c_opcode&0377);

	wr_string(buffer, y, x, NORMAL);
	x += strlen(buffer);
	if(type && type->n_parse)
		return (*type->n_parse)(pcp, y, x);
	return x;
	}


struct arp {
	unsigned	ar_hd;		/* hardware type */
	unsigned	ar_pro;		/* protcol type */
	char		ar_hln;		/* hardware addr length */
	char		ar_pln;		/* protocol header length */
	unsigned	ar_op;		/* opcode */
	union	{
		struct {
		char		ar_sha[6];	/* sender hardware address */
		long		ar_spa;		/* sender protocol address */
		char		ar_tha[6];	/* target hardware address */
		long		ar_tpa;		/* target protocol address */
		} arp_ip;
		struct {
		char		ar_sha[6];	/* sender hardware address */
		unsigned	ar_spa;		/* sender protocol address */
		char		ar_tha[6];	/* target hardware address */
		unsigned	ar_tpa;		/* target protocol address */
		} arp_ch;
		}	ar_addrs;
	};

prs_arp(pap, y, x)
	register struct arp *pap;
	int y;
	int x; {
	char buffer[100];
	struct nameber *type;

	if(pap->ar_op == 0x100) {
		wr_string("REQ", y, x, NORMAL);
		x += 3;
		}
	else if(pap->ar_op == 0x200) {
		wr_string("REP", y, x, NORMAL);
		x += 3;
		}
	else {
		sprintf(buffer, "???%04x", bswap(pap->ar_op));
		wr_string(buffer, y, x, NORMAL);
		x += 7;
		}

	type = lookup(ether_prots, bswap(pap->ar_pro));
	if(type) {
		sprintf(buffer, " prot: %s ", type->n_name);
		wr_string(buffer, y, x, NORMAL);
		x += strlen(buffer);
		}
	else {
		sprintf(buffer, " unknown prot %04x", bswap(pap->ar_pro));
		wr_string(buffer, y, x, NORMAL);
		x += strlen(buffer);
		}

	if(pap->ar_pro == 0x008) {	/* IP */
		sprintf(buffer, "%a -> %a", pap->ar_addrs.arp_ip.ar_spa,
						pap->ar_addrs.arp_ip.ar_tpa);
		wr_string(buffer, y, x, NORMAL);
		x += strlen(buffer);
		}
	else if(pap->ar_pro == 0x408) {	/* chaos */
		sprintf(buffer, "0%o -> 0%o", pap->ar_addrs.arp_ch.ar_spa,
						pap->ar_addrs.arp_ch.ar_tpa);
		wr_string(buffer, y, x, NORMAL);
		x += strlen(buffer);
		}
	return x;
	}

prs_ip(pip, y, x)
	register struct ip *pip;
	int y, x; {
	char buffer[100];
	struct nameber *n;

	sprintf(buffer, "%a -> %a", pip->ip_src, pip->ip_dest);
	wr_string(buffer, y, x, NORMAL);

	x += strlen(buffer);

	n = lookup(ip_prots, pip->ip_prot);
	if(n)
		sprintf(buffer, "%4d %6s ", bswap(pip->ip_len), n->n_name);
	else 
		sprintf(buffer, "%4d %6u ", bswap(pip->ip_len), pip->ip_prot);

	wr_string(buffer, y, x, NORMAL);
	x += strlen(buffer);

	if(n->n_parse) (*n->n_parse)(pip+1, y, x);
	return x;
	}

prs_rfc(pcp, y, x)
	register struct chaos *pcp;
	int y, x; {

	((char *)(pcp+1))[pcp->c_flen] = 0;
	wr_string(pcp+1, y, x, NORMAL);
	return x+strlen(pcp+1);
	}

wr_string(st, y, x, attrib)
	char *st;
	int y,x;
	unsigned attrib; {

	if(strlen(st) + x > 80)
		st[80-x] = '\0';

	write_string(st, y, x, attrib);
	}
