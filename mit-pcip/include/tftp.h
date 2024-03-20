/*  Copyright 1984 by the Massachusetts Institute of Technology  */

/* TFTP randoms */
#define	GET	10		/* GET file from other host to here */
#define	PUT	11		/* PUT file on other host */
#define	ASCII	1		/* transfer as netascii */
#define	IMAGE	2		/* transfer as image */
#define	TEST	3		/* test mode - diskless */
#define	OCTET	4		/* octet mode - same as image */

long tftpuse(), atol();
extern long cticks;
