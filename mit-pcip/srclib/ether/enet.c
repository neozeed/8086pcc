/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 10/3/84 - removed Init1(); changed to handle the new net structure
						<John Romkey>
*/

#include <ether.h>
#include <net.h>

#define NULL	0

/* This is the network configuration file for the new IP code. This file
	sets up the configuration for a single ethernet interface
	machine. Programs which want to use multiple net interfaces
	should use their own configuration file.
*/

int Nnet = 1;		/* The number of networks. */

/* random fnctns. */
int et_init(), et_send(), et_stat(), et_close();

NET nets[1] = { "Ethernet", et_init, et_send, 0, et_close, 0, 0,
#ifdef	WATCH
	ALLPACK,
#endif
#ifndef	WATCH
	MULTI,	/* accept multicast packets just in case some are fun */
#endif
	0, 1400, 14, 0, 0L, et_stat, 0L, &custom };
