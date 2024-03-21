include ..\nhowto.mak

OBJECTS =	fprintf.b sprintf.b printf.b scanf.b clrerr.b doscan.b\
		doprnt.b fdopen.b fgetc.b fopen.b fgets.b fputc.b\
		getc.b putc.b _rbyte.b _wbyte.b \
		fputs.b freopen.b fseek.b ftell.b gcvt.b gets.b getw.b \
		putchar.b puts.b putw.b	rdwr.b rew.b strout.b\
		ungetc.b setbuf.b flsbuf.b filbuf.b data.b

all:	$(OBJECTS)
	ar cr ..\nlibc.a $(OBJECTS)


clean:
	@del $(OBJECTS)
