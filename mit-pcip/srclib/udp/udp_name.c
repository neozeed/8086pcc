/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>
#include <timer.h>
#include "name.h"

#define	TIMEOUT	9

/* host name resolution code for use with pre-domain name name servers.
	Gets name server addresses from custom structure.

    4/7/84 - changed the strcpy into wanted_name to a simple assignment.
					<John Romkey>
    4/26/84 - changed name_rcv() to not wake up on last received response
		if the address has already been resolved.
					<John Romkey>
*/

static UDPCONN	udps[5];
static in_name  address;
static task	*name_task;
static unsigned nresp = 0;	/* # of responses */
static char *wanted_name;
static int name_rcv(), name_wake();

extern NET nets[];

#define	COMPILER_MAGIC	2

in_name udpname(name)
	register char *name; {
	timer *tm;
	PACKET p;
	register struct nmitem *pnm;
	int len;
	int i;

	/* grovel, grovel...check if the name is "me". If it is, special case
		it and use my net 0 ip address. A crock. */
	if(strcmp(name, "me") == 0) return nets[0].ip_addr;

	len = strlen(name) + sizeof(struct nmitem) - COMPILER_MAGIC;

	p = udp_alloc(len+1, 0);
	pnm = (struct nmitem *)udp_data(udp_head(in_head(p)));

	pnm->nm_type = NI_NAME;
	pnm->nm_len = len - 2;

	strcpy(pnm->nm_item, name);
	wanted_name = name;

	name_task = tk_cur;
	address = 0L;
	nresp = 0;

	for(i=0; i<custom.c_numname; i++) {
		if(i == 5) {
#ifdef	DEBUG
			if(NDEBUG & (PROTERR|NETERR))
				printf("UDP_NAME: Too many name servers!!\n");
			if(NDEBUG & BUGHALT) {
				printf("BUGHALT!\n");
				exit(1);
				}
#endif

			break;
			}

#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("udpname: req to %a\n", custom.c_names[i]);
#endif

		udp_write(udps[i], p, len);
		}

	udp_free(p);

	tm = tm_alloc();
	tm_set(TIMEOUT, name_wake, 0, tm);
	tk_block();

	/* Now one of two things should have happened: we should have received
		the resolved address or we should have timed out. If we've
		gotten the address, we should clear the timer. If we've timed
		out, we should just deallocate it. */

	tm_clear(tm);
	tm_free(tm);

	if(nresp == 0) return NAMETMO;
	else return address;
	}


static name_rcv(p, len, host)
	PACKET p;
	unsigned len;
	in_name host; {
	register struct nmitem *pnm;

	nresp++;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("NAME_RCV: response from %a\n", host);
#endif

	pnm = (struct nmitem *)udp_data(udp_head(in_head(p)));

/* if confirming message isnt the same as the name we sent out, then the
   recieved packet isnt really for us so return and wait for another packet */
	
	if(strncmp(pnm->nm_item, wanted_name, pnm->nm_len) != 0 ) {
		udp_free(p);
		return;
		}
		
	pnm = (struct nmitem *)((char *)pnm + pnm->nm_len + 2);

	if((pnm->nm_type == NI_ADDR) && (address == 0L)) {
		name_wake();
		address = *((in_name *)pnm->nm_item);
		}

	else {
#ifdef	DEBUG
	  if(NDEBUG & INFOMSG)
		printf("NAME_RCV: name srvr %a couldn't rslve name\n", host);
#endif
		if(nresp == custom.c_numname && !address)
			name_wake();
		}

	udp_free(p);
	}


static name_wake() {

#ifdef	DBEUG
	if(NDEBUG & INFOMSG)
		printf("Waking name user task.\n");
#endif

	tk_wake(name_task);
	}

/* new function to open 5 udp connections and hang on to them... */

nm_init()

{
	int i=0;
	for(i=0; i<custom.c_numname; i++) {
		if(i == 5) {
#ifdef	DEBUG
			if(NDEBUG & (PROTERR|NETERR))
				printf("UDP_NAME: Too many name servers!!\n");
			if(NDEBUG & BUGHALT) {
				printf("BUGHALT!\n");
				exit(1);
				}
#endif

			break;
			}

		udps[i] = udp_open(custom.c_names[i], NAMESOCK,
						udp_socket(), name_rcv);
		}
	}
