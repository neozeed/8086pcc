/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>


/* resolve_name.c */

/* Resolve a host name into an internet address.  Three name formats are
 * accepted:
 *	1) A character string host name
 *	2) An octal host number, in the form:
 *		<net>,<subnet>,<rsd>,<host>
 *	   or a decimal host number, in the form:
 *		<net>.<subnet>.<rsd>.<host>
 *	   Any of the <net>, <subnet>, and <rsd> may be left blank or left out
 *	   entirely; they default to the local net/subnet.
 *	3) A thirty-two bit hex number, preceeded by a '#', which is used
 * 	   without interpretation as the host number.
 * If a character string name is supplied, it is first looked up in a
 * local host table.  If it is not found there, the routine goes off to
 * internet name servers to try to resolve the name.
 *
 * The following routines are included in this file:
 *	resolve_name	Resolve a name as specified above
 *	gethmch		Parse a hex machine address specification
 *	getomch		Parse an octal machine address specification
 * The routines which use the internet name server are in the file
 * name_user.c.
 */


#define	INSZ	4
#define	TRUE	1
#define	FALSE	0

extern	long	getomch();
extern	long	getdmch();
extern	long	gethmch();
extern	long	udpname();

#define MYNET	022
#define MYSNET	011
#define MYRSD	0
#define MYHOST	0


/* Resolve foreign host internet address
 * Scan table of host names and nicknames.
 * For each name, see if our string is a prefix.  If so, keep checking -
 * could be ambiguous.
 * If ambiguous, return 0.
 * When find no matches, try internet name servers.
 *
 * Arguments:
 */

long resolve_name (name)
	register char *name; {			/* name to resolve */

	if((name[0] >= '0') && (name[0] <= '9')) 
		if (name[1] == '.' ||
		    name[2] == '.' ||
		    name[3] == '.')
		    return getdmch(name);
		    else return getomch(name);
		
	else if(name[0] == '#')
		return gethmch(name);
		
	else return udpname(name); } 

	
/* Parse foreign host number input as hex string */
long gethmch(name)
	register char *name; {
	register char	*tmp;
	register int	i;
	union {
		char	bytes[INSZ];
		long	name;
	} addr;
	
	tmp = &name[1];
	for (i = 0; i < 8; i++) {
		if ((*tmp >= '0') && (*tmp <= '9'))
			*tmp++ -= '0';
		else if ((*tmp >= 'A') && (*tmp <= 'Z'))
			*tmp++ -= ('A' - 10);
		else if ((*tmp >= 'a') && (*tmp <= 'z'))
			*tmp++ -= ('a' - 10);
		else return (0L);
		}

	if (*tmp != 0) return (0L);

	tmp = &name[1];
	for (i = 0; i < INSZ; i++)
		addr.bytes[i] = ((*tmp++ << 4) + *tmp++);
	return (addr.name);
}

/* Parse foreign host number input as octal string */

long getomch(name)
char	*name;
{
	char		tmp[INSZ];
	register char	*p;
	register int	i, j;
	register int	n;
	union {
		char	bytes[INSZ];
		long	name;
	} addr;

	addr.bytes[0] = MYNET;
	addr.bytes[1] = MYSNET;
	addr.bytes[2] = MYRSD;
	addr.bytes[3] = MYHOST;

	p = name;
	for (i = 0; i < INSZ; i++) {
		n = 0;
		while((*p >= '0') && (*p <= '7')) {
			n = ((n << 3) + (*p++ - '0'));
			if (n > 0377) return (0L);
			}
		tmp[i] = n;
		if (*p == 0) break;
		if (*p == ',') p++;
		else return (0L);
		}

	if (i == INSZ) return (0L);

	for (j = 3; j >= 0; j--) {
		addr.bytes[j] = tmp[i];
		if (--i < 0) break;
		}

	return (addr.name);
}


/* Parse foreign host number input as decimal string */
long getdmch(name)
char	*name;
{
	char		tmp[INSZ];
	register char	*p;
	register int	i, j;
	register int	n;
	union {
		char	bytes[INSZ];
		long	name;
	} addr;

	addr.bytes[0] = MYNET;
	addr.bytes[1] = MYSNET;
	addr.bytes[2] = MYRSD;
	addr.bytes[3] = MYHOST;

	p = name;
	for (i = 0; i < INSZ; i++) {
		n = 0;
		while((*p >= '0') && (*p <= '9')) {
			n = ((n << 3) +(n << 1) + (*p++ - '0'));
			if (n > 0377) return (0L);
			}
		tmp[i] = n;
		if (*p == 0) break;
		if (*p == '.') p++;
		else return (0L);
		}

	if (i == INSZ) return (0L);

	for (j = 3; j >= 0; j--) {
		addr.bytes[j] = tmp[i];
		if (--i < 0) break;
		}

	return (addr.name);
}

