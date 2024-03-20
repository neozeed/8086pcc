include ..\howto.mak

LIB = ../../lib

OBJS = v2net.b v2_close.b v2_init.b v2int.b v2_int.b v2_send.b \
	v2_stat.b v2_demux.t.b

libwv2.a: $(OBJS)
	ar r  libwv2.a $(OBJS)
	ranlib libwv2.a

lintlib:
	lint86 -Cv2 v2_close.c v2_demux.t.c v2_init.c v2_send.c v2_stat.c \
	v2_close.c v2net.c v2_int.c lint.c
	mv llib-lv2.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

v2int.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libwv2.a
