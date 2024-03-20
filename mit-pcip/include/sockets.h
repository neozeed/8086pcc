/*  Copyright 1984 by the Massachusetts Institute of Technology  */

/* This file contains definitions for sockets. We probably won't need
   anything more flexible than this for a long time, but it will be
   convenient to have one place to look for all socket numbers.
   The format is PROTOCOL_SERVICE.
*/

#define	UDP_TFTP	69
#define	UDP_NAME	42
#define	UDP_COOKIE	17
#define	UDP_TIME	37
#define	UDP_LOG		22
#define	UDP_TALK	517
#define	UDP_CHARGEN	19
#define	UDP_RWHO	513
#define	UDP_FLASH	801
#define	UDP_PRINTER	35

#define	TCP_TELNET	23
#define	TCP_FINGER	79
#define	TCP_WHOIS	43
#define	TCP_SMTP	25
#define	TCP_SUPDUP	95
#define	TCP_FTP		21
#define	TCP_SYSTAT	11
#define	TCP_WRITE	115
#define	TCP_CHARGEN	19
#define	TCP_REXEC	512
#define	TCP_DMSP	800
#define	TCP_PRINTER	35
