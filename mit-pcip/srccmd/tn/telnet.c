/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/*  Modified to send octal internet address on F10/control-I, to put
a blank after octal and decimal internet address, to leave the line-25
message containing "My internet address" displayed indefinitely, to
leave the message "file transfer in progress" on line 25 for the
duration of the transfer, to accept an upcall from the tftp server
when the transfer is complete, to display a message on line 25
indicating success or failure of that transfer, and to leave the
line-25 message "closing connection" displayed indefinitely, 12/23/83.
F10/A toggles TCP tracing, 12/28/83.
Line 25 message for tftp now gives name of file and host, 1/2/84.
					<J. H. Saltzer>
2/3/84 - changed to allow the status line (25th line) display to be
	turned off. <John Romkey>
2/8/84 - commented out call to scr_close() and _curse() because the
	emulator's stdne() already does it.	<John Romkey>
2/24/84 - changed code to use new printf internet address fields
	instead of having duplicated code all over the place.
						<John Romkey>
8/12/84 - added a space in the new printf() calls after the IP address.
						<John Romkey>
8/30/84 - added F10A to turn off TFTP server asking.
						<John Romkey>
*/

#include	<ip.h>
#include	"telnet.h"
#include	<em.h>
#include	<tftp.h>

#define	C_L	0x0c	/* Control-L */

extern long cticks;	/* number of clock ticks since program start */
extern int wrap_around;	/* h19 emulator wrap-around flag */
extern int ba_bs;	/* h19 emulator backarrow key flag */
extern unsigned clear25;
extern in_name tnhost;
extern int TIMERDEBUG;
extern int NBUF;

int chartest;
int scrntest;
int ct1time;
int ct2time;
int ctcount;
int st1time;
int st2time;
int index;
char buf1[80];
char numchar[10];
/*
char flashbuf[600] = "No flash messages\n";
*/

tel_init() {
	register struct ucb *pucb;

	pucb = &ucb;

	pucb->u_state = ESTAB;
	pucb->u_tftp = TFUNKNOWN;
	pucb->u_tcpfull = 0;
	pucb->u_rstate = NORMALMODE;
	pucb->u_rspecial = NORMALMODE;
	pucb->u_wstate = NORMALMODE;
	pucb->u_wspecial = NORMALMODE;
	pucb->u_sendm = custom.c_1custom & NLSET ? NEWLINE : EVERYC;
	pucb->u_echom = LOCAL;
	pucb->u_echongo = NORMALMODE;
	pucb->u_mode = TRUE;
	pucb->u_ask = custom.c_1custom & TN_TFTP_ASK ? FALSE : TRUE;
	chartest = 0;
	scrntest = 0;
	ct1time = 0;
	ct2time = 0;
	st1time = 0;
	st2time = 0;

	if(custom.c_1custom & BSDEL) ba_bs = !ba_bs;
	if(custom.c_1custom & WRAP) wrap_around = !wrap_around;
	}

tel_exit() {
	extern int x_pos, y_pos, pos;

	norm25();
	move_lines(1, 0, 24);
	set_cursor(pos = 24*80);
	x_pos = 0;
	y_pos = 24;
	scr_close();
	_curse();
	}

/* Return true if telnet must run; false otherwise. */

mst_run() {
	return 1; /*fready(stdin);*/
	}

bfr() {
	ucb.u_tcpfull = 0;
	clr25();
	}

/* gt_usr  Read nch chars from user's terminal
* When prompt char is encountered, go into
*  SPECIAL read mode and handle char following
*  prompt specially.
* Tc_put puts telnet chars into an output
*  to net packet.
*/

