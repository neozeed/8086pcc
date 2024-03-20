include ..\howto.mak


ASMOBJ = blt.b clear.b mfill.b setjmp.b

OBJECTS =	abs.b atof.b atoi.b atol.b calloc.b crypt.b ctime.b ctype_.b \
	cuexit.b ecvt.b errlst.b execvp.b frexp.b getenv.b getlogin.b index.b l3.b \
	malloc.b mktemp.b mon.b perror.b qsort.b rand.b rindex.b sleep.b \
	strcat.b strcatn.b strcmp.b strcmpn.b strcpy.b strcpyn.b strlen.b strncat.b strncmp.b \
	strncpy.b stty.b swab.b tell.b timezone.b ttyname.b ttyslot.b



all:		$(OBJECTS) $(ASMOBJ)
	ar cr ..\libc.a $(OBJECTS) $(ASMOBJ)

blt.b:		blt.a86
		a86 blt

clear.b:	clear.a86
		a86 clear

mfill.b:	mfill.a86
		a86 mfill

setjmp.b:	setjmp.a86
		a86 setjmp

#atof.b:		atof.b
#		cc86 -8087 -c atof.b

#ecvt.b:		ecvt.b
#		cc86 -8087 -c ecvt.b

#frexp.b:	frexp.b
#		cc86 -8087 -c frexp.b


clean:
	@del $(OBJECTS) $(ASMOBJ)