/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <stdio.h>
#include <netbuf.h>
#include "v2.h"

extern unsigned v2badfmt, v2int, v2parity, v2overrun, v2toobig, v2punted;
extern unsigned v2toosmall, v2tx, v2rcv, v2ref;

v2_stat(fd)
	FILE *fd; {

	fprintf(fd, "Pronet interface: %u interrupts\n", v2int);
	fprintf(fd, "txed packets %u\trcvd packets %u\n", v2tx, v2rcv);
	fprintf(fd, "badfmt %u\tparity %u\toverrun %u\n", v2badfmt, v2parity,
							v2overrun);
	fprintf(fd, "too big %u\ttoo small %u\tpunted %u\n", v2toobig,
							v2toosmall, v2punted);
	fprintf(fd, "refused %u\n", v2ref);
	fprintf(fd, "icsr = %02x\tocsr = %02x\n", inb(mkv2(V2ICSR)),
						inb(mkv2(V2OCSR)));
	in_stats(fd);
	}
