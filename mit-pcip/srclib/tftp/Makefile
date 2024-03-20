include ..\howto.mak

LIB = ../../lib

OBJS = tftp_use.b tftp_srv.b tftp_util.b

libtftp.a: $(OBJS)
	ar r libtftp.a $(OBJS)
	ranlib libtftp.a

lintlib:
	lint86 -Ctftp tftp_use.c tftp_srv.c tftp_util.c
	mv llib-ltftp.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

.a86.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libtftp.a
