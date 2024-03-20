/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

#include <netbuf.h>
#include "v2.h"

/* Process an incoming packet from the ring. The only protocol supported is
	Internet; I have no expectation for address resolution over V2 Rings
	yet.
*/

#define	TRUE	1

unsigned v2drop = 0;	/* # of packets dropped */
unsigned v2multi = 0;	/* # of times more than one packet on queue */
unsigned v2toosmall = 0;

/*



		This space intentionally left blank.








*/
