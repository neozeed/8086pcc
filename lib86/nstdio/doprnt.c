# include <stdio.h>

_doprnt(fmt, argp, file)
register char *fmt;
register int *argp;
register FILE *file;
{	register char c, *p, *q;
	register int d, width, ndigit, radix;
	long n;
	int sign, decpt;
	char t[128], digits[10], zfill, rjust, ndfnd;
	char *ecvt(), *fcvt();
	struct {
		short unsigned length;
		char dtype;
		char class;
		char *ptr;
	} desc;
	p = &t[0];
	while (c = *fmt++)
		if (c != '%')
			*p++ = c;
		else {
			if (p != &t[0]) {
				_strout(t, p-t, 0, file);
				p = &t[0];
			}
			rjust = 0;
			ndigit = 0;
			zfill = ' ';
			if (*fmt == '-') {
				rjust++;
				fmt++;
			}
			if (*fmt == '0') {
				zfill = '0';
				fmt++;
			}
			if(*fmt != '*') { 
				width = 0;
				while ((d = *fmt++ - '0') >= 0 && d <= 9)
					width = width*10+d;
			}
			else {
				fmt++;
				d = *fmt++ - '0';
				width = *argp++;
			}
			ndfnd = 0;
			if ((d += '0') == '.') {
			    if(*fmt != '*') {
				ndigit = 0;
				while ((d = *fmt++ - '0') >= 0 && d <= 9) {
					ndfnd++;
					ndigit = ndigit*10+d;
				}
				d += '0';
			    }
			    else {
				fmt++;
				d = *fmt++;
				ndfnd++;
				ndigit = *argp++;
			    }
			}
			switch (d) {
			case 'l':
			case 'L':
				switch(*fmt++) {
				case 'o': goto loct;
				case 'x': goto lhex;
				case 'd': goto ldec;
				case 'u': goto luns;
				default:
					fmt--;
					goto uns;
				}
			case 'o':
			case 'O':
			loct:	radix = 8;
				n = *argp++;
				goto compute;
			case 'X':
			lhex:	radix = 16;
				n = *(long *)argp;
				argp + = 2;
				goto hexcomp;

			case 'x':
				radix = 16;
				n = *argp++;
			hexcomp:
				if (n == 0 && ndigit == 0)
					*p++ = '0';
				for (q = &digits[0]; n != 0;
						n = (n>>4) & 0x0fffffff)
				{
					d = n & 0xF;
					*q++ = d + (d<10?'0':'a'-10);
				}
				while (q > &digits[0])
					*p++ = *--q;
				goto prbuf;


			case 'D':
			ldec:	
				radix = 10;
				n = *(long *)argp;
				if(n < 0){
					n = -n;
					*p++ = '-';
				}
				argp += 2;
				goto compute;

			case 'd':
				radix = 10;
			signed:	if (*argp < 0) {
					*p++ = '-';
					n = -*argp++;
				} else
					n = *argp++;
				goto compute;

			case 'u':
			case 'U':
			luns:
			uns:	n = *argp++;
				radix = 10;
			compute:
				if (n == 0 && ndigit == 0)
					*p++ = '0';
				for (q = &digits[0]; n != 0; n = n/radix)
				{
					d = n%radix; 
/*					d = n - ((n/radix) * radix); */
/*					d = n;
					while(d >= radix) d -= radix;
*/					if(d<0) d += radix;
					*q++ = d + (d<10?'0':'a'-10);
				}
				while (q > &digits[0])
					*p++ = *--q;
				goto prbuf;

			case 'c':
				for (q = (char *)argp++, d = 0; d < sizeof(int); d++)
					if ((*p++ = *q++) == 0)
						p--;
			prbuf:	q = &t[0];
			prstr:	if ((d = width - (p - q)) < 0)
					d = 0;
				if (rjust == 0)
					d = -d;
				_strout(q, p-q, d, file, zfill);
				p = &t[0];
				break;

			case 's':
				if ((q = (char *)*argp++) == 0)
					q = "(null)";
				if ((d = ndigit) == 0)
					d = 32767;
				for (p=q; *p!=0 && --d>=0; p++);
				goto prstr;

			case 'r':
				argp = (int *)*argp;
				fmt = (char *)*argp++;
				break;

			case '%':
				*p++= '%';
				break;
			}
	}
	if (p != &t[0])
		_strout(t, p-t, 0, file);
}
