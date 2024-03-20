/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include "tftp.h"

/* the tftp server code */

/*  Modified 12/23/83 to upcall a "done" function when the transfer is
    complete.  Modified 1/11/84 to detect and discard duplicate requests
    to open a tftp connection.  Fixed file open calls to use only binary mode
    (was making netascii transfers fail); fixed bug in counting number of 
    tftp connections in progress; changed calls to send error packets to
    use incoming packet and socket number where appropriate, 1/13/84.
    Added test to discard duplicate requests that match current connections,
    that arrived before user refused a request or before discovery that
    file can't be transferred.
					  <J. H. Saltzer>
    1/24/84 - added octet mode which is the same as image mode.
					 <John Romkey>
    3/21/84 - corrected several places in the code where cn->tf_expected
	was referenced as cn->tf_expect and caused problems with the new
	compiler.
					 <John Romkey>
    4/12/84 - in tfsread() fixed call to udp_alloc() which was missing
	the option length parameter.
					<John Romkey>
    12/21/84 - in tfsread() fixed buffer swapping code to prevent loss
	of one packet buffer on each transfer.
					<J. H. Saltzer>
*/

#define	OFF	0
#define	ON	1

#define	MAXTFTPS	1

UDPCONN tftp;

int tfshnd();
int (*tfs_alert)();
int (*tfs_done)();
int ntftps = 0;
int tfstate = OFF;
long refusedt = 0;		/*  time of most recent transfer refusal  */

/* tfsinit(alert, done) - initialize the tftp server. This opens a UDP
	 connectionbut does not turn on the server. That needs to done by
	 an explicit call to tfs_on().
	alert() is a function which the server will call whenever it receives
	request for a transfer. This function will be called int he following
	way:
		alert(ip_addr, file_name, direction)

	alert() should return TRUE if it wishes to allow the transfer and
	FALSE otherwise.  done() is a function that the server will
	call to	inform the invoker that this file transfer is complete or
	aborted.
*/

tfsinit(alert, done)
	int (*alert)();
	int (*done)(); {

	tftp = udp_open(0L, 0, TFTPSOCK, tfshnd, 0);
	if(tftp == NULL) {
		printf("tftp: couldn't open udp connection\n");
		exit();
		}

	tfs_done = done;
	tfs_alert = alert;
	}

/* turn the tftp server on
*/

tfs_on() {
	tfstate = ON;
	}

/* turn the tftp server off
*/

tfs_off() {
	tfstate = OFF;
	}

/* handle an incoming tftp packet. This involves opening a udp connection
	(immediately so that we can report errors). If the server is OFF
	then the tftp will be refused; otherwise more checking will be done.
	Call the alert function and verify that the "user" wishes to allow
	the tftp. Report an error if not. Finally, spawn a task to oversee
	the tftp and cleanup when it's done.
*/

int tfsread(), tfswrit();
static unsigned numtftps = 1;