gt_usr() {
	register struct ucb	*pucb;
	int	c;
	int	i;
	int	ich;
	static 	int	inc;
		
	pucb = &ucb;

	for(;;) {
	tk_yield();

	while((c = h19key()) != NONE) {
		switch(pucb->u_rspecial) {
		case HOLD:
			continue;
		case NORMALMODE:
			if(c == pucb->u_prompt)  {
				pucb->u_rspecial = SPECIAL;
				clear25 = 0;
				inv25();
				clr25();
				pr25(0, "Command: ");
				break;
				}

			if(pucb->u_tcpfull) {
				tcpfull();
				break;
				}

			if(pucb->u_echom == LOCAL) {
				if(c == '\r') putchar('\n');

				if(c == '\n' || c == '\r' || c >= ' ' ||
				   c == 9    || c == 5    || c == 8)
								putchar(c);
				else if(c == 27) putchar("$");
				else if(c == F9) {
					putchar('F');
					putchar('9');
					}
				else if(c == F10) {
					putchar('F');
					putchar('1');
					putchar('0');
					}
				else if(c == C_BREAK || c == C_L) ;
				else {
					putchar('^');
					putchar(c + '@');	}
				}


			if(c == '\r') c = '\n';
			else if(c == '\n') tc_put('\r');
			else if(c == IAC) tc_put(IAC);
			else if(c == C_BREAK) {
				tc_put(IAC);
				tc_put(INTP);
				tc_put(IAC);
				tc_put(DM);
				tcpurgent();
				break;
				}

			if(pucb->u_sendm == EVERYC) {
				if(tc_fput(c)) tcpfull();
				}
			else {
				if(tc_put(c)) {
					tcpfull();
					break;
					}
				if(c == '\n') tcp_ex();
				}
			break;

		case SPECIAL:
			pucb->u_rspecial = NORMALMODE;
/*			if(c == pucb->u_prompt) {
				if(pucb->u_sendm == EVERYC) {
					if(tc_fput(c)) tcpfull();
					}
				else
					if(tc_put(c)) tcpfull();	
				break;
				}
*/
			switch(c) {
			case 'q':
				pucb->u_rspecial = CONFIRM;
				clr25();
				prbl25("Dangerous, confirm with Y: ");
				break;

/*			case 'C': {
				int fd;

				pr25(9, "Dumping core");
				fd = creat("core", 1);
				if(fd < 0) {
					perror("couldn't open core file\n");
					break;
					}
				write(fd, 0, 65535);
				close(fd);
				printf("done\n");
				tel_exit();
				exit(1);
				}
				break;
*/

			case '!': {
				extern char *sys_errlist[];
				extern int errno;
				int retcode;

				norm25();
				clear_lines(24, 1);
				printf("\nEntering inferior command interpreter. Do not execute any network programs.\n");
				printf("Use the EXIT command to return to telnet\n");
				_curse();
				scr_close();
				retcode = system(0);
				scr_init();
				if(retcode < 0)
					printf("Can't exec command interpreter: %s\n", sys_errlist[errno]);
				else puts("");
				break;
				}
			case 'O':
				TDEBUG = !TDEBUG;
				pr25(9, "Toggle task tracing.");
				norm25();
				clear25 = 4;
				break;

			case 'A':
				pucb->u_ask = !pucb->u_ask;
				pr25(9, "Toggle tftp server asking.");
				norm25();
				clear25 = 4;
				break;

			case 'u':
				pucb->u_mode = FALSE;
				norm25();
				clear_lines(24, 1);
				break;

			case 'U':
				pucb->u_mode = TRUE;
				pr25(9, "Turn on status line.");
				norm25();
				clear25 = 4;
				break;

			case 'M':
				TIMERDEBUG = !TIMERDEBUG;
				pr25(9, "Toggle timer tracing.");
				norm25();
				clear25 = 4;
				break;

			case 'P':
				NDEBUG = (NDEBUG ^ APTRACE);
				pr25(0, "Toggle TCP tracing.               ");
				norm25();
				clear25 = 4;
				break;
			
			case 'L':
				udp_table();
/*				space_display();	*/
				pr25(9, "Active UDP connections.");
				norm25();
				clear25 = 4;
				break;

			case 'T':
				tfs_on();
				pr25(9, "File transfer service turned on.");
				norm25();
				clear25 = 4;
				break;

			case 't':
				tfs_off();
				pucb->u_tftp = TFNO;
				pr25(9, "File transfer service turned off.");
				norm25();
				clear25 = 4;
				break;

			case 'I':
				{
				in_name me;
				char buffer[20];

				me = in_mymach(tnhost);
				sprintf(buffer, "%A = %a",me, me);
				pr25(0, "My internet address is ");
				pr25(23, buffer);
				norm25();
				clear25 = 0;
				}
				break;

			case 'i':
				{
				in_name me;
				char buffer[20];

				me = in_mymach(tnhost);
				sprintf(buffer, "%a ", me);

				ich = 0;
				for(ich = 0; buffer[ich]; ich++) {
					if(tc_put(buffer[ich])){
						tcpfull();
						break;
						}
					if(pucb->u_echom == LOCAL)
						putchar(buffer[ich]);
					}

				if(pucb->u_sendm == EVERYC) tcp_ex();

			       pr25(0,"Send my internet address in decimal.");
				norm25();
				clear25 = 2;
				}
				break;

			case '\t':
				{
				in_name me;
				char buffer[20];

				me = in_mymach(tnhost);
				sprintf(buffer, "%A ", me);

				ich = 0;
				for(ich = 0; buffer[ich]; ich++) {
					if(tc_put(buffer[ich])){
						tcpfull();
						break;
						}
					if(pucb->u_echom == LOCAL)
						putchar(buffer[ich]);
					}

				if(pucb->u_sendm == EVERYC) tcp_ex();

				pr25(0,"Send Internet address in octal");
				norm25();
				clear25 = 2;
				}
				break;

			case 'c':
				tcp_close();
				pucb->u_state = CLOSING;
				pucb->u_rstate = BLOCK;
				pr25(9, "Closing connection.");
				norm25();
				clear25 = 0;
				break;
/*
			case 'S':
				if(!tcp_save()) {
					prbl25("Suspend failed!");
					clear25 = 0;
					break;
					}
				pr25(9, "Suspending telnet.");
				norm25();
				clear25 = 0;
				tel_exit();
				exit();
*/
			case 'n':
				pucb->u_tftp = TFNO;
				pr25(9, "File transfer refused.");
				norm25();
				clear25 = 4;
				break;


			case 'y':
				pucb->u_tftp = TFYES;
				clr25();
				pr25(0, buf1);
				norm25();
				clear25 = 0;
				break;

			case 'N':
				net_stats(stdout);
				printf("NBUF = %d\n", NBUF);
				pr25(9, "Network statistics.");
				norm25();
				clear25 = 4;
				break;

			case 'D':
				ba_bs = 0;
				pr25(9, "<-- key is delete.");
				norm25();
				clear25 = 4;
				break;

			case 'B':
				ba_bs = 1;
				pr25(9, "<-- key is backspace.");
				norm25();
				clear25 = 4;
				break;

			case 'w':
				wrap_around = 1;
				pr25(9, "Turn end-of-line wrap-around on.");
				norm25();
				clear25 = 4;
				break;

			case 'd':
				wrap_around = 0;
				pr25(9, "Turn end-of-line discard on.");
				norm25();
				clear25 = 4;
				break;

			case 'E':
				pucb->u_sendm = NEWLINE;
				pr25(9, "Send on end-of-line.");
				norm25();
				clear25 = 4;
				break;

			case 'e':
				pucb->u_sendm = EVERYC;
				pr25(9, "Send on every character.");
				norm25();
				clear25 = 4;
				break;

			case 'l':
				pr25(9, "Local echo mode.");
				norm25();
				clear25 = 4;
				if(pucb->u_echom == LOCAL)
					break;
				echolocal(pucb);
				pucb->u_echongo = LECHOREQ;
				break;

			case 'r':
				pr25(9, "Remote echo mode.");
				norm25();
				clear25 = 4;
				if(pucb->u_echom == REMOTE)
					break;
				echoremote(pucb);
				pucb->u_echongo = RECHOREQ;
				break;

			case 'a':
				pr25(9, "Sending Are You There.");
				norm25();
				clear25 = 4;
				if(tc_put(IAC)) {
					tcpfull();
					break;
					}
				if(tc_fput(AYT)) tcpfull();
				break;

			case 'z':
				tc_put(IAC);
				tc_fput(AO);
				pr25(9, "Sending abort output.");
				norm25();
				clear25 = 4;
				break;
				
			case 'b':
			case C_BREAK:
				tc_put(IAC);
				tc_put(INTP);
				tc_put(IAC);
				tc_put(DM);
				tcpurgent();
				pr25(9, "Sending break.");
				norm25();
				clear25 = 4;
				break;

			case 'x':
				tcp_ex();
				pr25(9, "Expediting data.");
				norm25();
				clear25 = 4;
				break;
				
			case '?':
				showcmds();
				pr25(9, "Help.");
				norm25();
				clear25 = 4;
				break;

			case 's':
				showstats();
				pr25(9, "TCP statistics.");
				norm25();
				clear25 = 4;
				break;
			case '+':
				tk_stats();
				pr25(9, "Tasking statistics.");
				norm25();
				clear25 = 4;
				break;

			default:
				clr25();
				norm25();
				if (c != pucb->u_prompt){
					prerr25("Unknown command.");
					clear25 = 4;}
				}
			rst_screen();
			break;

		case CONFIRM:
			pucb->u_rspecial = NORMALMODE;
			if(c == 'y' || c == 'Y') {
				clr25();
				pr25(0, "Confirmed, quitting.");
				tel_exit();
				exit();
				}
			clr25();
			pr25(0, "Quit aborted.");
			norm25();
			clear25 = 4;
			break;

		default:
			printf("\nTelnet BUG %u\n", pucb->u_rspecial);
			pucb->u_rspecial = NORMALMODE;
			break;
			}
		}
	}
}

