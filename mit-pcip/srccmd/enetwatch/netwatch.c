/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include <stdio.h>
#include <task.h>
typedef long in_name;
#include <custom.h>
#include <em.h>
#include <attrib.h>
#include <match.h>
#include "modes.h"

char *header =
"destination   source        type  len  0    1    2    3    4    5    6    7     ";
/*		 ^	       ^	*/

unsigned start=14;
char keybuf[100];
unsigned match = M_ALL;
unsigned mtype;
char maddr[6];

unsigned prot_mode = MD_SYMBOLIC;

char *proff = "New offset into packet [decimal]: ";
char *prtype = "Type to match on [hex]: ";
char *prsrc = "Source ethernet address to match: ";
char *prdst = "Destination ethernet address to match: ";
char *minlen = "New minimum packet length: ";

extern unsigned clear25;
extern long npackets;
extern unsigned NBUF;
extern unsigned x_pos, y_pos, pos;
extern unsigned etminlen;

extern int pkt_display(), scr_close(), _curse();

main() {
	char c;
	int i;

	NBUF = 10;

	Netinit(1200);
	scr_init();

	exit_hook(_curse);
	exit_hook(scr_close);

	if(tk_fork(tk_cur, pkt_display, 1200, "Pkt display") == NULL) {
		printf("couldn't fork packet display task\n");
		exit(1);
		}

	clear_lines(0, 25);

	fixhdr(14);

	write_string(header, 0, 0, UNDER);

	tminit();
	inv25();

	while(1) {
		c = h19key();
		if(c == NONE) {
			tk_yield();
			continue;
			}

		switch(c) {
		case 's':
			clr25();
			write_string("Entering symbolic mode", 24, 0, INVERT);
			start = 14;
			prot_mode = MD_SYMBOLIC;
			clear25 = 3;
			break;
		case 'I':
			clr25();
			write_string("Entering IP mode", 24, 0, INVERT);
			start = 14;
			match = M_TYPE;
			mtype = 0x800;
			clear25 = 3;
			break;
		case 'A':
			clr25();
			write_string("Entering ARP mode", 24, 0, INVERT);
			start = 14;
			match = M_TYPE;
			mtype = 0x806;
			clear25 = 3;
			break;
		case 'C':
			clr25();
			write_string("Entering Chaos mode", 24, 0, INVERT);
			start = 14;
			match = M_TYPE;
			mtype = 0x804;
			clear25 = 3;
			break;
		case 'n':
			clr25();
			write_string("Entering normal mode", 24, 0, INVERT);
			prot_mode = MD_NORMAL;
			clear25 = 3;
			break;
		case 'q':
			clr25();
			write_string("Exiting", 24, 0, INVERT);
			norm25();
			move_lines(1, 0, 24);
			set_cursor(pos = 24*80);
			x_pos = 0;
			y_pos = 24;
			scr_close();
			_curse();
			exit(0);
		case 'S':
			clr25();
			net_stats(stdout);
			write_string("statistics", 24, 0, INVERT);
			clear25 = 3;
			write_string(header, 0, 0, UNDER);
			break;
		case 'p':
			clr25();
			etarsnd(custom.c_defgw);
			write_string("sending ARP packet", 24, 0, INVERT);
			clear25 = 3;
			break;
		case 'o':
			clr25();
			pr25(0, proff);
			if(rdchrs(5, strlen(proff))) {
				clr25();
				break;
				}
			start = atoi(keybuf);
			fixhdr(start);
			clear_lines(0, 25);
			putchar(27); putchar('H');
			printf("\n");
			write_string(header, 0, 0, UNDER);
			inv25();
			clr25();
			break;
		case '<':
			clr25();
			pr25(0, minlen);
			if(rdchrs(4, strlen(minlen))) {
				clr25();
				break;
				}
			etminlen = atoi(keybuf);
			clr25();
			break;

		case '?':
			clr25();
			printf("\nCommand Summary:\n");
			printf("a\tmatch all packets\n");
			printf("d\tmatch on destination\n");
			printf("e\tdump ethernet interface registers\n");
			printf("l\tclear screen\n");
			printf("m\tmatch on source\n");
			printf("n\tnormal mode\n");
			printf("o\tset offset into packet for display\n");
			printf("p\tsend ARP packet to default gateway\n");
			printf("q\tquit\n");
			printf("r\treset packet count\n");
			printf("s\tenter symbolic mode\n");
			printf("t\tmatch on packet type\n");
			printf("A\tARP mode\n");
			printf("C\tChaos mode\n");
			printf("I\tIP mode\n");
			printf("S\tprint statistics\n");
			printf("?\tprint command summary\n");
			printf("+\tprint stack usage\n");
			printf("<\tset minimum packet length\n");
			write_string(header, 0, 0, UNDER);
			break;
		case 'a':
			clr25();
			match = M_ALL;
			pr25(0, "Match all packets");
			clear25 = 3;
			break;
		case '+':
			clr25();
			tk_stats(stdout);
			pr25(0, "Stack usage");
			clear25 = 3;
			break;
		case 'e':
			clr25();
			for(i=0x300; i<0x310; i++)
				printf("%02x ", inb(i));
			printf("\n");
			pr25(0, "Ethernet interface registers");
			break;
		case 't':
			clr25();
			pr25(0, prtype);
			if(rdchrs(8, strlen(prtype))) {
				clr25();
				break;
				}
			match = M_TYPE;
			mtype = htoi(keybuf);
			clr25();
			sprintf(keybuf, "Matching on type %x", mtype);
			pr25(0, keybuf);
			clear25 = 3;
			break;
		case 'm':
			clr25();
			pr25(0, prsrc);
			if(rdchrs(13, strlen(prsrc))) {
				clr25();
				break;
				}
			match = M_SRC;
			etparse();
			clear25 = 3;
			break;
		case 'd':
			clr25();
			pr25(0, prdst);
			if(rdchrs(13, strlen(prdst))) {
				clr25();
				break;
				}
			match = M_DST;
			etparse();
			clear25 = 3;
			break;
		case 'r':
			npackets = 0;
		case 'l':
			clear_lines(0, 25);
			putchar(27); putchar('H');
			printf("\n");
			write_string(header, 0, 0, UNDER);
			inv25();
			pr25(0, "Clear screen");
			clear25 = 3;
			break;
		default:
			clr25();
			write_string("unknown command", 24, 0, INVERT);
			}
		}
	}

