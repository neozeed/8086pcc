include ..\howto.mak

ASMOBJ = handler.b io.b msd2dot0.b dos.b rwbyte.b
OBJECTS = sys2.b

all:	$(OBJECTS) $(ASMOBJ)
	ar cr ..\libc.a $(OBJECTS) $(ASMOBJ)

msd2dot0.b:	msd2dot0.a86
		a86 msd2dot0


dos.b:		dos.a86
		a86 dos

handler.b: handler.a86
	a86 handler

io.b: io.a86
	a86 io

rwbyte.b:	rwbyte.a86
		a86 rwbyte


clean:
	@del $(OBJECTS) $(ASMOBJ)
