include ..\nhowto.mak

LIB = ../../lib
OBJS = getopt.b _prints.b gencpy.b chirp.b io.b crock.b q.b \
	signal.b dosl.b mkraw.b ffirst.b crit_err.b int.b seg.b

#OBJS = getopt.b gencpy.b getenv.b chirp.b io.b crock.b \
#	signal.b dosl.b mkraw.b ffirst.b crit_err.b

libpc.a: $(OBJS)
	ar r libpc.a $(OBJS)
	ranlib libpc.a

lintlib:
	lint86 -Cc random.c signal.c time.c lint.c
	mv llib-lc.ln $(LIB)

#.c.b:
#	cc86 -c $*.c

arith.b:      
	a86 $*
chirp.b:      
	a86 $*
crit_err.b:   
	a86 $*
crock.b:      
	a86 $*
dosl.b:       
	a86 $*
ffirst.b:     
	a86 $*
gencpy.b:     
	a86 $*
getenv.b:
	a86 $*
int.b:        
	a86 $*
io.b:         
	a86 $*
mkraw.b:      
	a86 $*
seg.b:
	a86 $*

clean:
	@del $(OBJS)
	@del libpc.a