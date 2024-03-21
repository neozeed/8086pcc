include ..\howto.mak

ASMOBJ = blt.b clear.b mfill.b setjmp.b

OBJECTS =	abs.b atoi.b  atof.b atol.b  calloc.b crypt.b\
		ctime.b ctype_.b cuexit.b ecvt.b errlst.b execvp.b \
		getlogin.b index.b l3.b malloc.b frexp.b\
		mktemp.b mon.b perror.b qsort.b rand.b\
		rindex.b sleep.b strcat.b strcatn.b strcmp.b strcmpn.b\
		strcpy.b strcpyn.b strlen.b strncat.b strncmp.b strncpy.b\
		stty.b swab.b tell.b timezone.b ttyname.b ttyslot.b\
		access.b stat.b

all:	$(OBJECTS) $(ASMOBJ)
	ar cr ..\libc.a $(OBJECTS) $(ASMOBJ)


blt.b:		blt.a86
		a86 blt

clear.b:	clear.a86
		a86 clear

mfill.b:	mfill.a86
		a86 mfill

setjmp.b:	setjmp.a86
		a86 setjmp

clean:
	@del $(OBJECTS) $(ASMOBJ)
