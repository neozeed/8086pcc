/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>
#include "internal.h"

/* Dump the internal table of UDP connections. */

udp_table() {
	register UDPCONN con;

	printf("\nUDP demux table:\n");
	printf("Local Port\tForn Port\tFhost\t\troutine\tcn\n");

	for(con=firstudp; con; con = con->u_next) {
		printf("%04x\t\t%04x\t %a ",
				con->u_lport, con->u_fport, con->u_fhost);
		printf("\t\t%04x\t%04x\n",
				con->u_rcv, con->u_data);
		}

	printf("\n");
	}
