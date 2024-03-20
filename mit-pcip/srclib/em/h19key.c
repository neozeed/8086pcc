/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <em.h>
#include <h19.h>

int	ba_bs = FALSE;		/* flag for mapping of the backarrow key */

h19key() {
	int chr;

	chr = get_kbd();

	if(chr == NONE) return chr;

	if(chr == '\r') chr = '\n';
	else if(chr == '\n') chr = '\r';

	return chr;
	}

#define	CNTRL_BREAK	0	/* the scan code sent for cntrl-break */
#define BK_ARROW	14	/* the scan codes for various keys */
#define UP_ARROW	72
#define DOWN_ARROW	80
#define	RIGHT_ARROW	77
#define	LEFT_ARROW	75
#define	DEL_KEY		83
#define	PG_UP		73
#define	PG_DN		81
#define	END		79
#define	HOME		71
#define	CENTER		76
#define	SC_F1		59
#define	SC_F2		60
#define	SC_F3		61
#define	SC_F4		62
#define	SC_F5		63
#define	SC_F6		64
#define	SC_F7		65
#define	SC_F8		66
#define	SC_F9		67
#define	SC_F10		68

get_kbd()
{
  int	data;	/* data from keyboard: high byte is scan code,
					low byte is ascii */
  char	odata;
  char	scan_code;
  if (q_cnt())
	return((int)get_q());

  if (kbd_stat()) {
	data = kbd_in();
	scan_code = data >> 8;
	odata = (char)data;
	if (odata == 0){
		switch(scan_code){
		case DEL_KEY:
			return (int)DELETE;
			break;
		case CNTRL_BREAK:
			return (int)C_BREAK;
			break;
		case UP_ARROW:
			h19_put_q('A');
			return (int)ESC;
			break;
		case DOWN_ARROW:
			h19_put_q('B');
			return (int)ESC;
			break;
		case RIGHT_ARROW:
			h19_put_q('C');
			return (int)ESC;
			break;
		case LEFT_ARROW:
			h19_put_q('D');
			return (int)ESC;
			break;
		case HOME:
			h19_put_q('E');
			return (int)ESC;
			break;
		case END:
			h19_put_q('F');
			return (int)ESC;
			break;
		case PG_UP:
			h19_put_q('G');
			return (int)ESC;
			break;
		case PG_DN:
			h19_put_q('H');
			return (int)ESC;
			break;
		case CENTER:
			h19_put_q('I');
			return (int)ESC;
			break;
		case SC_F1:
			h19_put_q('S');
			return (int)ESC;
			break;
		case SC_F2:
			h19_put_q('T');
			return (int)ESC;
			break;
		case SC_F3:
			h19_put_q('U');
			return (int)ESC;
			break;
		case SC_F4:
			h19_put_q('V');
			return (int)ESC;
			break;
		case SC_F5:
			h19_put_q('W');
			return (int)ESC;
			break;
		case SC_F6:
			h19_put_q('P');
			return (int)ESC;
			break;
		case SC_F7:
			h19_put_q('Q');
			return (int)ESC;
			break;
		case SC_F8:
			h19_put_q('R');
			return (int)ESC;
			break;
		case SC_F9:
			return (int)F9;
		case SC_F10:
			return (int)F10;
		}
	}
	if (ba_bs == FALSE) {
		if ((odata == B_SPACE) && (scan_code == BK_ARROW))
			odata = DELETE;
		else if (odata == DELETE)
			odata = B_SPACE;
	}
	if (alt_key())
		return (int)data | 0x80;
	else
		return (int)odata;
  }
  return (int)NONE;
}

_key_rdy()
{
	if (q_cnt())
		return TRUE;
	if (kbd_stat())
		return TRUE;
	return FALSE;
}


/* definitions for the queue */
#define	max_q	6
static	char	queue[max_q];
static	int	q_head = 0;
static	int	q_tail = 0;

h19_put_q(data)
char	data;
{
	queue[q_head++] = data;
	if (q_head >= max_q)
		q_head = 0;
}

static get_q()
{
	char	data;

	data = queue[q_tail];
	if (++q_tail >= max_q)
		q_tail = 0;
	return data;
}

static q_cnt()
{
	return q_head - q_tail;
}
