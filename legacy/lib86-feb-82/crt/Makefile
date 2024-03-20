# Description file for crt: primitive functions, arithmetic etc.
include ..\howto.mak

OBJECTS =	abort.b arith.b mcount.b

all:		$(OBJECTS)
	$(AR) cr ..\libc.a $(OBJECTS)

abort.b:	abort.a86
		a86 abort

arith.b:	arith.a86
		a86 arith

mcount.b:	mcount.a86
		a86 mcount


clean:
	del $(OBJECTS)