/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* This header file includes descriptions of the register of the 3COM
	10Mb ethernet board for the IBM PC (hence known as "the ethernet").
	The actual packet transfer into memory should be done using DMA,
	not using the facilites provided on the card. DMA programming
	information is in <dma.h> and a similar header for the interrupt
	controller is in <int.h>. Both of these are included here. */

/* Ports on the ethernet board */
/* Read ports */
#define	EADDR		(custom.c_base+0) /* Address (read & write)	*/
#define	ERCVSTAT	(custom.c_base+6)	/* Receiver status */
#define	ETXSTAT		(custom.c_base+7)	/* Transmitter status */
#define	EGPPLOW		(custom.c_base+8)	/* General purpose buffer pointer low byte */
#define	EGPPHIGH	(custom.c_base+9)	/* General purpose buffer pointer high byte */
#define	ERBPLOW		(custom.c_base+0xA)	/* Receive buffer pointer low byte */
#define	ERBPHIGH	(custom.c_base+0xB)	/* Receive buffer pointer high byte */
#define	EADDRWIN	(custom.c_base+0xC)	/* Address PROM Window	*/
#define	EAUXSTAT	(custom.c_base+0xE)	/* Auxiliary Status Register */
#define	EBUFWIN		(custom.c_base+0xF)	/* Buffer Window */
/* write ports (as opposed to wrong ports) */
#define	ERCVCMD		(custom.c_base+0x6)	/* Receiver command */
#define	ETXCMD		(custom.c_base+0x7)	/* Transmitter command */
#define	ECLRRP		(custom.c_base+0xA)	/* Receive buffer pointer clear */
#define	EAUXCMD		(custom.c_base+0xE)	/* Auxiliary command register */

/* Bit definitions */
/* Transmit command register */
#define	EDCTUNDR	0x01	/* Detect underflow */
#define	EDCTCOLL	0x02	/* Detect collision */
#define	EDCTCSXT	0x04	/* Detect collision 16 */
#define	EDCTSCTX	0x08	/* Detect successful transmission */

/* Transmit status register */
#define	EUNDERFLOW	0x01
#define	ECOLLISION	0x02
#define	ECOLLSIXTEEN	0x04
#define	ERDYFORNEW	0x08	/* Ready for new frame */

/* Receive command register */
#define	EDTOVRFLOW	0x01	/* Detect overflow */
#define	EDTFCSERR	0x02	/* Detect FCS errors */
#define	EACCDRIBBLE	0x04	/* Accept dribble */
#define	EDTSHORTFRAMES	0x08	/* Detect short frames */
#define	EDTNOOVERFLOW	0x10	/* "Detect frames without overflow error */
#define	EACCGOODFRAMES	0x20	/* Accept good frames */
#define	ERCVDISABLE	0x00	/* Disable receiver */
#define	ERCVALLADDR	0x40	/* receive all addresses */
#define	ERCVBROAD	0x80	/* receive station addr and broadcast */
#define	ERCVMULTI	0xC0	/* receive station addr and multicast */

/* Receiver status */
#define	EOVERFLOW	0x01	/* Overflow error */
#define	EFCSERR		0x02	/* FCS error */
#define	EDRIBBLEERR	0x04	/* Dribble error */
#define	ESHORTFRAME	0x08	/* Short frame error */
#define	EGOODPACKET	0x10	/* Good packet received */
#define	ESTALESTAT	0x80	/* Status is stale */

/* Auxiliary command register */
#define	EEXMTBADFCS	0x01	/* Transmit bad FCS (for testing purposes) */
#define	ESYSBUS		0x00	/* System bus has access to packet buffer */
#define	EXMTRCV		0x04	/* Transmit followed by receive */
#define	ERECEIVE	0x08	/* Receive */
#define	ELOOPBACK	0x0C	/* Loopback */
#define	EDMAREQ		0x20	/* DMA Request */
#define	EINTDMAENABLE	0x40	/* Request Interrupt and DMA enable */
#define	ERESET		0x80	/* Reset */

/* Auxiliary status register */
#define	ERCVBUSY	0x01	/* the receiver is busy */
#define	EDMADONE	0x10	/* DMA complete */
#define	EXMITIDLE	0x80	/* Transmitter idle */

#include <dma.h>
#include <int.h>

/* Values of etherial state variables */

#define	NONBUSY		97	/* well, its different */
#define	DMASTART	1
#define	TXSTART		2
#define	TXCOMPLETE	3
#define	TIMEOUT		10
#define	WAITING		20

/* Ethernet protocol types: */
#define	ETIP	0x0008
#define	ETADR	0x0608	/* Dave Plummer's Address resolution protocol */
#define	ETCHAOS	0x0408

#define	ETMINLEN	60	/* minimum length packet */

/* The ethernet address copying function */
#define	etadcpy(a, b)	{ int _i;	\
	for(_i=0; _i<6; _i++) (b)[_i] = (a)[_i];	}


extern NET *et_net;	/* ET's net */
extern task *EtDemux;	/* ET's demultiplexing task */
extern char _etme[6];	/* my ethernet address */
extern char ETBROADCAST[6];	/* ethernet broadcast address */

