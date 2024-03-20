include ..\howto.mak

LIB = ../../lib

OBJS = snet.b sl_close.b sl_stat.b sl_demux.t.b sl_init.b sl_rcv.b \
	sl_snd.b slah.b

libserial.a: $(OBJS)
	ar r libserial.a $(OBJS)
	ranlib libserial.a

lintlib:
	lint86 -Cserial sl_close.c sl_demux.t.c sl_init.c sl_rcv.c sl_snd.c \
	sl_stat.c snet.c lint.c
	mv llib-lserial.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

slah.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libserial.a
