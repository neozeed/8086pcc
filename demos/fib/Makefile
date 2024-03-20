CPP = cpp
CC1 = c86
AS = a86
LD = ld
CVT = cvt86
EMUL = msdos


CPPFLAGS = -P -U -nostdinc -DIBMDOS -Uvax -Dibm -I$(INCLUDE)
LDFLAGS = -X -r

OBJS=   fib.b

.c.b:
        $(CPP) $(CPPFLAGS) $*.c -o $*.i
	$(CC1) < $*.i > $*.a86
	$(AS) $*.a86

all: fib.com


fib.com: $(OBJS)
	$(LD) $(LDFLAGS) -o fib.out $(LIB)\crt0.b $(OBJS) $(LIB)\libc.a
	$(CVT) fib.out fib.com

clean:
	del $(OBJS)
	del fib.com fib.out
	del *.a86 *.b *.i