tfshnd(p, len, host)
	PACKET p;
	unsigned len;
	in_name host; {
	unsigned nport;
	struct tfreq *ptreq;
	register struct tfconn *cn;
	char *file, *smode, *tmp;
	unsigned mode;
	struct udp *pup = udp_head(in_head(p));
	UDPCONN tmpudp;

	/* If there is already a connection like this one, ignore duplicate 
	request. */

	if(udp_ckcon(host, pup->ud_srcp)) { 
		udp_free(p);
		return;
		}

	/*  If we refused a connection since this request got enqueued,
	assume this is a duplicate and discard it, so we don't bother the
	user with a duplicate question.  (If we are unlucky, this might be
	a request from somewhere else that arrived while the user was
	thinking over the previous request.  Tough; somewhere else will
	just have to try again.)  */

	if (refusedt > p->nb_tstamp) {
		udp_free(p);
		return;
		}

	/*  The next question:  Do we have room to do this transfer?  */

	if(ntftps >= MAXTFTPS) {
		tfrpyerr(tftp, p, ERRTXT, "Too many connections.");
		udp_free(p);
		return;
		}
	ntftps++;

	/*  OK, let's check over the request more carefully. */

	ptreq = (struct tfreq *)tftp_head(p);
	ptreq->tf_op = bswap(ptreq->tf_op);
	if(ptreq->tf_op > WRQ) {
#ifdef DEBUG
		if(NDEBUG & INFOMSG|NETERR|PROTERR)
		    printf("TFTP SERVER: bad tftp opcode %u\n", ptreq->tf_op);
#endif
		udp_free(p);
		ntftps--;
		return;
		}
	file = ptreq->tf_name;


	smode = file+strlen(file)+1;

	for(tmp = smode; *tmp; tmp++)
		if(*tmp >= 'A' && *tmp <= 'Z') *tmp += 32;

	if(strcmp(smode, "image") == 0) mode = IMAGE;
	else if(strcmp(smode, "octet") == 0) mode = IMAGE;
	else if(strcmp(smode, "netascii") == 0) mode = ASCII;
	else {
#ifdef DEBUG
		if(NDEBUG & INFOMSG|NETERR|PROTERR)
			printf("TFTP SERVER:  Bad mode %s in req\n", smode);
#endif
		tfrpyerr(tftp, p, ERRTXT, "Bad mode");
		udp_free(p);
		ntftps--;
		return;
		}

	if(tfstate == OFF) {
		tfrpyerr(tftp, p, ERRTXT,"Transfers are not being accepted.");
		udp_free(p);
		ntftps--;
		return;
		}


	if((*tfs_alert)(host, file, ptreq->tf_op == RRQ ? PUT : GET) == 0) {
		tfrpyerr(tftp, p, ERRTXT, "Transfer refused.");
		refusedt = cticks;
		udp_free(p);
		ntftps--;
		return;
		}

	/*  It looks safe to try to open a connection.  */

	cn = tfmkcn(PUT, ASCII);	/*  Direction is a dummy for now  */
	if(cn == NULL) {
	    printf("TFTP SERVER:  Can't make connection, ignoring request\n");
		udp_free(p);
		ntftps--;
		return;
		}

	cn->tf_udp = udp_open(host, pup->ud_srcp,
				udp_socket(), tftprcv, cn);
	if(cn->tf_udp == 0) {
	printf("TFTP SERVER: Can't open udp connection, ignoring request\n");
		tfcleanup(cn);
		udp_free(p);
		ntftps--;
		return;
		}

	cn->tf_mode = mode;
	if(ptreq->tf_op == RRQ) {
		cn->tf_dir = PUT;
		cn->tf_fport = 1;
		cn->tf_fd = fopen(file, "rb");
		if(cn->tf_fd == NULL) {
#ifdef DEBUG
			if(NDEBUG & NETERR|PROTERR)
				printf("couldn't open file\n");
#endif
			tfudperr(cn->tf_udp, p, FNOTFOUND, " ");
			refusedt = cticks;
			tfcleanup(cn);
			(*tfs_done)(OFF);
			udp_free(p);	
			ntftps--;
			return;
			}

		cn->tf_task = tk_fork(tk_cur, tfsread, 1200, "tfrd", cn);
		}
	else {
		cn->tf_dir = GET;
		cn->tf_fport = 1;
		cn->tf_fd = fopen(file, "wb");
		if(cn->tf_fd == NULL) {
			printf("couldn't open file\n");
			tfudperr(cn->tf_udp, p, ACCESS, " ");
			refusedt = cticks;
			tfcleanup(cn);
			(*tfs_done)(OFF);
			udp_free(p);	
			ntftps--;
			return;
			}

		cn->tf_task = tk_fork(tk_cur, tfswrit, 1200, "tfwr", cn);
		}

	udp_free(p);
	}

