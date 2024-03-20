include ..\howto.mak

LIB = ../../lib
INCLUDE = ../../include
OBJS = log.b res_name.b udp_init.b udp_name.b udp_time.b udp_alloc.b \
	udp_socket.b udp_write.b udpdemux.b udpswap.b udptable.b \
	udp_close.b udp_open.b

libudp.a: $(OBJS)
	ar r libudp.a $(OBJS)
	ranlib libudp.a

$(OBJS): $(INCLUDE)/udp.h

log.b udp_name.b udp_time.b: $(INCLUDE)/custom.h $(INCLUDE)/sockets.h

lintlib:
	lint86 -Cudp -DDEBUG log.c res_name.c udp_alloc.c udp_init.c \
	udp_name.c udp_open.c udp_socket.c udp_time.c udp_write.c udpdemux.c \
	udpswap.c udptable.c udp_close.c
	mv llib-ludp.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

.a86.b:
	a86 $*


clean:
	@del $(OBJS)
	@del libudp.a