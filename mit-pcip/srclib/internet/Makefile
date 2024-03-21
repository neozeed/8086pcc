include ..\nhowto.mak

LIB = ../../lib
INCLUDE = ../../include
OBJS =  ggp.b icmp.b in_alloc.b in_dump.b in_init.b in_more.b in_mymach.b \
	in_open.b in_write.b indemux.b inroute.b


libip.a: $(OBJS)
	ar r libip.a $(OBJS)
	ranlib libip.a

$(OBJS): $(INCLUDE)/ip.h

inroute.b: $(INCLUDE)/custom.h redirtab.h

icmp.b: $(INCLUDE)/icmp.h

in_open.b in_write.b indemux.b: ipconn.h

lintlib:
	lint86 -Cip -DDEBUG ggp.c icmp.c in_alloc.c in_dump.c in_init.c \
	in_more.c in_mymach.c in_open.c in_write.c indemux.c inroute.c
	mv llib-lip.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

.a86.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libip.a
