/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include	<stdio.h>
#include	<net.h>
#include	"menu.h"

#define	BYTESTODO	sizeof(struct custom)
struct custom	custom;
struct custom   custom2;

extern	struct menu_ent top[];
extern  int errno;
extern  char *sys_errlist[];

long	get_dosl();

main(argc,argv)
int	argc;
char	*argv[];
{
	int	infd;
	int	tfd;
	char	temp[80];
	int	ret;
	int	count;

	scr_init();		/* initialize the screen package */
	if (argc < 2 || argc > 3) {
		printf("Usage: customize program [sample file]\n");
		quit(1);
	}

	strcpy(temp, argv[1]);
	strcat(temp, ".sys");
	infd = open(temp, 2);
	if (infd < 0) {
		printf("Error opening %s: %s\n", temp, sys_errlist[errno]);
		quit(1);
	}
	mkraw(infd);
	read(infd, &custom, BYTESTODO);

	if(custom.c_iver != CVERSION) {
		close(infd);
		printf("\tYou attempted to customize a program that has an\n");
		printf("old version number or one that is not customizeable.\n");
		quit(1);
	}

	if (argc == 3) {
		strcpy(temp, argv[2]);
		strcat(temp, ".sys");
		tfd = open(temp, 2);
		if (tfd == -1) {
			printf("Couldn't open %s: %s\n", temp,
							sys_errlist[errno]);
			close(infd);
			quit(1);
		}
		else {
			mkraw(tfd);
			read(tfd, &custom2, BYTESTODO);
			close(tfd);
			if (custom2.c_iver != CVERSION) {
				printf("Error: The template file does not have a current\n");
				printf("version of the custom structure. The copy has been\n");
				printf("aborted\n");
				close(infd);
				quit(1);
			}
			cust_copy(&custom2, &custom);
		}
	}

	ret = menu(top);
	printf("\n");
	if (ret != 42) {
		close(infd);
		quit(0);
	}

	/* Now save the data back. */

#define	DATE	0x2a
#define	TIME	0x2c

	custom.c_ctime = get_dosl(TIME);	/* Set the time and date of */
	custom.c_cdate = get_dosl(DATE);	/* the last modification */

	strcpy(temp, argv[1]);
	strcat(temp, ".sys");
	infd = open(temp, 1);
	mkraw(infd);
	write(infd, &custom, BYTESTODO);	/* write block */
	close(infd);
	quit(0);
}

cust_copy(from, to)
struct custom	*from;
struct custom	*to;
{
	int	i;
	
	to->c_baud = from->c_baud;
	to->c_debug = from->c_debug;
	to->c_1custom = from->c_1custom;
	to->c_me = from->c_me;
	to->c_log = from->c_log;
	to->c_defgw = from->c_defgw;
	to->c_cookie = from->c_cookie;
	to->c_printer = from->c_printer;
	to->c_scribe = from->c_scribe;
	to->c_subnet_bits = from->c_subnet_bits;
	to->c_net_mask = from->c_net_mask;
	to->c_route = from->c_route;

	to->c_numtime = from->c_numtime;
	for (i = 0; i < MAXTIMES; i++)
		to->c_time[i] = from->c_time[i];
	to->c_numname = from->c_numname;
	for (i = 0; i < MAXNAMES; i++)
		to->c_names[i] = from->c_names[i];

	strcpy(to->c_user, from->c_user);
	strcpy(to->c_office, from->c_office);
	strcpy(to->c_phone, from->c_phone);

	to->c_tmoffset = from->c_tmoffset;
	strcpy(to->c_tmlabel, from->c_tmlabel);

	to->c_route = from->c_route;

	to->c_seletaddr = from->c_seletaddr;
	for (i = 0; i < sizeof(struct etha); i++)
		((char *)(&to->c_myetaddr))[i] =
			((char *)(&from->c_myetaddr))[i];

	for (i = 0; i < (3 * (sizeof(struct etha) + sizeof(in_name))); i++)
		((char *)(to->c_ether))[i] = ((char *)(from->c_ether))[i];

	for(i=0; i < 3; i++)
		to->c_ipname[i] = from->c_ipname[i];

	to->c_intvec = from->c_intvec;
	to->c_dmachan = from->c_dmachan;
	to->c_base = from->c_base;
	to->c_rvd_base = from->c_rvd_base;
}

quit(status)
	int status; {

	scr_close();
	_curse();
	exit(status);
	}
