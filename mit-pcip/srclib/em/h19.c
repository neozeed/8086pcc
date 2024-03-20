/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include	<h19.h>

/*  h19 terminal emulator
    written 1982 by David Bridgham

    4/84 - changed set_cursor to use linear position instead of (x,y)
	coordinates to eliminate multiply.
						<David Bridgham>
    6/84 - redesigned emulator FSM. Copied set_cursor in write_char.
						<John Romkey>
    8/31/84 - split out some of the h19key functions so that using h19key()
	wouldn't drag in the whole emulator.
						<John Romkey>
    9/11/84 - made changes suggested by Mark Colan to fix inverse video
	on clear to end of line problem.
						<John Romkey>
    10/2/84 - added a reference to _wbyte to make em work with the new
	I/O library.
						<John Romkey>
*/

/* State definitions for the screen */
#define	ST_NORM		0
#define	ST_ESC		1
#define	ST_ESC_Y	2
#define	ST_HAV_y	3
#define	ST_ANSI_BRACK	4
#define	ST_ANSI_QMARK	5
#define	ST_THROW_OUT	6

/* Mode definitions */
#define	MD_HEATH	1
#define	MD_ANSI		2

#define	TRUE		1
#define	FALSE		0

/* this is only here so that it forces _wbyte to get dragged in
*/
int _wbyte();
static int (*writer)() = _wbyte;

/* these get swapped to change the sense of the screen */
char	NORM_VIDEO = 0x07;	/* normal video */
char	REV_VIDEO = 0x70;	/* reverse video */

int	x_pos = 0,y_pos = 0;		/* cursor position */
unsigned pos = 0;		/* cursor position = x_pos + 80*y_pos */
static	int	x_save = 0,y_save = 0;	/* saved cursor position */
static	int	insert_char = FALSE;	/* flag for insert character mode */
int	wrap_around = FALSE;	/* flag for wrap around mode */
extern int ba_bs;
int	arg1,arg2;		/* arguments for ANSI mode */

char	attrib = 0x07;		/* holds the current attribute byte */
char	mode = MD_HEATH;	/* hold the current mode */

/* The new FSM for the emulator uses the variable em_routine as a
   pointer to the function which should dispose of the next character.
   Normally this is the norm() function, which just handles a plain
   character. Escape sequences are handled by the rest of the functions.
   The second character of an escape sequence is handled by esc() or
   ansi_esc(), depending on whether the emulator is in ANSI mode or not.
   esc_y(), esc_hav_y(), ansi_brack() and ansi_qmark() are all auxiliary
   routines.
*/

static int norm(), esc(), ansi_esc(), esc_y(), esc_hav_y(), dummy();
static int ansi_brack(), ansi_qmark();

int	(*em_routine)() = norm;	/* emulator routine for h19 FSM */

static norm(data)
char	data;
{
  int	count;		/* random integer */
  int	line_buf[80];	/* buffer for lines */

	switch (data) {
	case ESC:
		em_routine = mode == MD_HEATH ? esc : ansi_esc;
		break;
	case CR:
		pos -= x_pos;
		x_pos = 0;
		set_cursor();
		break;
	case LF:
		if (y_pos < 23)	{	/* if cursor not at bottom */
			y_pos++;	/* move cursor down 1 line */
			pos += 80;
		}
		else {	/* else cursor is at bottom */
			int save_attrib;

			save_attrib = attrib;
			attrib = NORM_VIDEO;

			scrollup();

			attrib = save_attrib;
			}
		set_cursor();
		break;
	case B_SPACE:
		if (x_pos > 0) {	/* if not at left edge */
			x_pos--;
			pos--;
			set_cursor();
		}
		break;
	case TAB:
		/* if it is at the last tab then only go one space */
		if (x_pos > 72)	{
			x_pos++;
			pos++;
			}
		else {
			int ox_pos = x_pos;

			x_pos = (x_pos + 8) & ~7;
			pos += x_pos - ox_pos;
			}
		if (x_pos > 79) {
			x_pos = 79;
			pos = 80 * y_pos + x_pos;
			}
		set_cursor();
		break;
	case BELL:
		ring();
		break;
	case DELETE:
		break;		/* do nothing */
	default:	/* send characters to screen */
		if (data < ' ') break;
		if (insert_char) {
			read_line(line_buf,y_pos);
			for (count = 79; count > x_pos; --count)
				line_buf[count] = line_buf[count - 1];
			write_line(line_buf,y_pos);
		}

		write_char(data | (attrib << 8), pos);
		x_pos++;
		pos++;

		if (x_pos > 79) {
			if (wrap_around) {
				x_pos = 0;
				if (y_pos < 23)
					y_pos++;
				else
					scrollup();
				pos = y_pos * 80;
				}
			else {
				x_pos = 79;
				pos--;
				}
			set_cursor();
			}
		break;
		}
	}

