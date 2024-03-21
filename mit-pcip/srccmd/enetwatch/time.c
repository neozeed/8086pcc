/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <q.h>
#include <task.h>
#include <timer.h>
#include <stdio.h>
#include "attrib.h"

/* routines to print the time and the date. */

#define	GETTIME	0x2c
#define	GETDATE	0x2a

long get_dosl();

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

tminit() {

	tm = tm_alloc();
	if(tm == 0) {
		printf("TN_TMINIT: Couldn't allocate a timer.\n");
		return;
		}

	blanks[49] = 0;
	tntime = tk_fork(tk_cur, tn_time, 1000, "TnTime");
	tm_set(1, tmwake, 0, tm);
	}

tmwake() {
	tk_wake(tntime);
	}

tn_time() {
	union {
		long l;
		char c[4];
		} time;
	char s[40];
	unsigned secs;


	while(1) {
		time.l = get_dosl(GETTIME);
		sprintf(s, "%02u:%02u:%02u", time.c[3], time.c[2], time.c[1]);

		write_string(s, 24, 71, att25);
		if(clear25 && (--clear25 == 0))
			clr25();

		tm_clear(tm);
		tm_set(1, tmwake, 0, tm);
		tk_block();
		}
	}

clr25() {
	write_string(blanks, 24, 0, att25);
	}
