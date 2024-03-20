/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <net.h>

/* This routine is called from netcrt to shut down the network. It calls
	the net_close routine associated with each network interface which
	should disable the interface and patch back any interrupt vectors
	which it might have been using.
*/

extern int Nnet;
extern NET nets[];

netclose() {
	int i;

	for(i=0; i<Nnet; i++)
		if(nets[i].n_close) (*(nets[i].n_close))();

	crock_c();
	brk_c();
	}
