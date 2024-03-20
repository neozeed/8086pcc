/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

/* This file contains the routine that implements an Internet Time
 * service user.  The Internet addresses of one or more default time
 * servers are found in a customization structure.
 * Calling udptime (0,n) sends requests in parallel to the first n
 * time servers to try to get the time; udptime(x,1) sends a request
 * to host x only.
*/
typedef long time_t;

#include	<udp.h>
#include	<timer.h>

#define	TIMEOUT		2		/* response timeout */
#define	TIMESOCK	37		/* time server socket */


static task *timetask;
static int time_rcv(), time_wake();
static long rcvdtime;

/* Try to find the current time by asking other udp timeservers.
 * Build timeserver requests for each of the timeservers we presently
 * know about, send them all out, then wait for some response.  Returns
 * the time in seconds since midnight Jan 1 1900 GMT, or 0 if unknown.
 */

time_t	udptime (arg, maxpoll)
	int		maxpoll;	/* limit on servers to ask */
	in_name		arg;	 {
	in_name		pn;		/* pointer to current ts */
	unsigned	locsoc;		/* local socket number */
	PACKET		p;		/* output packet */
	UDPCONN		tmnd[MAXTIMES];	/* udp file descriptors */
	int		i;		/* temp index */
	timer		*tm;

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Entering TIMEUSER.\n");
#endif

	locsoc = udp_socket ();

	if((p = udp_alloc(0, 0)) == NULL) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG || NDEBUG & PROTERR)
			printf("TIMEUSER: Couldn't allocate packet!\n");
#endif

		return 0L;
		}

	timetask = tk_cur;		

	if (arg != 0) {
		custom.c_numtime = 1;
		custom.c_time[0] = arg;
		}

	for(i=0; i<custom.c_numtime & i<maxpoll ; i++) {
#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("udp_time: pkt # %u.\n", i);
#endif

		tmnd[i] = udp_open(custom.c_time[i],TIMESOCK,locsoc,time_rcv);
		if(tmnd[i] == 0) {
#ifdef	DEBUG
			if(NDEBUG & (PROTERR|INFOMSG))
				printf("TIMEUSER: Couldn't open UDP conn.\n");
#endif
			if(i==0) return 0L;
			}

#ifdef	DEBUG
		if(NDEBUG & INFOMSG)
			printf("TIMEUSER: req to %a\n", custom.c_time[i]);
#endif

		udp_write(tmnd[i], p, 0);
		}

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("Done sending packets.\n");
#endif

	udp_free(p);
	tm = tm_alloc();
	tm_set(TIMEOUT, time_wake, 0, tm);

	/* Block, waiting for one of two events: (1) receipt of a response
		or (2) the timer to fire. */

	tk_block();

	tm_clear(tm);
	tm_free(tm);

	/* Clean up the UDP connections */
/*	for(i=0; i<custom.c_numtime; i++) udp_close(tmnd[i]);	*/

	return rcvdtime;	}


static time_rcv(p, len, host)
	PACKET p;
	unsigned len;
	in_name host; {
	time_t *ptm;

	ptm = (time_t *)udp_data(udp_head(in_head(p)));
#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("TIMEUSER: response from %a\n", host);
#endif

	rcvdtime = lswap(*ptm) ;

	udp_free (p);
	time_wake();
	}


static time_wake() {
	tk_wake(timetask);
	}
