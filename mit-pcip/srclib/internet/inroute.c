/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 11/17/84 - changed to use subnet masks.
					<John Romkey>
*/

#include <ip.h>
#include "redirtab.h"

/* Route a packet.
	Takes the internet address that we want to send a packet to and
	tries to figure out which net interface to send it through.
	
*/

extern NET nets[];
extern int Nnet;

NET *inroute(host, hop1)
	in_name host;
	in_name *hop1; {
	int i;

	/* first check through the redirect table for this host */
	for(i=0; i<REDIRTABLEN && redtab[i].rd_dest; i++) {
		if(redtab[i].rd_dest == host) {
			*hop1 = redtab[i].rd_to;
			return &nets[0];
			}
		}

	for(i=0; i<Nnet; i++) {
		/* Check if it is on my net */
		if((nets[i].ip_addr & nets[i].n_custom->c_net_mask) ==
			      (host & nets[i].n_custom->c_net_mask)) {
			*hop1 = host;
			return &nets[i];
			}
		}

	/* The host isn't on a net I'm on, so send it to the default gateway
		on my first net   --- this is a really bad idea.
	*/

	*hop1 = nets[0].n_defgw;
	return &nets[0];

	}
