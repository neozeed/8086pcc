/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>

/* Initialize the UDP layer; get an internet connection, initialize the
	demux table */

IPCONN udp;
int udpdemux();

UdpInit() {
	if((udp = in_open(UDPPROT, udpdemux)) == 0) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG || NDEBUG & PROTERR)
			printf("UDP: Couldn't open InterNet connection.\n");
#endif
		}
#ifdef	DEBUG
	else if(NDEBUG & INFOMSG)
		printf("UDP: Opened InterNet connection.\n");
#endif
	}
