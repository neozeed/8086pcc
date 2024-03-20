CPP = cpp
CC1 = c86
AS = a86
LD = ld
CVT = cvt86
EMUL = msdos


CPPFLAGS = -P -U -nostdinc -DIBMDOS -Uvax -Dibm -I$(INCLUDE)
LDFLAGS = -X -r

BROKEN = 

OBJ =	abandon.b attack.b autover.b capture.b cgetc.b checkcond.b check_out.b \
	compkl.b computer.b damage.b damaged.b dcrept.b destruct.b dock.b \
	dumpgame.b dumpme.b dumpssradio.b events.b externs.b getcodi.b getpar.b \
	help.b impulse.b initquad.b kill.b klmove.b lose.b lrscan.b \
	main.b move.b nova.b out.b phaser.b play.b ram.b \
	ranf.b rest.b schedule.b score.b setup.b setwarp.b shield.b \
	snova.b srscan.b systemname.b torped.b utility.b visual.b warp.b \
	win.b



trek.com: $(OBJ)
	$(LD) $(LDFLAGS) -o trek.out $(LIB)\crt0.b $(OBJ) $(LIB)\libc.a
	$(CVT) trek.out trek.com

.c.b:
 $(CPP) $(CPPFLAGS) $*.c -o $*.i
 $(CC1) < $*.i > $*.a86
	@del $*.i
 $(AS) $*.a86
	@del $*.a86

clean:
	@del /f trek.com trek.out $(OBJ)

dclean:
	@del /f $(OBJ) *.i *.a86

