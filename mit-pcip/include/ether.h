/*  Copyright 1983 by the Massachusetts Institute of Technology  */

/* Definitions of options for the ethernet initialization routine. */
#define	LOOPBACK	0x01
#define	ALLPACK		0x02
#define	MULTI		0x04

struct ethhdr {
	char	e_dst[6];
	char	e_src[6];
	unsigned	e_type;	};


