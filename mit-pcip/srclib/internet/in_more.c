/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <ip.h>

/* Return true if there are any enqueued, unprocessed packets in the
	system. */

extern NET nets[];

in_more() {

	return (int)nets[0].n_inputq->q_head;
	}
