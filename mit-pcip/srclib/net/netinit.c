/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* Get NET structs, PACKET structs, tasking and Queue stuff. */

#include <stdio.h>
#include <types.h>
#include <netbuf.h>

/* 8/12/84 - modified to work with version 8 of the custom structure.
						<John Romkey>
   10/1/84 - added a declaration of calloc to make work with the new i/o
	library.				<John Romkey>
   10/3/84 - changed code to work with new net structure.
						<John Romkey>
   11/30/84 - made netinit check if forward compatiblity with subnet
	masks is necessary. If mask is 0 then generate it.
						<John Romkey>
*/

/* The netinit() routine does the basic initialization at the lowest level
	of the PC networking environment. It does (in order):
	0. Custom initialization
	1. Packet buffer initialization, creation of the free queue.
	2. Tasking initialization.
	3. Per net initialization.

	Per protocol initialization is not done by this routine; it has to
	be done explicitly by the user. Thus if the applications program
	requires InterNet, it has to do in_init().
*/

#define	AMASK	0x80
#define	AADDR	0x00
#define	BMASK	0xC0
#define	BADDR	0x80
#define	CMASK	0xE0
#define	CADDR	0xC0

queue freeq;

int MaxLnh;			/* Length of the biggest local net header in
					the whole system */

extern int Nnet;		/* Number of network drivers */
extern NET nets[];		/* The actual network structs */

PACKET buffers[20];

struct custom custom;		/* foo */

int NBUF=NBUFINIT;		/* # of packet buffers */
int LBUF=LBUFINIT;		/* size of packet buffers */

unsigned NDEBUG = 0;		/* Debugging...*/

char *calloc();

int crock_c(), brk_c(), netclose();

Netinit(stack_size)
	int stack_size; {
	int i;			/* general counter variable */
	PACKET packet;
	unsigned physaddr;

	crock_init();
	exit_hook(crock_c);

	brk_init();
	exit_hook(brk_c);

	/* this whole business with the custom structure will have to
		change soon to support multiple net interfaces. There
		will be one structure associated with each interface.
		Each net structure will have a pointer to its custom
		structure. The device names will be "netcust", "netcust2",
		"netcust3", etc., though no program will probably use
		more than two interfaces. The global "struct custom"
		will still remain, though, and will be the custom
		structure of the first interface, from "netcust".
	*/

	/* read in the custom structure */
	i = open("netcust", 2);
	if(i < 0) {
		printf("Couldn't read in the custom structure, so you \n");
		printf("probably didn't install netdev.sys correctly.\n");
		exit(1);
		}

	mkraw(i);
	read(i, &custom, sizeof(struct custom));
	close(i);

	if(custom.c_cdate == 0L) {
		printf("It looks like netdev.sys probably wasn't\n");
		printf("customized before you booted. You should\n");
		printf("customize it and reboot before you try to run\n");
		printf("this program again.\n");
		exit(1);
		}

	if(custom.c_iver != CVERSION) {
		printf("This custom structure is the wrong version. That ");
		printf("means there's a mismatch\nbetween this version of ");
		printf("this program and the customization information.\n");
		exit(1);
		}

	NDEBUG = custom.c_debug;

	MaxLnh = nets[0].n_lnh;

	/* initialize freeq */
	freeq.q_head = freeq.q_tail = NULL;


	/* Create the queue of free packets. Format each packet and add it to
		the tail of the queue */

	for(i=0; i<NBUF; i++) {
		packet = (PACKET)calloc(1, sizeof(struct net_buf));
		if(packet == 0) {
			printf("NETINIT: Couldn't allocate pckt skeleton.\n");
			exit(1);
			}

		buffers[i] = packet;
		packet->nb_tstamp = 0;
		packet->nb_len = 0;
		packet->nb_buff = calloc(1, LBUF);

	/* Check if the packet buffer crosses a physical 64K boundary. If
		it does, throw it away and allocate another packet buffer.
		Because of the way the core allocator works and because
		of us only having 64K of data area, we should never get
		two of such packets. */

		physaddr = (unsigned)packet->nb_buff;
		physaddr = (physaddr >> 4) + get_ds();

		if(((physaddr + (LBUF>>4)) & 0xF000) > (physaddr & 0xF000)) {
#ifdef	DEBUG
			if(NDEBUG & INFOMSG) printf("found bad pkt buffer\n");
#endif
			packet->nb_buff = calloc(1, LBUF);
			}

		if(packet->nb_buff == NULL) {
			printf("Ran out of packet storage during NETINIT().");
			printf(" Program aborting.\n");
			exit(1);
			}

		putfree(packet);
		}

	freeq.q_max = freeq.q_len;
	freeq.q_min = freeq.q_len;

	/* The packet buffer system should now be initialized. It's time to
		go on to initializing tasking. */
#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Initializing Tasking.\n");
#endif

	tk_init(stack_size);		/* Do it! */
	tm_init();

	/* call the init procedure */
	nets[0].ip_addr = custom.c_me;
	nets[0].n_defgw = custom.c_defgw;
	nets[0].n_inputq = q_create();
	if(!nets[0].n_inputq) {
		printf("BUGHALT: couldn't allocate net queue\n");
		exit(1);
		}

	exit_hook(netclose);

	if(nets[0].n_init == NULL) {
		printf("BUGHALT: can't find net interface init code\n");
		exit(1);
		}

	(*nets[0].n_init)(&nets[0], nets[0].n_initp1, nets[0].n_initp2);

	if(nets[0].ip_addr == 0) {
		printf("BUGHALT: PC address not known.\n");
		exit(1);
		}


	/* check on subnet routing - if no mask then make one */
	if(custom.c_net_mask == 0) {
		if((custom.c_me & AMASK) == AADDR)
			custom.c_net_mask = (custom.c_route == SUBNET ?
					0xff : 0xffff);
		else if((custom.c_me & BMASK) == BADDR)
			custom.c_net_mask = (custom.c_route == SUBNET ?
					0xffff : 0xffffff);
		else if((custom.c_me & CMASK) == CADDR)
			custom.c_net_mask = 0xffffff;
		}

	/* Per net initialization is now finished. Can return to the user
		all happy and initialized in a warm tasking environment. */

	return;
	}
