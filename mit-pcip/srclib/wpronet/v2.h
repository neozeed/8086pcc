/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"

/* V2 LNI header file - defines packet header format and some constants.
*/

#include <dma.h>
#include <int.h>

/* register addresses (offsets from the base) */
#define	V2ICSR	0	/* input CSR */
#define	V2IBUF	1	/* input buffer */
#define	V2ILCNT	2	/* low byte of input buffer byte count */
#define	V2IHCNT	3	/* high byte of input buffer byte count */
#define	V2OCSR	4	/* output CSR */
#define	V2OBUF	5	/* output buffer */
#define	V2OLCNT	6	/* low byte of output buffer byte count */
#define	V2OHCNT	7	/* high byte of output buffer byte count */

/* should actually be a variable set from the net struct or the
	custom structure
*/
#define	v2_base	0x300	/* for now */
#define	mkv2(x)	(x)+v2_base

/* Bits. */
/* input CSR */
#define	COPYEN	0x01
#define	MODE1	0x02
#define	MODE2	0x04
#define	BADFMT	0x08
#define	PARITY	0x10
#define	INRST	0x20
#define	OVERRUN	0x20
#define	ININTEN	0x40
#define	ININTRES	0x80
#define	ININTSTAT	0x80

/* output CSR */
#define	ORIGEN	0x01
#define	REFUSED	0x02
#define	OBADFMT	0x04
#define	OUTTMO	0x08
#define	INITRING	0x10
#define	OUTRST	0x20
#define	RGNOK	0x20
#define	OUTINTEN	0x40
#define	OUTINTRES	0x80
#define	OUTINTSTAT	0x80


/* header includes hardware header and link-level (set by convention) header
*/
struct v2hdr {
	char	v2h_dst;
	char	v2h_src;
	long	v2h_type;
	};

#define	V2MINLEN	sizeof(struct v2hdr)

/* V2 LNI packet types */
#define	V2IP	0x00000102L


extern NET *v2_net;
extern task *V2Demux;
extern char _v2me;
extern unsigned v2_eoi;		/* end-of-interrupt command for 8259A */
