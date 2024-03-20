/* Test Acorn file I/O stuff
 */

#define RECSIZE 256

dbg(a1, a2, a3, a4, a5, a6)
 {
	printf(a1, a2, a3, a4, a5, a6);
	printf("\r\n");
 }

readtest()
 {	int fid, bufsize=1024, size=0, left=0, i;
	char *name = "test.fil", buf[1024], eof=0;

	printf("Opening file %s\r\n", name);
	fid = open("test.fil", 0);
	if (fid < 0)
	 { printf("FAILURE: open returns %d.\r\n", fid); return;
	 }
	else printf("sucessfully opened ... fid is %d\r\n", fid);

	do { left = read(fid, buf, bufsize);
	     if (left < 0)
		{ printf("\r\nERROR: read returns %d\r\n", left);
		  break;
		}
	     if (left == 0) { eof++; break; }
	     if (left<bufsize) eof++;
	     size += left;
	     for (i=0; i<left; i++)
		{ putchar(buf[i]);
		  if (buf[i] == '\n') putchar('\r');
		}
	   }
	while (!eof);

	printf("\n\rEOF after %d bytes.\r\n\n", size);
	close(fid);
 }

writetest()
 {	int fid, bufsize=1024, size=0, left=0, i, j, k;
	char *name = "test.fil", buf[1024], eof=0, *aa, *bb;

	printf("Opening file %s for writing\r\n", name);
	fid = open("test.fil", 1);

	if (fid < 0)
	 { printf("FAILURE: open returns %d.\r\n", fid); return;
	 }
	else printf("sucessfully opened ... fid is %d\r\n", fid);

	i=0;
	for (aa="Testing write...\r\n"; buf[i++] = *aa++;);
	i--;
	for (aa="                   Second line.\r\n"; buf[i++] = *aa++;);
	i--;
	while (i%RECSIZE) buf[i++] = '-';
	i--; i--; buf[i++] = '\r'; buf[i++] = '\n';

	for (size=j=0; j<9; j++)
		{ k = write(fid, buf, i);
		  if ((k < 0) || (k != i))
			{ printf("ERROR: write returns %d\r\n", k);
			  return;
			}
		  size += i;
		}
	k = close(fid);
	if (k < 0) printf("Close returns %d\r\n", k);
	else printf("Successfully closed... size is %d\r\n", size);
 }


main()
 {
	writetest();
	readtest();
 }