static esc(data)
char	data;
{
  int	count;
  int	line_buf[80];

  em_routine = norm;

  switch (data) {
  case 'A':			/* move cursor up */
	if (y_pos > 0) {	/* if not at top */
		y_pos--;
		pos -= 80;
		set_cursor();
	}
  	break;
  case 'B':		/* move cursor down */
	if (y_pos < 23) {	/* if not at bottom */
		y_pos++;
		pos += 80;
		set_cursor();
	}
  	break;
  case 'D':		/* move cursor left */
	if (x_pos > 0) {	/* if not at left edge */
		x_pos--;
		pos--;
		set_cursor();
	}
  	break;
  case 'C':		/* move cursor right */
	if (x_pos < 79) {	/* if not at right edge */
		x_pos++;
		pos++;
		set_cursor();
	}
  	break;
  case 'K':		 /* clear to end of line */
	{
	int save_attrib;

	save_attrib = attrib;
	attrib = NORM_VIDEO;

	read_line(line_buf,y_pos);
	for (count = x_pos; count <= 79; count++)
		line_buf[count] = SPACE | (NORM_VIDEO << 8);
	write_line(line_buf,y_pos);

	attrib = save_attrib;
	}
  	break;
  case 'J':		/* erase to end of screen */
	{
	int save_attrib;

	save_attrib = attrib;
	attrib = NORM_VIDEO;

	/* first erase to end of line */
	read_line(line_buf,y_pos);
	for (count = x_pos; count <= 79; count++)
		line_buf[count] = SPACE | (NORM_VIDEO << 8);
	write_line(line_buf,y_pos);
	/* now erase rest of screen */
	if ((23-y_pos) > 0)
		clear_lines(y_pos+1,23-y_pos);

	attrib = save_attrib;
	}
  	break;
  case 'H':		/* home */
	x_pos = y_pos = pos = 0;
	set_cursor();
  	break;
  case 'E':		/* erase screen */
	attrib = NORM_VIDEO;
	x_pos = y_pos = pos = 0;
	set_cursor();
	clear_lines(y_pos,24);
  	break;
  case 'Y':		/* cursor addressing */
	em_routine = esc_y;
  	break;
  case 'I':		/* reverse line feed */
	if (y_pos == 0) {	/* at top of screen */
		int save_attrib;

		save_attrib = attrib;
		attrib = NORM_VIDEO;
		
		scrolldn();	/* scroll down */
		set_cursor();
		attrib = save_attrib;
		}
	else {
		y_pos--;
		pos -= 80;
		set_cursor();
	}
  	break;
  case 'L':		/* insert line */
	x_pos = 0;
	pos = 80 * y_pos;
	set_cursor();
	move_lines(23,24,24 - y_pos);
	clear_lines(y_pos,1);
  	break;
  case 'M':		/* delete line */
	x_pos = 0;
	move_lines(y_pos + 1, y_pos, 24 - y_pos);
  	break;
  case '@':		/* enter insert character mode */
	insert_char = TRUE;
  	break;
  case 'O':		/* exit insert character mode */
	insert_char = FALSE;
  	break;
  case 'N':		/* delete character */
	read_line(line_buf,y_pos);
	for (count = x_pos; count < 79; count++)
		line_buf[count] = line_buf[count + 1];
	line_buf[79] = SPACE | (attrib << 8);
	write_line(line_buf,y_pos);
  	break;
  case 'j':		/* save cursor position */
	y_save = y_pos;
	x_save = x_pos;
  	break;
  case 'k':	 	/* set cursor to saved position */
	y_pos = y_save;
	x_pos = x_save;
	pos = x_pos + 80 * y_pos;
	set_cursor();
  	break;
  case 'n':		/* report cursor location */
	h19_put_q(ESC);
	h19_put_q('Y');
	h19_put_q(y_pos + 32);
	h19_put_q(x_pos + 32);
  	break;
  case 'l':		/* erase line */
	clear_lines(y_pos,1);
  	break;
  case 'v':		/* wrap around mode on */
	wrap_around = TRUE;
  	break;
  case 'w':		/* wrap around mode off */
	wrap_around = FALSE;
  	break;
  case 'p':		/* enter reverse video */
	attrib = REV_VIDEO;
  	break;
  case 'q':		/* exit reverse video */
	attrib = NORM_VIDEO;
  	break;
  case 'o':		/* erase from begining of line */
	read_line(line_buf,y_pos);
	for(count = 0; count < x_pos; count++)
		line_buf[count] = SPACE | (attrib << 8);
	write_line(line_buf,y_pos);
  	break;
  case 'b':		/* erase from begining of screen */
	/* first erase top of screen */
	if (y_pos > 0)
		clear_lines(0,y_pos);

	/* now erase to cursor on current line */
	read_line(line_buf,y_pos);
	for(count = 0; count < x_pos; count++)
		line_buf[count] = SPACE | (attrib << 8);
	write_line(line_buf,y_pos);
   	break;
  case 'x':
  case 'y':
	em_routine = dummy;
	break;
  case '<':		/* enter ANSI mode */
	mode = MD_ANSI;
	break;
  }
}

