include ..\howto.mak

LIB = ../../lib

OBJS = profile.b prof.b pcrt.b pnetcrt.b

libprof: $(OBJS)
	ar r libprof.a profile.b prof.b
	ranlib libprof.a

#.c.b:
#	cc86 -c -DDEBUG $*.c

prof.b:
	a86 $*

pcrt.b:
	a86 $*

pnetcrt.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libprof.a
