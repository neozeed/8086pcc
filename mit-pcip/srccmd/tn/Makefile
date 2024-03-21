include ..\howto.mak

OBJ = ../../obj
LIB = \pcc\lib
INCLUDE = ../../include

OBJS = telnet.b telnet2.b time.b tn.b tnscreen.b

LIBS = -L$(LIB) -ltftp -ltcp -ludp -lether -lnet -lip -ltask -lh19 -lnet -lpc $(LIB)\nlibc.a

etn: $(OBJS)
#	cc86 -o etn tn.b -ludp -lether -lip -lnet -ltask -lnc -lh19
	$(LD) $(LDFLAGS) -o etn.out $(LIB)\netcrt.b $(OBJS) $(LIBS)
	$(CVT) etn.out etn.com
#	setver86 etn.com 71

ptn: tn.b
#	cc86 -o ptn tn.b -ludp -lv2 -lip -lnet -ltask -lnc -lh19
	setver86 ptn.com 50

stn: tn.b
#	cc86 -o stn tn.b -ludp -lserial -lip -lnet -ltask -lnc -lh19
	setver86 stn.com 50

tn.b: $(INCLUDE)/ip.h

lint:
	lint86 tn.c -ludp -lether -lip -lnet -ltask -lnc -lh19 -lnc
	lint86 tn.c -ludp -lserial -lip -lnet -ltask -lnc -lh19 -lnc
	lint86 tn.c -ludp -lv2 -lip -lnet -ltask -lnc -lh19 -lnc

all: etn ptn stn

install:
	mv etn.com stn.com ptn.com $(OBJ)

#.c.b:
#	cc86 -c -DDEBUG $*.c

.a86.b:
	a86 $*


clean:
	@del $(OBJS) *.out *.com