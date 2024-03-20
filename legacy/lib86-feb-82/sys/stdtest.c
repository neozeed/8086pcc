/* Test Acorn file I/O stuff
 */

#include <stdio.h>


FILE *file;

readtest()
 {	int fid, bufsize=1024, size=0, left=0, i;
	char *name = "test.fil", buf[1024], eof=0;

	printf("Opening file %s\r\n", name);
	file = fopen("test.fil", "r");

	if (file == NULL)
	 { printf("FAILURE: open returns %d.\r\n", file); return;
	 }
	else printf("sucessfully opened ... fid is %d\r\n", fid);

	while ((i = getc(file)) != EOF)
	 { size++; putchar(i);
	   if (i == '\n') putchar('\r');
	 }

	printf("EOF after %d chars.\r\n", size);
	printf("fclose returns %d\r\n\n", fclose(file));
 }


writetest()
 {	int fid, bufsize=1024, size=0, left=0, i, j, k;
	char *name = "test.fil", buf[1024], eof=0, *aa, *bb;

	printf("Opening file %s for writing\r\n", name);
	file = fopen("test.fil", "w");

	if (file == NULL)
	 { printf("FAILURE: open returns %d.\r\n", fid); return;
	 }
	else printf("sucessfully opened ... fid is %d\r\n", fid);

	for (i=0; i<20; i++)
	 { for (j=0; j<i; j++) fprintf(file, "  ");
	   fprintf(file, "Line %d\r\n", i); }

	printf("fclose returns %d\r\n\n", fclose(file));

 }


main()
 {
	writetest();
	readtest();
 }
