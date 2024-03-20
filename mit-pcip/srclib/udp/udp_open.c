/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>

/* Create a UDP connection and enter it in the demux table. */

UDPCONN firstudp = 0;

UDPCONN udp_open(fhost, fsock, lsock, handler, data)
	in_name fhost;			/* foreign host */
	unsigned fsock;			/* foreign socket */
	unsigned lsock;			/* local socket */
	int (*handler)();		/* upcalled on receipt of a packet */
	unsigned data; {		/* random data */
	int i;
	register UDPCONN con;
	UDPCONN ocon;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("udp_open: host %a, lsock %u, fsock %u, foo %u\n",
						fhost,lsock, fsock, data);
#endif

	for(con = firstudp; con; con = con->u_next) {
		if(con->u_lport == lsock && con->u_fport == fsock &&
				            con->u_fhost == fhost) {
#ifdef	DEBUG
			if(NDEBUG & INFOMSG || NDEBUG & PROTERR)
				printf("UDP: Connection already exists.\n");
#endif
			return 0;
			}

		ocon = con;
		}

	con = (UDPCONN)calloc(1, sizeof(struct udp_conn));
	if(con == 0) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG || NDEBUG & PROTERR)
			printf("UDP: Couldn't allocate conn storage.\n");
#endif
		return 0;
		}


	if(firstudp) ocon->u_next = con;
	else firstudp = con;

	con->u_next = 0;

	con->u_lport = lsock;		/* fill in connection info */
	con->u_fport = fsock;
	con->u_fhost = fhost;
	con->u_rcv   = handler;
	con->u_data  = data;
	
	return con;
	}

UDPCONN	udp_ckcon(fhost, fsock)
	in_name		fhost;
	unsigned	fsock;{
	register UDPCONN con;

	con = firstudp;
	while (	!(
		   (con == 0) ||
			(con->u_fport == fsock &&
			 con->u_fhost == fhost) 
		 )
		)
		con = con->u_next;
	return con;

	}  /*  end of udp_ckcon()  */
	
