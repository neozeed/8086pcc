/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <net.h>
#include "ether.h"

/* Shutdown the ethernet interface */

et_close() {
	int vec;	/* crock to avoid compiler bug */

	int_off();
	if(!custom.c_otheruser) {
		vec = custom.c_intvec;
		outb(IIMR, inb(IIMR) | (1 << vec));
		}

	et_unpatch();
	int_on();

	}
