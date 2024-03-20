/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* This file contains the addresses of registers in the 8237-2 DMA
	controller for the PC and bit assignments in those registers. */

/* Bits. */
/* Command register */
#define	DMMEMTOMEM	0x01	/* Enable memory to memory transfers */
#define	DMCHANZERO	0x02	/* Channel 0 address hold */
#define	DMCONTROLLER	0x04	/* Controller enable/disable */
#define	DMTIMING	0x08
#define	DMPRIORITY	0x10
#define	DMLATEWRITE	0x20
#define	DMDREQSENSE	0x40
#define	DMDACKSENSE	0x80

/* Mode register bits */
#define	DMCHAN0		0x00
#define	DMCHAN1		0x01
#define	DMCHAN2		0x02
#define	DMCHAN3		0x03
#define	B_DMVERIFY	0x00
#define	B_DMWRITE		0x04
#define	B_DMREAD		0x08
#define	DMAUTOINIT	0x10
#define	DMADDINCDEC	0x20
#define	DMDEMAND	0x00
#define	DMSINGLE	0x40
#define	DMBLOCK		0x80
#define	DMCASCADE	0xC0

/* Request register bits */
#define	DMREQUEST	0x04

/* register locations */
#define	DMADDR1		0x02
#define	DMCOUNT1	0x03
#define	DMADDR3		0x06
#define	DMCOUNT3	0x07
#define	DMSTAT		0x08
#define	DMREQ		0x09
#define	DMSMASK		0x0A
#define	DMMODE		0x0B
#define	DMBYTEP		0x0C

/* better versions of the register ports. Add 2*channel to get the right
	port */
#define	DMADDR		0x00
#define	DMCOUNT		0x01

/* some commonly used constants - add the channel number to them
	before using them
*/
#define	DMMRST	0x04	/* write to DMSMASK */
#define	DMMSET	0x06
#define	DMWRITE	0x44	/* write to DMMODE */
#define	DMREAD	0x48

/* page registers for dma channels:
	channel		register
	  0		not usable for dma
	  1		0x83
	  2		0x81
	  3		0x82
*/
