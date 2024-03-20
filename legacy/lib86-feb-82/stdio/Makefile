
include ..\howto.mak

OBJECTS =	clrerr.b data.b doprnt.b doscan.b fdopen.b fgetc.b fgets.b \
	filbuf.b flsbuf.b fopen.b fprintf.b fputc.b fputs.b freopen.b \
	fseek.b ftell.b gcvt.b getchar.b gets.b getw.b printf.b \
	putchar.b puts.b putw.b rdwr.b rew.b scanf.b setbuf.b \
	sprintf.b strout.b system.b ungetc.b



all:		$(OBJECTS)
	$(AR) cr ..\libc.a $(OBJECTS)

$(OBJECTS):	

clean:
	@del $(OBJECTS)