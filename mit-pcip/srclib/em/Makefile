include ..\nhowto.mak


H19OBJS = curse.b h19line.b em.b h19.b _wbyte.b h19key.b \
        scr_close.b kh.b sh.b alt.b

SUPOBJS = curse.b scr_close.b supdup.b _wbyte_sup.b kh.b sh.b

default: libh19.a libsup.a

libh19.a:	$(H19OBJS)
	ar cr libh19.a $(H19OBJS)
	ranlib libh19.a

libsup.a:	$(SUPOBJS)
	ar cr libsup.a $(SUPOBJS)
	ranlib libsup.a

alt.b:		alt.a86
		a86 alt

curse.b:	curse.a86
		a86 curse

em.b:	em.a86
		a86 em

kh.b:	kh.a86
		a86 kh

sh.b:	sh.a86
		a86 sh


clean:
	@del $(H19OBJS) $(SUPOBJS)
	@del libsup.a libh19.a
