/*  Copyright 1984 by Proteon, Inc. */
/*  See permission and disclaimer notice in file "proteon-notice.h"  */
#include	"proteon-notice.h"


/* 10/3/84 - removed Init1(); changed to handle the new net structure
						<John Romkey>
*/

#include <net.h>
#include "v2.h"

#define NULL	0

/* This is the network configuration file for the new IP code. This file
	configures only for a single proNET interface. Applications
	which wish to use multiple net interfaces should have their
	own configuration file.
*/

int Nnet = 1;		/* The number of networks. */

/* random fnctns. */
int v2_init(), v2_send(), v2_stat(), v2_close();

NET nets[1] = {"Pronet", v2_init, v2_send, 0, v2_close, 0, 0, 0, 0,
	900, sizeof(struct v2hdr), 0, 0L, v2_stat, 0L, &custom };