fixhdr(base)
	unsigned base; {
	char *start;
	int i;

	start = header+37;

	for(i=0; i<8; i++)
		sprintf(start+i*5, " %4u", base+i);

	*(start+i*5) = ' ';
	}

rdchrs(n, scr_x)
	int n;
	int scr_x; {
	int pos=0;
	char c;
	char foo[2];

	foo[1] = 0;

	while(1) {
		c = h19key();
		switch(c) {
		case NONE:
			continue;
		case 0177:
			if(pos) {
				write_string(" ", 24, scr_x+pos, INVERT);
				keybuf[pos--]=0;
				}
			break;
		case 033:
			return 1;
		default:
			if(c == '\n' || c == '\r') {
				keybuf[pos]=0;
				return 0;
				}
			if(pos == n-1) ring();
			else {
				keybuf[pos++] = c;
				foo[0] = c;
				write_string(foo, 24, scr_x+pos, INVERT);
				continue;
				}
			}
		}
	}

htoi(s)
	register char *s; {
	unsigned a=0; 

	while(1) {
		if(*s >= '0' && *s <= '9') a = *s++ - '0' + 16*a;
		else if(*s >= 'a' && *s <= 'f') a = *s++ - 'a' + 10 + 16*a;
		else if(*s >= 'A' && *s <= 'F') a = *s++ - 'A' + 10 + 16*a;
		else return a;
		}
	}

etparse() {
	char *next;
	char old;
	int i;

	next = keybuf;

	for(i=0; i<6; i++) {
		if(*next == 0 || *(next+1) == 0) return;
		old = *(next+2);
		*(next+2) = 0;
		maddr[i] = htoi(next);
		*(next+2) = old;
		next++; next++;
		}
	}

