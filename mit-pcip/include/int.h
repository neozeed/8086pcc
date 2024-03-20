/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* This header file defines the addresses of registers in the 8259A interrupt
	controller in the IBM PC and the bits in these registers. */

/* registers */
#define	IOCWR		0x20	/* operation control word register */
#define	IIMR		0x21	/* interrupt mask register */

/* OCW bits */
#define	IEOI5		0x65	/* End of interrupt, line 5 */
#define	IRDIIR		0x0A	/* Read the interrupt ID register */
#define	IRDISR		0x0B	/* Read the interrupt status register */

/* IMR bits */
#define	IMASK5		0xDF	/* mask interrupt 5 active */

