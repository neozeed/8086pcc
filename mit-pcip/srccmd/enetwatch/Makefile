include ..\howto.mak

OBJ = ../../obj
LIB = \pcc\lib
OBJS = netwatch.b display.b screen.b time.b
LIBS = -L$(LIB) -lnet -lwether -ltask -lh19 -lpc $(LIB)\netlibc.a

enetwatch: $(OBJS)
	$(LD) $(LDFLAGS) -o enetwatch.out $(LIB)\netcrt.b $(OBJS) $(LIBS)
	$(CVT) enetwatch.out enetwatch.com

lint:
	lint86 netwatch.c time.c screen.c -lwether -lnet -ltask -lh19 -lc -lpc

all: enetwatch

install:
	mv enetwatch.com $(OBJ)


.a86.b:
	a86 $*

clean:
	@del $(OBJS)
	@del enetwatch.com enetwatch.out
