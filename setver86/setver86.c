/*  Copyright 1984 by the Massachusetts Institute of Technology  */
/*  See permission and disclaimer notice in file "notice.h"  */
#include "..\mit-pcip\include\notice.h"

#include <stdio.h>
#include <fcntl.h>


/* program to store the version number of a PC network program inside the
	first record of the program.
*/

extern int errno;
extern char *sys_errlist[];

main(argc, argv)
	int argc;
	char *argv[]; {
	int fi;
	short number;

	if(argc != 3) {
		printf("usage: %s file version_number\n", argv[0]);
		exit(1);
		}

	fi = open(argv[1], O_RDWR|O_BINARY);
	if(fi < 0) {
		printf("Couldn't open %s: %s\n", argv[1], sys_errlist[errno]);
		exit(1);
		}

	lseek(fi, 3L, 0);
	number = atoi(argv[2]);
	write(fi, &number, sizeof(short));
	close(fi);
	}
