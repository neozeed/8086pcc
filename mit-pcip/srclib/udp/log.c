/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


#include <udp.h>
#include <sockets.h>
#include "log.h"

/* 8/23/84 - changed to use UDP_LOG from sockets.h. Also fixed bug where
	log() wouldn't free its packet if it couldn't send it. Also
	changed some debugging messages.
						<John Romkey>
   8/24/84 - increased buffer size in log().
						<John Romkey>
   8/30/84 - split out the packet header declaration into log.h.
						<John Romkey>
   10/3/84 - changed log() to work with _doprnt in the new I/O library.
						<John Romkey>
*/

/* log user program; takes a logname string and a message string, copies them
	into a packet and sends them to our local neighborhood logging
	server.	*/

static UDPCONN logudp;
static int logrcv();

LogInit() {
	if(custom.c_log == 0) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("loginit: server not known\n");
#endif
		return 0;
		}

	logudp = udp_open(custom.c_log, UDP_LOG, udp_socket(), logrcv);
	if(logudp == NULL)	{
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|NETERR|PROTERR))
			printf("loginit: can't open udp con\n");
#endif
		}
	}

static int logrcv(p, len, host)
	PACKET p;
	int len;
	in_name host;	{

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("loguser: pkt from %a", host);

#endif

	udp_free(p);
	}

dologmsg(logname, msg)
	char *logname;
	char *msg; {
	PACKET p;
	struct logmsg *plm;
	char *text;
	unsigned len;
	int i;
	unsigned done = 0;

	if(custom.c_log == NULL || logudp == NULL) return 0;

	len = sizeof(struct logmsg)+strlen(msg);

	p = udp_alloc(len, 0);
	if(p == NULL) {
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("dolog: can't alloc pkt\n");
#endif	
		return;
		}

	plm = (struct logmsg *)udp_data(udp_head(in_head(p)));
	plm->l_type = '1';
	plm->l_subtype = '4';
	plm->l_id[0] = '0';
	plm->l_id[1] = '1';
	plm->l_status[0] = '4';
	plm->l_status[1] = '2';
	plm->l_substatus[0] = ' ';
	plm->l_substatus[1] = ' ';

	strcpy(plm->l_logname, logname);
	if(custom.c_user[0] == 0) strcpy(plm->l_sender, "IBM PC");
	else for(i=0; i<8; i++) {
		if(done) plm->l_sender[i] = 0;
		else {
			plm->l_sender[i] = custom.c_user[i];
			if(custom.c_user[i] == 0) done = 1;
			}
		}

	text = ((char *)plm) + sizeof(struct logmsg);
	strcpy(text, msg);

#ifdef	DEBUG
	if(NDEBUG & INFOMSG)
		printf("dolog: logging to %a\n", custom.c_log);
#endif

	if(udp_write(logudp, p, len) < 0)
#ifdef	DEBUG
		if(NDEBUG & (INFOMSG|PROTERR))
			printf("log: can't send pkt\n");
#endif

	udp_free(p);
	return;
	}


/* Send out a log packet to the server. */
extern char *_sp;
int store_string();

log(logname, format, args)
	char *logname;
	char *format;
	int args; {
	char msg[500];
/*	struct _iobuf _strbuf;

	_strbuf._flag = _IOWRT+_IOSTRG;
	_strbuf._ptr = msg;
	_strbuf._cnt = 500;
	_doprnt(format, &args, &_strbuf);
	putc('\0', &_strbuf);
	dologmsg(logname, msg);
*/

	_sp = msg;
	_format_string(store_string, NULL, &format);
	*_sp = 0;
	dologmsg (logname, msg);
	}
