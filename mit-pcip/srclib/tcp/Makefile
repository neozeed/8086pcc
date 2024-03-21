include ..\nhowto.mak

LIB = ../../lib

OBJS = tcp.b tcp_dump.b

libtcp.a: $(OBJS)
	ar r libtcp.a tcp.b tcp_dump.b 
	ranlib libtcp.a

lintlib:
	lint86 -Ctcp tcp.c tcp_dump.c
	mv llib-ltcp.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

.a86.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libtcp.a
