#include <stat.h>

extern unsigned _dosax, _dosbx, _doscx, _dosdx;
long flength();

fstat(fid, ss)
 register struct stat *ss;
 {	register i;
	char *p;
	p = (char *) ss;
	for (i = sizeof *ss; i--; ) *p++ = 0;

	if (_ioctl(0, fid) < 0) return -1;
	i = _dosdx;
	ss -> st_dev = i & 0x8F;	/* Pseudo-device number.	*/

	if (i & 0x80)			/* A disk file...		*/
	 { ss->st_size = flength(fid);
	 }	
	return 0;
 }

stat(name, ss)
 char *name;
 struct stat *ss;
 {	int fid = open(name, 0), res;
	if (fid < 0) return -1;
	res = fstat(fid, ss);
	close(fid);
	return res;
 }
