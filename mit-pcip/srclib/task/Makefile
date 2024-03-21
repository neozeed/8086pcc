include ..\nhowto.mak

LIB = ../../lib

OBJS = timer.b task.b tk_util.b stack.b

libtask.a: $(OBJS)
	ar r libtask.a $(OBJS)
	ranlib libtask.a

lintlib:
	lint86 -Ctask stack.c task.c timer.c lint.c
	mv llib-ltask.ln $(LIB)

#.c.b:
#	cc86 -c -DDEBUG $*.c

tk_util.b:
	a86 $*


clean:
	@del $(OBJS)
	@del libtask.a
