/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <netbuf.h>

extern NET nets[];
extern unsigned Nnet;

net_stats(fd)
	FILE *fd; {
	int i;

	fd = stdout;

	for(i=0; i<Nnet; i++) (*nets[i].n_stats)(fd);

	fprintf(fd, "Packet stats: %4u free packets now\n", freeq.q_len);
	fprintf(fd, "Min depth: %4u\tMax depth: %4u\n", 
					freeq.q_min, freeq.q_max);
	}