/* wr_usr  manage chars coming from net and
*   going to user
* Process received telnet special chars and
*  option negotiation.  When wstate is URGENTM,
*  only process special chars.
* All interrupts should be turned off
*  when in this routine - write to terminal
*  may block; an interrupt would cause
*  an error return from write with resulting
*  loss of chars to terminal
*/
wr_usr(buf, len, urg)
	char *buf;
	int len;
	int urg; {
	register struct ucb	*pucb;
	register char	*p;
	int c;

	pucb = &ucb;

	for(p = buf; p < buf+len; p++) {
		c = (*p & 0377);
		switch(pucb->u_wspecial) {
		case NORMALMODE:
			switch(c) {
			case IAC:
				pucb->u_wspecial = IAC;
				break;
			default:
				/* Don't print ^L because Multics sends them
					quite often */
				if(pucb->u_wstate != URGENTM && c != C_L)
								em(c); 

			}
			break;

		case IAC:
			switch(c) {
			case IAC:
				if(pucb->u_wstate != URGENTM)
					putchar(c);
				pucb->u_wspecial = NORMALMODE;
				break;

			case AO:
				tc_put(IAC);
				tc_put(DM);
				tcpurgent();
				pucb->u_wspecial = NORMALMODE;
				break;

			case WILL:
			case WONT:
			case DO:
			case DONT:
				pucb->u_wspecial = c;
				break;

/*			case GA:
				pucb->u_wspecial = NORMALMODE;
				putchar('G');
				putchar('A');
				break;		*/
			
		/* Ignore IAC x */
			default:
				pucb->u_wspecial = NORMALMODE;
				break;
				}
			break;

		case WILL:
			switch(c) {
			case OPTECHO:
				switch(pucb->u_echongo) {
		/* This host did not initiate echo negot - so respond */
				case NORMALMODE:
					if(pucb->u_echom != REMOTE)
						echoremote(pucb);
					break;
		/* Rejecting my IAC DONT ECHO  (illegit) */
				case LECHOREQ:
					ttechoremote(pucb);
					break;
					}			
				
				pucb->u_echongo = NORMALMODE;
				break;

			case OPTSPGA:	/* suppress GA's */
				tc_put(IAC);
				tc_put(DO);
				tc_fput(c);
				break;

			default:
				tc_put(IAC);
				tc_put(DONT);
				tc_fput(c);
				break;
				}

			pucb->u_wspecial = NORMALMODE;
			break;

		case WONT:
			switch(c) {
			case OPTECHO:
				switch(pucb->u_echongo) {
		/* This host did not initiate echo negot - so respond */
				case NORMALMODE:
					if(pucb->u_echom != LOCAL)
						echolocal(pucb);
					break;
		/* Rejecting my IAC DO ECHO */
				case RECHOREQ:
					ttecholocal(pucb);
					break;
					}			
				
				pucb->u_echongo = NORMALMODE;
				break;
				}

			pucb->u_wspecial = NORMALMODE;
			break;

		case DO:
			tc_put(IAC);
			tc_put(WONT); 
			tc_fput(c);
			pucb->u_wspecial = NORMALMODE;
			break;

		case DONT:
			pucb->u_wspecial = NORMALMODE;
			break;
			}
		}
}


