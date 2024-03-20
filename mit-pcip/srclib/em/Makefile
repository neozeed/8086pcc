include ..\howto.mak

ASMOBJ = alt.b curse.b em.b kh.b sh.b

BROKEN = 
OBJECTS =	h19.b h19key.b h19line.b lint.b scr_close.b stdne.b supdup.b \
	_wbyte.b _wbyte_sup.b stinit.b


libsup.a:	$(OBJECTS) $(ASMOBJ)
	ar cr libsup.a $(OBJECTS) $(ASMOBJ)


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
	@del $(OBJECTS) $(ASMOBJ)
	@del libsup.a