/* simple print pgm for UNIX-style Acorn files			6/82 SAW
 */

#include <stdio.h>

int lines = 0;

pr(name)
 char *name;
 {	register FILE *f = fopen(name, "r");
	register int ch;

	for (;;) switch(ch = getc(f))
	 { case EOF:	fclose(f); return;
	   case '\n':	putchar('\n'); putchar('\r'); lines++; continue;
	   default:	putchar(ch); continue;
	 }
 }

main(argc, argv)
 char **argv;
 {	register int i;
	for (i=1; i<argc; i++) pr(argv[i]);
 }