echolocal(pucb)
register struct ucb	*pucb;
{
	tc_put(IAC);
	tc_put(DONT);
	tc_fput(OPTECHO);
	pucb->u_echom = LOCAL;
/*	foptions(STECHO);	*/
}

ttecholocal(pucb)
	register struct ucb	*pucb; {

	pucb->u_echom = LOCAL;
/*	foptions(STECHO);	*/
	}

echoremote(pucb)
	register struct ucb	*pucb; {

	tc_put(IAC);
	tc_put(DO);
	tc_fput(OPTECHO);
	pucb->u_echom = REMOTE;
/*	foptions(STNECHO);		*/
	}

ttechoremote(pucb)
	register struct ucb	*pucb; {

	pucb->u_echom = REMOTE;
/*	foptions(STNECHO);		*/
	}

opn_usr() {
	printf("Open\n"); }

cls_usr() {
	printf("Closed\n");
	tel_exit();
	exit(); }

tmo_usr() {
	printf("Host not responding\n");
	tel_exit();
	exit(); }

pr_dot() {
	putchar('.');
	}

tcpfull() {
	ucb.u_tcpfull = 1;
	clr25();
	norm25();
	prbl25("Output buffer full");
	clear25 = 0;
/*	fullbell();	*/
	}


tntftp(host, file, dir)
	in_name host;
	char *file;
	unsigned dir; {
	char buffer[80];
	char c;

	ring();

	if(host == tnhost){
	sprintf(buffer,"Host %s wants to %s file %s; OK? [F10/y or F10/n]",
			tnshost,  dir == PUT ? "read" : "write",file);
	sprintf(buf1,"Host %s is %s file %s", 
			tnshost, dir == PUT ? "reading" : "writing", file);
			}
	else		{
	sprintf(buffer, "Host %a wants to %s file %s; OK? [F10/y or F10/n]",
				host, dir == PUT ? "read" : "write",file);
	sprintf(buf1, "Host %a is %s file %s",
			host, dir == PUT ? "reading" : "writing", file);
			}

	if(ucb.u_ask) {
		inv25();
		pr25(0, buffer);
		clear25 = 0;
		}
	else {
		pr25(0, buf1);
		clear25 = 10;
		}

/*if(FALSE)return true;*/	/*TEMPORARY DEBUGGING AID*/

	if(ucb.u_ask) {
		ucb.u_tftp = TFWAITING;

		while(ucb.u_tftp == TFWAITING) tk_yield();

		if(ucb.u_tftp == TFYES)
			return TRUE;
		else
			return FALSE;
		}
	else return TRUE;

	}  /*  end of tntftp()  */

/*  function called when file transfer is done.  */

tntfdn(success) 
	int	success;
	{
	clr25();
	if (success) pr25(0, "File transfer successful.");
	    else     pr25(0, "File transfer failed.");
	ring();
	*buf1 = '\0';
	clear25 = 6;
	}