tfswrit(cn)
	register struct tfconn *cn; {

	cn->tf_expected = 1;
	tfsndack(cn, 0);
/*	tm_set(cn->tf_rt, tftptmo, cn, cn->tf_tm);	*/

	while(1) {
		cn->tf_state = DATAWAIT;
		while(cn->tf_state == DATAWAIT) tk_block();

		if(cn->tf_state == TIMEOUT) {
#ifdef DEBUG
			if(NDEBUG & TMO)
			   printf("TFTP: Host not responding, giving up\n");
#endif
			tfudperr(cn->tf_udp,cn->tf_outp, ERRTXT,
				 "Retry limit exceeded, giving up");
			tfcleanup(cn);
			(*tfs_done)(OFF);
			break;
			}

		if(cn->tf_state == RCVLASTDATA) {
			tfcleanup(cn);
			(*tfs_done)(ON);
			break;
			}

		if(cn->tf_state != RCVDATA) {
			tfcleanup(cn);
			(*tfs_done)(OFF);
			break;
			}
		}

	ntftps--;
	tk_exit();
	}

tfsread(cn)
	register struct tfconn *cn; {
	int flen, done;
	PACKET pfill, psnd;
	int temp;
	unsigned CRSEEN = 0;
	unsigned HOLDING = 0;
	char *data;
	char HELD;

	flen = NORMLEN;
	done = 0;
	cn->tf_expected = 0;
	psnd = cn->tf_outp;		/* psnd is tftp_util's buffer */
	pfill = udp_alloc(NORMLEN, 0);  /* pfill is ours */
	if(pfill == 0) {
		tfudperr(cn->tf_udp, pfill,
				 ERRTXT, "Couldn't allocate packet.");
#ifdef DEBUG
		if(NDEBUG & NETERR|PROTERR)
			printf("tfsread: couldn't allocate psnd\n");
#endif
		tfcleanup(cn);
		(*tfs_done)(OFF);
		tk_exit();
		}

	/* Here's how the main loop for putting data blocks works:
		We make the request packet and then we enter the main
		loop.

		SEND current pkt	<-----------------------+
		If we haven't filled the data pkt, fill it	|
		Wait for the ACK				|
		If timeout, ------------------------------------+
		Swap the pkt buffers				|
		GOTO -------------------------------------------+
	*/

	while(1) {
		cn->tf_state = ACKWAIT;
		data = (char *)tftp_data(pfill);
		for(flen=0; flen<NORMLEN; flen++) {
			if(HOLDING) {
				HOLDING = 0;
				data[flen] = HELD;
				continue;
				}
			temp = getc(cn->tf_fd);
			if(CRSEEN) {
				CRSEEN = 0;
				if(temp == 0x0a) {
					data[flen] = temp;
					continue;
					}
				else {
					HOLDING = 1;
					HELD = temp;
					data[flen] = 0;
					continue;
					}
				}

			if(cn->tf_mode == ASCII && temp == 0x0d)
				CRSEEN = 1;
			if(temp == EOF) break;
			data[flen] = temp;
			if(cn->tf_mode == ASCII && temp == dos_eof)
				break;
			}

		cn->tf_size += flen;
		if(!cn->tf_expected) cn->tf_state = RCVACK;

		while(cn->tf_state == ACKWAIT) tk_block();

		if(cn->tf_state == TIMEOUT) {
#ifdef DEBUG
			if(NDEBUG & TMO)
			   printf("TFTP: Host not responding, giving up\n");
#endif
			tfudperr(cn->tf_udp,cn->tf_outp,
				ERRTXT, "Retry limit exceeded, giving up");
			cn->tf_state = DEAD;
			tfcleanup(cn);
			(*tfs_done)(OFF);
			udp_free(pfill);
			break;
			}

		if(cn->tf_state == RCVACK) {
			cn->tf_expected++;

			/* Check if we're done */
			if(done) {
				tfcleanup(cn);
				(*tfs_done)(ON);
				udp_free(pfill);
				break;
				}

			/* If not, resume the loop */
			psnd = pfill;		/* prepare to switch */
			pfill = cn->tf_outp;	/* take tftp_util's buffer */
			cn->tf_outp = psnd;	/* our buffer to tftp_util */
			tfsndata(cn, flen);
			if(flen < NORMLEN) done = 1;
			continue;
			}

		tfcleanup(cn);
		(*tfs_done)(ON);
		udp_free(pfill);
		break;
		}

	ntftps--;
	tk_exit();
	}
