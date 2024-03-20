/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <netbuf.h>
#include <stdio.h>
#include <ether.h>
#include "ether.h"

/* This code handles the address resolution protocol as described by
	David Plummer in NIC RFC-826. It's split into two parts:
	etarsnd which sends a request packet and
	etarrcv which handles an incoming packet. If the packet is a request
		and is for us, reply to it. If it is a reply, update our
		table.

	The code is currently crocked to work only with internet and ethernet.
*/

/* define the table format of internet -> ethernet address mappings */
#define	MAXENT	16

struct tabent {
	in_name	ip_addr;
	char	et_addr[6];
	int	count;	};

static struct tabent table[MAXENT];

static task *etneed;

/* Plummer's internals. All constants are already byte-swapped. */
#define	ARETH	0x100		/* ethernet hardware type */
#define	ARIP	ETIP		/* internet protocol type */
#define	ARREQ	0x100		/* byte swapped request opcode */
#define	ARREP	0x200		/* byte swapped reply opcode */

struct adr {
	unsigned	ar_hd;		/* hardware type */
	unsigned	ar_pro;		/* protcol type */
	char		ar_hln;		/* hardware addr length */
	char		ar_pln;		/* protocol header length */
	unsigned	ar_op;		/* opcode */
	char		ar_sha[6];	/* sender hardware address */
	in_name		ar_spa;		/* sender protocol address */
	char		ar_tha[6];	/* target hardware address */
	in_name		ar_tpa;		/* target protocol address */
	};

static unsigned etadsnd = 0;	/* # of packets sent */
static unsigned etadreq = 0;	/* # of requests received */
static unsigned etadrep = 0;	/* # of replies received */
static unsigned etadnotme = 0;	/* # of requests not for me */
static unsigned etadbad = 0;	/* # of bad ARP packets */
static unsigned etadlen = 0;	/* # of bad lengths */
static unsigned etunexpect = 0;	/* # of unexpected replies */

etainit() {
	int i;

	for(i=0; i<3; i++) {
		table[i].ip_addr = custom.c_ipname[i];
		etadcpy(custom.c_ether[i].e_ether, table[i].et_addr);
		}
	}

/* Send an address request packet for internet address ipaddr. This routine
	doesn't wait to receive a reply. */

etarsnd(ipaddr)
	in_name ipaddr; {
	struct adr *padr;
	PACKET p;

	etadsnd++;

	p = a_getfree();
	if(p == NULL) return 0;

	padr = (struct adr *)(p->nb_buff+sizeof(struct ethhdr));

	padr->ar_hd = ARETH;
	padr->ar_pro = ARIP;
	padr->ar_hln = 6;
	padr->ar_pln = sizeof(in_name);
	padr->ar_op  = ARREQ;
	etadcpy(_etme, padr->ar_sha);
	padr->ar_spa = et_net->ip_addr;
	padr->ar_tpa = ipaddr;

	if(et_send(p, ADR, sizeof(struct adr), ETBROADCAST) == 0) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|NETERR))
			printf("ETARSND: Couldn't xmit request packet.\n");
#endif
		}

	putfree(p);
	return 0;
	}

/* Handle an incoming ADR packet. If it is a request and is for us, answer it.
	Otherwise, if it is a reply, log it. If not, discard it */

