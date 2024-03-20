# Description file for ncrt: primitive functions, arithmetic etc.

NOTNEEDED = atof.b ecvt.b frexp.b

ASMOBJ = blt.b clear.b mfill.b setjmp.b

OBJECTS =       abs.b atoi.b  atol.b  calloc.b crypt.b\
                ctime.b ctype_.b cuexit.b errlst.b execvp.b \
                getlogin.b index.b l3.b malloc.b \
                mktemp.b mon.b perror.b qsort.b rand.b\
                rindex.b sleep.b strcat.b strcatn.b strcmp.b strcmpn.b\
                strcpy.b strcpyn.b strlen.b strncat.b strncmp.b strncpy.b\
                stty.b swab.b tell.b timezone.b ttyname.b ttyslot.b\
                access.b stat.b


all:
	cd ..\gen& nmake
	copy /Y ..\gen\*.b .
	ar cr ..\nlibc.a $(OBJECTS) $(ASMOBJ)

clean:
	@del $(OBJECTS) $(ASMOBJ) $(NOTNEEDED)
