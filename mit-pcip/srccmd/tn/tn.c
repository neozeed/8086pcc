/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include	<ip.h>
#include	"telnet.h"

/*  Modified 12/23/83 to include upcalled "done" function in initialization
    of tftp server.  <J. H. Saltzer>
    Modified 1/2/84 to get window and low window sizes from the custom
    structure. <John Romkey>
    11/15/84 - fixed screen condition on error exit.
					<John Romkey>
*/

#define	WAITCLS		5		/* close wait time */


struct ucb	ucb;
struct	task		*TNsend;		/* telnet data sending task */
int	speed;
in_name tnhost;

extern int wr_usr(), mst_run(), opn_usr(), cls_usr(), tmo_usr(), pr_dot();
extern int bfr();
extern int tn_flash();

extern int tntftp();
extern int tntfdn();

char usage[] = "usage: telnet [-p port] host\n";

main(argc, argv)
	int	argc;
	char	*argv[]; {

	if(!main_init(argc, argv))
		exit(1);

	tel_init();
	gt_usr();
	}

int tel_exit();

main_init(argc, argv)
	int	argc;
	char	*argv[]; {
	in_name	fhost;
	unsigned sock;
	unsigned fsock = TELNETSOCK;

	scr_init();
	exit_hook(tel_exit);

	if(argc < 2 || argc > 4) {
		printf("telnet: improper arguments.\n");
		printf(usage);
		return FALSE;
		}

	if(argc == 4) {
		if(strcmp(argv[1], "-p") == 0) {
			fsock = atoi(argv[2]);
			argv++; argv++;
			argc--; argc--;
			}
		else {
			printf(usage);
			return FALSE;
			}
		}

	if(argc != 2) {
		printf(usage);
		return FALSE;
		}

	/* Initialize tasking and save our task's name. */
	NBUF = 14;

	/* need lots of stack because of terminal emulator */
	tcp_init(5500, opn_usr, wr_usr, mst_run, cls_usr, tmo_usr, pr_dot, bfr);

	if(argc == 3) NDEBUG = atoi(argv[2]);

	fhost = resolve_name(argv[1]);
	if(fhost == 0) {
		printf("Foreign host %s not known.\n", argv[1]);
		return FALSE;
		}

	if(fhost == 1) {
		printf("Name servers not responding.\n");
		return FALSE;
		}

	tnhost = fhost;

	/* setup the initial telnet screen */
	putchar(27);
	putchar('E');

	pr_banner(argv[1]);
	tnscrninit();
	tn_tminit();

	/* start the tftp server and turn it on */
	tfsinit(tntftp, tntfdn);
	tfs_on();

	printf("Trying...");

	sock = tn_sock();
	tcp_open(&fhost, fsock, sock, custom.c_telwin, custom.c_tellowwin);

	tk_yield();	/* let it start */

	return(TRUE);
	}
