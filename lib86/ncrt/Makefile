# Description file for ncrt: primitive functions, arithmetic etc.

OBJECTS =       abort.b arith.b mcount.b

all:
	cd ..\crt& nmake
	copy /Y ..\crt\*.b .
	a86 arith
	ar cr ..\nlibc.a $(OBJECTS)

clean:
	@del $(OBJECTS)