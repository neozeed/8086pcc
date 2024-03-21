/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include	<notice.h>

#include	<stdio.h>

int putc();

printf(format)
	char	*format; {

	_format_string(putc, stdout, &format);
	}

fatal(format)
	char *format; {

	_format_string(putc, stdout, &format);
	putchar('\n');
	exit();
	}
	
fprintf(fd, format)
FILE	*fd;
char	*format;
{
	_format_string(putc, fd, &format);
}

int	store_string();
char	*_sp;			/* this should be not be static since it */
				/* is used by TCP's printf and others 	 */

sprintf(s, format)
char	*s;
char	*format;
{
	_sp = s;
	_format_string(store_string, NULL, &format);
	*_sp = '\0';
}

store_string(c, foo)
char	c;
int	foo;
{
	*_sp++ = c;
}

_format_string(routine, outarg, data)
char	**data;		/* the address to get the format string and the
			 * agruments from */
int	(*routine)();	/* the routine to output the characters */
int	outarg;		/* an argument to pass to the output routine */
{
	unsigned	*arg;	/* a pointer to the arguments on the stack */
	char	*format;	/* a pointer to the format string */
	int	field;		/* number after a '%' and before the format
				 * character */
	int	l_zero;		/* flag is true if the number started with a
				 * '0', if so then print out leading zeros */
	char	*s;
	
	format = *data;
	arg = (unsigned *)(data) + 1;
loop:	while ((*format != '%') && (*format != '\0'))
		(*routine)(*format++, outarg);
	if (*format == '\0')
		return;

	format++;
	if (*format == '0') l_zero = TRUE;
	  else l_zero = FALSE;
	field = 0;
	while (*format >= '0' && *format <= '9') {
		field = field * 10 + *format - '0';
		format++;
	}
	switch (*format) {
	case 's':
		s = *(char **)arg;
		while (*s != '\0')
			(*routine)(*s++, outarg);
		arg++;
		break;
	case 'c':
		(*routine)(*(char *)arg, outarg);
		arg++;
		break;
	case '%':
		(*routine)('%', outarg);
		break;
	case 'o':
		int_print((unsigned long)*arg, 010, FALSE, field, l_zero, routine, outarg);
		arg++;
		break;
	case 'O':
		int_print(*(unsigned long *)arg, 010, FALSE, field, l_zero, routine, outarg);
		arg += 2;
		break;
	case 'd':
		int_print((long)*(int *)arg, 10, TRUE, field, l_zero, routine, outarg);
		arg++;
		break;
	case 'D':
		int_print(*(long *)arg, 10, TRUE, field, l_zero, routine, outarg);
		arg += 2;
		break;
	case 'u':
		int_print((unsigned long)*arg, 10, FALSE, field, l_zero, routine, outarg);
		arg++;
		break;
	case 'U':
		int_print(*(unsigned long *)arg, 10, FALSE, field, l_zero, routine, outarg);
		arg += 2;
		break;
	case 'x':
		int_print((unsigned long)*arg, 0x10, FALSE, field, l_zero, routine, outarg);
		arg++;
		break;
	case 'X':
		int_print(*(unsigned long *)arg, 0x10, FALSE, field, l_zero, routine, outarg);
		arg += 2;
		break;

	}
	format++;
	goto	loop;
}
	
	
int_print(number, radix, signed, field, l_zero, routine, outarg)
unsigned long	number;	/* the number to print */
int	radix;		/* the radix to print the number in */
int	signed;		/* true if should print as signed number */
int	field;		/* the argument in the format control */
int	l_zero;		/* true if arg had a leading zero */
int	(*routine)();	/* routine to print out the number */
int	outarg;		/* an argument to pass to the output routine */
{
	char	table[20];	/* place to make up the output */
	int	negative;	/* true if number is signed and negative */
	int	i;
	char	c;
	
	if (signed && ((long)number < 0)) {
		negative = TRUE;
		number = -(long)number;
	}
	else	negative = FALSE;
	
	c = l_zero ? '0' : ' ';
	for (i=0; i < 20; i++)
		table[i] = c;
	
	for (i = 0; i < 20; i++) {
		table[i] = "0123456789ABCDEF"[number % (long)radix];
		number /= (long)radix;
		if (number == 0) break;
	}
	
	if (negative) table[++i] = '-';
	if ((field != 0) && (field < 20)) i = field - 1;
	while (i >= 0)
		(*routine)(table[i--], outarg);
}
