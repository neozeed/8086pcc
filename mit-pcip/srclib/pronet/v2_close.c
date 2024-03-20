/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <net.h>
#include "v2.h"

/* Shutdown the v2 lni */

v2_close() {
	int i;

	if(!custom.c_otheruser) {
		outb(V2OCSR, 0);
/*		outb(V2ICSR, 0); */
		outb(V2ICSR, MODE1|MODE2);
		i = custom.c_intvec;
		outb(IIMR, inb(IIMR) | (1 << i));
		}

	v2_unpatch();

	}
