/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <tcp.h>

tcp_dump(p,dlen,ph,phlen)
	PACKET 	p; 
	int	dlen;
	struct 	tcpph	*ph;
	int	phlen; {
	register struct tcp *pit;
	register char *data;
	int i, j;

	data = (char *)(p->nb_buff);
	pit = (struct tcp *)in_data(in_head(p));

	printf ("\ntcp data len = %8d\t, psudo header len = %8d\n", dlen, phlen);

	for(i=0; i<5; i++) {
		for(j=0; j<24; j++) printf ("%02x ",(char)*data++ & 0xff);
		prints("\n\r");
	}
	printf ("Pseudo-header:");
	data = (char *) ph;
	for (i=0;i<phlen;i++) printf("%02x ", (char) *data++ & 0xff);
	return; }


lprint(l)
	long l; {

	printf("%02x%02x", (unsigned)((l & 0xffff0000L) >> 16),
			   (unsigned)( l & 0x0000ffffL));

	return; }


unsigned tcpdrop = 0;

tcp_stats() {
	printf("TCP has dropped %u packets.\n", tcpdrop);
}

