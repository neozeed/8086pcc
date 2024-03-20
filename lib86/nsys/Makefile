# Description file for ncrt: primitive functions, arithmetic etc.

ASMOBJ = handler.b io.b msd2dot0.b dos.b rwbyte.b
OBJECTS = sys2.b


all:
	cd ..\sys& nmake
	copy /Y ..\sys\*.b .
	ar cr ..\nlibc.a $(OBJECTS) $(ASMOBJ)

clean:
	@del $(OBJECTS) $(ASMOBJ)