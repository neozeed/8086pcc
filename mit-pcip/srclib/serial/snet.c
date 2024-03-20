/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* 10/3/84 - removed Init1(); changed to handle the new net structure
						<John Romkey>
*/

#include <net.h>
#include <ah.h>

#define NULL	0

/* This is the network configuration file for the new IP code. This file
	configures only for the serial line net.	*/

int Nnet = 1;		/* The number of networks. */

/* random fnctns. */
int sl_init(), sl_write(), sl_open(), sl_close(), sl_stat();

NET nets[1] = { "Serial Line", sl_init, sl_write, 0, sl_close, 0, 0,
		BAUD_9600, 0, 900, 4, 0, 0L, sl_stat, 0L, &custom };

static struct net SLNET;
