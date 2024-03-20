CPP = cpp
CC1 = c86
AS = a86
LD = ld
AR = ar
CVT = cvt86
EMUL = msdos


CPPFLAGS = -P -U -nostdinc -DIBMDOS -Uvax -Dibm -I..\..\include -I$(INCLUDE)
LDFLAGS = -X -r

OBJS=   fib.b

.c.b:
        $(CPP) $(CPPFLAGS) $*.c -o $*.i
        $(CC1) < $*.i > $*.a86
	@del $*.i
        $(AS) $*.a86
	@del $*.a86

.a86.b:
        $(AS) $*.a86

