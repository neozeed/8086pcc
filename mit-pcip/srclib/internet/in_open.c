/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>
#include "ipconn.h"

/* Open a protocol connection on top of internet. Protocol information
	necessary for packet demultiplexing; handler is upcalled
	upon receipt of packet. handler is
		int handler(p, len, fhost)
			PACKET p
			int len
			in_name fhost
*/

IPCONN ipconn[10];		/* demux table */
int nipconns = 0;			/* current posn in demux table */

IPCONN in_open(prot, handler)
	unshort prot;
	int (*handler)(); {
	int i;
	IPCONN conn;

	for(i=0; i<nipconns; i++)
		if(ipconn[i]->c_prot == prot) return 0;

	conn = (IPCONN)calloc(1, sizeof(struct ip_iob));
	if(conn == 0) return 0;
	conn->c_prot = prot;
	conn->c_handle = handler;
	conn->c_net = 0;
	ipconn[nipconns++] = conn;
	return conn;
	}
