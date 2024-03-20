/* This file contains those routines which interpret the SUPDUP %TD codes
 * and do the right thing to the screen.  The code was taken from an H-19
 * emulator and if there is something which seems to have no purpose it
 * is probably due to history.
 */

/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include	"supdup.h"

/*  Supdup terminal driver.
    1/84 - Written.
    						<David Bridgham>
    9/84 - Changed to use improved screen package that used a linear
	screen position instead of (x,y) thereby eliminating a multiply per
	character.
*/

/* this is only here so that it forces _wbyte to get dragged in
*/
int _wbyte();
static int (*writer)() = _wbyte;

	/* these get swapped to change the sense of the screen */
char	NORM_VIDEO = 0x07;	/* normal video */
char	REV_VIDEO = 0x70;	/* reverse video */

int	x_pos = 0,y_pos = 0;		/* cursor position */
unsigned	pos = 0;		/* cursor position =
					 *  x_pos + 80*y_pos. */
static	int	x_save = 0,y_save = 0;	/* saved cursor position */
static	int	insert_char = FALSE;	/* flag for insert character mode */
int	wrap_around = FALSE;	/* flag for wrap around mode */
int	ba_bs = FALSE;		/* flag for mapping of the backarrow key */
int	arg1;			/* for holding arguments */

char	attrib = 0x07;		/* holds the current attribute byte */
static char	state = ST_NORM;	/* holds the current state */

supem(data)	/* SUPDUP Terminal Emulator */
char	data;
{
    int		line_buf[80];
    int		count;

    switch (state) {
	case ST_NORM:	norm(data); break;
	case ST_MOV1:	state = ST_MOV2; break;
	case ST_MOV2:	state = ST_MV01; break;
	case ST_MV01:
		y_save = (data < 24 ? data : 23);
		state = ST_MV02;
		break;
	case ST_MV02:
		y_pos = y_save;
		x_pos = (data < 80 ? data : 79);
		pos = x_pos + 80 * y_pos;
		set_cursor();
		state = ST_NORM;
		break;
	case ST_QUOTE:	state = ST_NORM; break;	/* just ignore */
	case ST_IL:	/* Insert Lines */
		state = ST_NORM;
		if (data > 24) data = 24;
		move_lines(24 - data, 24, 25 - data - y_pos);
		clear_lines(y_pos, data);
		break;
	case ST_DL:	/* Delete Lines */
		state = ST_NORM;
		if (data > 24) data = 24;
		move_lines(y_pos + data, y_pos, 24 - y_pos);
		clear_lines(24 - data, data);
		break;
	case ST_IC:	/* Insert Characters */
		state = ST_NORM;
		if (data > 80) data = 80;
		read_line(line_buf, y_pos);
		for (count = 79; count >= x_pos + data; count--)
			line_buf[count] = line_buf[count - data];
		for (count = x_pos; count < x_pos + data; count++)
			line_buf[count] = SPACE | (attrib << 8);
		write_line(line_buf, y_pos);
		break;
	case ST_DC:	/* Delete Characters */
		state = ST_NORM;
		if (data > 80) data = 80;
		read_line(line_buf, y_pos);
		for (count = x_pos + data; count < 80; count++)
			line_buf[count - data] = line_buf[count];
		for (count = 80 - data; count < 80; count++)
			line_buf[count] = SPACE | (attrib << 8);
		write_line(line_buf, y_pos);
		break;
	case ST_RSU1:		/* Get first arg for Region Scroll Up */
		arg1 = (data <= 24 ? data : 24);
		state = ST_RSU2;
		break;
	case ST_RSU2:		/* Region Scroll Up */
		state = ST_NORM;
		if (data > 24) data = 24;
		move_lines(y_pos + data, y_pos, arg1 - data);
		clear_lines(y_pos + arg1 - data, data);
		break;
	case ST_RSD1:		/* Get first arg for Region Scroll Down */
		arg1 = (data <= 24 ? data : 24);
		state = ST_RSD2;
		break;
	case ST_RSD2:		/* Region Scroll Down */
		state = ST_NORM;
		if (data > 24) data = 24;
		move_lines(y_pos + arg1 - data, y_pos + arg1, arg1 - data);
		clear_lines(y_pos, data);
		break;
    }
}

