include ..\nhowto.mak


LIB = ../../lib
INCLUDE = ../../include
OBJS = enet.b et_close.b et_stat.b et_arp.b et_init.b et_demux.t.b \
	et_send.b et_int.b ethint.b

WOBJS = enet.w et_close.b et_stat.w et_arp.b et_init.b et_demux.t.w \
	et_send.w et_int.w ethint.b


libether.a: $(OBJS)
	ar r libether.a $(OBJS)
	ranlib libether.a

libwether: $(WOBJS)
	ar r $(LIB)/libwether.a $(WOBJS)
	ranlib $(LIB)/libwether.a

$(OBJS) $(WOBJS): $(INCLUDE)/ether.h $(INCLUDE)/netbuf.h ether.h

et_int.w: $(INCLUDE)/match.h

lintlib:
	lint86 -Cether enet.c et_arp.c et_close.c et_demux.t.c et_init.c \
	et_int.c et_send.c et_stat.c lint.c
	mv llib-lether.ln $(LIB)
	lint86 -Cwether -DWATCH enet.c et_arp.c et_close.c et_demux.t.c \
	et_init.c et_int.c et_send.c et_stat.c lint.c
	mv llib-lwether.ln $(LIB)


.c.w:
	mv $*.b $*.save
	cc86 -c -DDEBUG -DWATCH $*.c
	mv $*.b $*.w
	mv $*.save $*.b

ethint.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libether.a
