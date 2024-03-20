/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/*  Modified 12/17/83 to include display of tracing information at clock
    update time.  <J. H. Saltzer>
	2/3/84 - added support for turning off telnet usage of the 25th
		line. <John Romkey>
*/

#include <net.h>
#include <timer.h>
#include <stdio.h>
#include "telnet.h"
#include <attrib.h>

/* routines to print the time and the date. */

#define	GETTIME	0x2c
#define	GETDATE	0x2a

long get_dosl();
extern char tnshost[];

p_time(time)
	long	time; {
	
	printf("%02u:%02u:%02u",(int)(time >> 24),
			(int)(time >> 16) & 0xff,
			(int)(time >> 8) & 0xff);
	}

char	*month[] = { "FOO","Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
	"Sep","Oct","Nov","Dec" };

p_date(date)
	long	date; {
	
	printf("%d-%s-%d", (int)(date & 0xff),
			month[(int)(date >> 8) & 0xff],
			(int)(date >> 16));
	}


/* this routine updates the "real-time" on the 25th line. */
task *tntime;
int tmwake(), tn_time();
static timer *tm;
static char blanks[80] =
"                                                                            ";

extern unsigned clear25;	/* how many seconds to clear err msg in */
extern unsigned att25;		/* 25th line attribute byte */

tn_tminit() {

	tm = tm_alloc();
	if(tm == 0) {
		printf("TN_TMINIT: Couldn't allocate a timer.\n");
		return;
		}

	blanks[70-strlen(tnshost)] = 0;
	tntime = tk_fork(tk_cur, tn_time, 1000, "TnTime");
	tm_tset(TPS, tmwake, 0, tm);
	}

tmwake() {
	tk_wake(tntime);
	}

tn_time() {
	union {
		long l;
		char c[4];
		} time;
	char s1[60];
	char s[40];
	unsigned secs;


	while(1) {
		if(ucb.u_mode) {
			time.l = get_dosl(GETTIME);
			sprintf(s, "%02u:%02u:%02u", time.c[3], time.c[2], time.c[1]);
			if(clear25 && (--clear25 == 0))
				clr25();
			if(NDEBUG & APTRACE){
				tcp_state(s1);
				write_string(s1, 24, 0, att25);
				}
			write_string(s, 24, 71, att25);
			write_string(tnshost, 24, 70-strlen(tnshost), att25);
			}
		tm_clear(tm);
		tm_tset(TPS, tmwake, 0, tm);
		tk_block();
		}
	}

clr25() {
	write_string(blanks, 24, 0, att25);
	}

extern long cticks;
extern unsigned _tcount;

tntmdump() {
	printf("function address is %04x\n", tn_time);
	printf("Timer address is %04x\n", tm);
	printf("Timer dump:\n");
	printf("time: %04x%04x\n", (unsigned)(tm->tm_time>>16),
						(unsigned)tm->tm_time);
	printf("nonce: %u\n", tm->tm_nonce);
	printf("timer function: %04x\n", tm->tm_subr);
	printf("timer elt next : %04x\n", tm->tm_elt.qe_next);
	printf("cticks = %04x%04x, _tcount = %u\n", cticks, _tcount);
	tm_dump();
	}