etarrcv(p, len)
	register PACKET p;
	unsigned len; {
	register struct adr *padr;
	int i;

	padr = (struct adr *)p->nb_prot;
	if(len < sizeof(struct adr)) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("ETARRCV: bad pkt len %u\n", len);
#endif
		etadlen++;
		putfree(p);
		return;
		}

	switch(padr->ar_op) {
	case ARREQ:
		etadreq++;
		if(padr->ar_hd != ARETH) {
			etadnotme++;
			putfree(p);
			return;
			}

		if(padr->ar_pro != ARIP) {
			etadnotme++;
			putfree(p);
			return;
			}

		if(padr->ar_tpa != et_net->ip_addr) {
			etadnotme++;
			putfree(p);
			return;
			}

		/* Make an entry in our translation table for this host since
			he's obviously going to send me a packet. */
		for(i=0; i<MAXENT; i++) {
			/* update the table even if we already know the host
			   - makes it possible to change gateway interfaces
			*/
			if(table[i].ip_addr == padr->ar_spa) {
				etadcpy(padr->ar_sha, table[i].et_addr);
				break;
				}
			if(i == MAXENT-1) {
				table[i].ip_addr = padr->ar_spa;
				etadcpy(padr->ar_sha, table[i].et_addr);
				break;
				}
			if(table[i].ip_addr) continue;
			table[i].ip_addr = padr->ar_spa;
			etadcpy(padr->ar_sha, table[i].et_addr);
			break;
			}

		padr->ar_op = ARREP;
		etadcpy(padr->ar_sha, padr->ar_tha);
		etadcpy(_etme, padr->ar_sha);
		padr->ar_tpa = padr->ar_spa;
		padr->ar_spa = et_net->ip_addr;
		et_send(p, ADR, sizeof(struct adr), padr->ar_tha);
		putfree(p);
		break;

	case ARREP:
		if(padr->ar_tpa != et_net->ip_addr) {
			etadnotme++;
			putfree(p);
			return;
			}

		for(i=0; i<MAXENT&table[i].ip_addr; i++)
			if(table[i].ip_addr == padr->ar_spa) {
				etadcpy(padr->ar_sha, table[i].et_addr);
				putfree(p);
				if(etneed) {
					tk_wake(etneed);
					etneed=0;
					}
				return;
				}

		if(i==MAXENT)
			i=MAXENT-1;

		table[i].ip_addr = padr->ar_spa;
		etadcpy(padr->ar_sha, table[i].et_addr);
		putfree(p);

		if(etneed) {
			tk_wake(etneed);
			etneed = 0;
			}
		break;

	default:
		etadbad++;
#ifdef	DEBUG
		printf("ETARRCV: Unknown opcode %u.\n", padr->ar_op);
#endif
		putfree(p);
		}
	}

/* Convert an internet address into an ethernet address. Assumes that the
	internet address is for someone on our net; does no checking.
	First tries to look the internet address up in a table; if
	this fails, then it sends out a Plummer packet and waits for
	a reply with a timeout. Fills in the ethernet address of the
	foreign host. If the ethernet address couldn't be determined,
	returns a FALSE, otehrwise TRUE. */

extern long cticks;

ip2et(ether, ip)
	register char *ether;
	in_name ip; {
	int i;
	long time;

	if(ip == 0L) {
#ifdef	DEBUG
		printf("IP2ET: Passed null IP address!\n");
#endif
		return 0;
		}

	for(i=0; (i<MAXENT)&&(table[i].ip_addr != ip)&&(table[i].ip_addr); i++)
		;

	if(i != MAXENT && table[i].ip_addr) {
		etadcpy(table[i].et_addr, ether);
		return 1;
		}

	etarsnd(ip);
	etneed = tk_cur;

	time = cticks;
	while(cticks - time < 3*18) {
		tk_yield();
		if(!etneed) {
			for(i=0; i<MAXENT; i++) {
				if(table[i].ip_addr == ip) {
					etadcpy(table[i].et_addr, ether);
					return 1;
					}
				}
			}
		}

#ifdef	DEBUG
	if(NDEBUG & INFOMSG || NDEBUG & PROTERR)
		printf("IP2ET: Exiting without having resolved address.\n");
#endif
	etneed = 0;
	return 0;
	}


etadstat(fd)
	FILE *fd; {

	fprintf(fd, "%u ARPs sent\t%u ARP reqs rcvd\t%u ARP reps rcved\n",
						etadsnd, etadreq, etadrep);
	fprintf(fd, "%u ARP reqs not for me\t%u bad ARPs\n",
						etadnotme, etadbad);
	fprintf(fd, "%u unexpected replies\t%u bad lengths\n",
							etunexpect, etadlen);
	}
