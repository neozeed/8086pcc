/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include "udp.h"
static unsigned socket = 0;
extern long cticks;

unshort udp_socket() {

	if(socket) return socket++;

	socket = cticks;
	if(socket < 1000) socket +=1000;
	return socket++;
	}