static dummy() {
	em_routine = norm;
	}

static esc_y(data)
	char data; {

	em_routine = esc_hav_y;
	data -= 32;
	if((data <= 24) && (data >= 0))
		y_pos = data;
	}

static esc_hav_y(data)
	char data; {

	em_routine = norm;

	x_pos = data - 32;
	pos = x_pos + 80*y_pos;
	set_cursor();
	}

static ansi_esc(data)
	char	data; {

	if (data == '[')
		em_routine = ansi_brack;
	else {
		em_routine = norm;
		mode = MD_HEATH;
		}
	}

static ansi_brack(data)
	char	data; {
	static int	arg = 0;
	int		count;
	int		line_buf[80];

	if ((data <= '9') && (data >= '0')) {
		arg = (arg*10) + (data - '0');
		return;
	}

	em_routine = norm;

	if (arg == 0)
		arg = 1;
	switch(data) {
	case '?':
		em_routine = ansi_qmark;
		arg = 0;
		return;
	case 'L':	/* Insert Lines */
		move_lines(24 - arg, 24, 25 - arg - y_pos);
		clear_lines(y_pos, arg);
		x_pos = 0;
		pos = 80 * y_pos;
		set_cursor();
		break;
	case 'M':	/* Delete Lines */
		move_lines(y_pos + arg, y_pos, 24 - y_pos);
		clear_lines(24 - arg, arg);
		y_pos++;
		x_pos = 0;
		pos = y_pos * 80;
		set_cursor();
		break;
	case 'P':	/* Delete Characters */
		read_line(line_buf,y_pos);
		for (count = x_pos; count < 80 - arg; count++)
			line_buf[count] = line_buf[count + arg];
		for (count = 80 - arg; count < 79; count++)
			line_buf[count] = SPACE | (attrib << 8);
		write_line(line_buf,y_pos);
		break;
		}
	arg = 0;
	}

static ansi_qmark(data)
	char	data; {
	static int	arg = 0;

	if ((data <= '9') && (data >= '0')) {
		arg = arg*10 + arg - '0';
		return;
	}
	mode = MD_HEATH;
	em_routine = norm;
	}

/* write_string(string,y,x,attrib) - writes the passed string to the x and y
 *		locations with the passed attribute.
 */

write_string(string,y,x,attrib)
char	*string;
int	x,y;
char	attrib;
{
  int	newpos;
  newpos = x + 80 * y;
  while(*string != '\0')
	nwrite_char((*string++ & 0xff) | (attrib << 8),newpos++);
}
