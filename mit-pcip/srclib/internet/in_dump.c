/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>

/* This function dumps an internet packet to the screen. It uses some of the
	screen handling functions to help in this tedious chore. */

in_dump(p)
	PACKET p; {
	int i, j;
	register struct ip *pip;
	register char *data;
	unsigned xsum, osum;

	pip = in_head(p);
	
	data = p->nb_buff;

	printf("Pkt address = %04x\n\r", p);

	for(i=0; i < 14; i++) {
		for(j=0; j<24; j++) printf("%02x ",(char)*data++ & 0xff);
		prints("\n\r"); }

	/* Display header info in reasonable form */

	printf("Hdr len = %u\tIP Len = %u\tTotal Len =%u\n\r",
			pip->ip_ihl, bswap(pip->ip_len), p->nb_len);

	printf("Vers = %u\t\tType of Ser = %u\tProt = %u\tTTL = %u\n\r",
		pip->ip_ver, pip->ip_tsrv, pip->ip_prot, pip->ip_time&0xff);

	printf("Frag Offset = %u\tFlags = %u\n\r",pip->ip_foff,pip->ip_flgs);

	printf("\tSrc = %a\tDst = %a", pip->ip_src, pip->ip_dest);

	printf("\tID = %u\n\r", pip->ip_id);

	printf("\tXsum = %04x\t",pip->ip_chksum);
	osum = pip->ip_chksum;
	pip->ip_chksum = 0;
	xsum = ~cksum(pip, pip->ip_ihl << 1);
	printf("Computed xsum = %04x\t", xsum);
	pip->ip_chksum = osum;
	if(xsum == osum) prints("Xsum is CORRECT.\n\r");
	else prints("Xsum is NOT CORRECT\n\r");

	return;
	}