norm(data)
unsigned char	data;
{	
    int		line_buf[80];
    int		new_x;
    int		count;
    
    data &= 0377;
    if (data < '\200') {
	switch (data) {
	case TAB:
		new_x = (x_pos + 8) & ~7;
		if (new_x > 79)
			new_x = 79;
		pos += new_x - x_pos;
		x_pos = new_x;
		set_cursor();
		break;
	case '\0':	/* ignore nulls. This isn't exactly supdup spec but
			 * we shouldn't ever be sent '\0' anyway. */
		break;
	default:
		nwrite_char(data | (attrib << 8), pos);

		if (x_pos < 79) {
			x_pos++;
			pos++;
			set_cursor();
		}
		break;
	}
    }
    else {
	/* This may look somewhat odd.  The reason that I did not just put
	 * the %TD codes in the obvious switch statement but rather cleared
	 * the high bit and then did this switch is because of a compiler
	 * bug.  The compiler I was using produced bad code with an unsigned
	 * char and case statements with values that had the 0200 bit set.
	 * Sigh.
	 */
	data &= 0177;
	switch (data) {
	case '\000':			/* %TDMOV */
		state = ST_MOV1;
		break;
	case '\002':			/* %TDEOF */
		/* first erase to end of line */
		read_line(line_buf,y_pos);
		for (count = x_pos; count <= 79; count++)
			line_buf[count] = SPACE | (attrib << 8);
		write_line(line_buf,y_pos);
		/* now erase rest of screen */
		if ((23-y_pos) > 0)
			clear_lines(y_pos+1,23-y_pos);
		break;
	case '\003':			/* %TDEOL */
		read_line(line_buf,y_pos);
		for (count = x_pos; count <= 79; count++)
			line_buf[count] = SPACE | (attrib << 8);
		write_line(line_buf,y_pos);
		break;
	case '\004':			/* %TDDLF */
		nwrite_char(SPACE | (attrib << 8), pos);
		break;
	case '\007':			/* %TDCRL */
		pos = pos - x_pos;
		x_pos = 0;
		if (y_pos < 23)	{ /* if cursor not at bottom */
			y_pos++;	/* move cursor down 1 line */
			pos += 80;
		}
		else		/* else cursor is at bottom */
			scrollup();	/* scroll screen up */
		set_cursor();
		/* now clear current line */
		read_line(line_buf,y_pos);
		for (count = x_pos; count <= 79; count++)
			line_buf[count] = SPACE | (attrib << 8);
		write_line(line_buf,y_pos);
		break;
	case '\010':			/* %TDNOP */
		break;
	case '\014':			/* %TDORS */
		/* need to send back the current cursor location */
		break;
	case '\015':			/* %TDQOT */
		state = ST_QUOTE;
		break;
	case '\016':			/* %TDFS */
		if (x_pos < 79) {
			x_pos++;
			pos++;
			set_cursor();
		}
		break;
	case '\017':			/* %TDMV0 */
		state = ST_MV01;
		break;
	case '\020':			/* %TDCLR */
		x_pos = 0;
		y_pos = 0;
		pos = 0;
		set_cursor();
		clear_lines(y_pos,24);
		break;
	case '\021':			/* %TDBEL */
		ring();
		break;
	case '\022':			/* %TDINI */
		break;		/* should never get this */
	case '\023':			/* %TDILP */
		state = ST_IL;
		break;
	case '\024':			/* %TDDLP */
		state = ST_DL;
		break;
	case '\025':			/* %TDICP */
		state = ST_IC;
		break;
	case '\026':			/* %TDDCP */
		state = ST_DC;
		break;
	case '\027':			/* %TDBOW */
		attrib = REV_VIDEO;
  		break;
	case '\030':			/* %TDRST */
		attrib = NORM_VIDEO;
  		break;
	case '\032':			/* %TDRSU */
		state = ST_RSU1;
		break;
	case '\033':			/* %TDRSD */
		state = ST_RSD1;
		break;
	}
    }
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
			put_q('A');
			return (int)ESC;
			break;
		case DOWN_ARROW:
			put_q('B');
			return (int)ESC;
			break;
		case RIGHT_ARROW:
			put_q('C');
			return (int)ESC;
			break;
		case LEFT_ARROW:
			put_q('D');
			return (int)ESC;
			break;
		case HOME:
			put_q('E');
			return (int)ESC;
			break;
		case END:
			put_q('F');
			return (int)ESC;
			break;
		case PG_UP:
			put_q('G');
			return (int)ESC;
			break;
		case PG_DN:
			put_q('H');
			return (int)ESC;
			break;
		case CENTER:
			put_q('I');
			return (int)ESC;
			break;
		case SC_F1:
			put_q('S');
			return (int)ESC;
			break;
		case SC_F2:
			put_q('T');
			return (int)ESC;
			break;
		case SC_F3:
			put_q('U');
			return (int)ESC;
			break;
		case SC_F4:
			put_q('V');
			return (int)ESC;
			break;
		case SC_F5:
			put_q('W');
			return (int)ESC;
			break;
		case SC_F6:
			put_q('P');
			return (int)ESC;
			break;
		case SC_F7:
			put_q('Q');
			return (int)ESC;
			break;
		case SC_F8:
			put_q('R');
			return (int)ESC;
			break;
		case SC_F9:
			return (int)F9;
		case SC_F10:
			return (int)F10;
		}
	}
	if (data == '\034') {
		put_q('\034');
		return (int)'\034';
	}
	if (ba_bs == FALSE) {
		if ((odata == B_SPACE) && (scan_code == BK_ARROW))
			odata = DELETE;
		else if (odata == DELETE)
			odata = B_SPACE;
	}
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

static put_q(data)
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

/* write_string(string,y,x,attrib) - writes the passed string to the x and y
 *		locations with the passed attribute.
 */

write_string(string,y,x,attrib)
char	*string;
int	x,y;
char	attrib;
{
  int	pos;
  pos = x + 80 * y;
  while(*string != '\0')
	nwrite_char(*string++ | (attrib << 8),pos++);
}
