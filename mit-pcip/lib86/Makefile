# the mit lib is really non 8087
# also the network stuff uses it's on crt startup code
# pcrt & pnetcrt

LIBS=\pcc\lib

all: nlibc.a

libc.a:
	cd crt& nmake
	cd sys& nmake
	cd gen& nmake
	cd stdio& nmake
	ranlib libc.a

nlibc.a:
	cd ncrt& nmake
	cd nsys& nmake
	cd ngen& nmake
	cd nstdio& nmake
	ranlib nlibc.a


crt0.b:	crt0.a86
	a86 crt0

install: nlibc.a
#	copy /Y crt0.b $(LIBS)\crt0.b
#	copy /Y libc.a $(LIBS)\libc.a
	copy /Y nlibc.a $(LIBS)\netlibc.a

zclean:
	cd crt& nmake clean
	cd sys& nmake clean
	cd gen& nmake clean
	cd stdio& nmake clean
	@del libc.a crt0.b

clean:
	cd ncrt& nmake clean
	cd nsys& nmake clean
	cd ngen& nmake clean
	cd nstdio& nmake clean
	@del nlibc.a crt0.b
