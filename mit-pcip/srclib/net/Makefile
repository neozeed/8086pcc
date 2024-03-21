include ..\nhowto.mak


LIB = ../../lib
INCLUDE = ../../include
OBJS = printf.b netinit.b netclose.b netstats.b net_utils.b

# install: net

default: libnet.a netcrt.b

libnet.a: $(OBJS)
	ar r libnet.a $(OBJS)
	ranlib libnet.a

netinit.b: $(INCLUDE)/custom.h $(INCLUDE)/netbuf.h

netclose.b: $(INCLUDE)/net.h

q.b: $(INCLUDE)/q.h

netstats.b: $(INCLUDE)/netbuf.h

lintlib:
	lint86 -Cnet netinit.c netclose.c netstats.c out_inaddr.c \
	printf.c lint.c
	mv llib-lnet.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

netcrt.b:
	a86 $*

net_utils.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libnet.a
	@del netcrt.b
