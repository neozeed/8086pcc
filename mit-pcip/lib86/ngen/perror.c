#include <stdio.h>

/*
 * Print the error indicated
 * in the cerror cell.
 */

int	errno;
int	sys_nerr = 18;
char	*sys_errlist[] = {
	"No error",
	"Invalid function",
	"File not found",
	"Path not found",
	"Too many open files",
	"Access denied",
	"Invalid handle",
	"Memory control corrupted",
	"Out of memory",
	"Bad memory block address",
	"Invalid environment",
	"Invalid format",
	"Invalid data",
	"Invalid drive",
	"Can't remove current directory",
	"Not same device",
	"No more files"
	};

perror(s)
char *s;
{
	register char *c;
	register n;

	c = "Unknown error";
	if(errno < sys_nerr)
		c = sys_errlist[errno];
	n = strlen(s);
	if(n)
		fprintf(stderr, "%s: ", s);

	fprintf(stderr, "%s\n", c);
}
