
all: libc.a nlibc.a crt0.b

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


clean:
	cd crt& nmake clean
	cd sys& nmake clean
	cd gen& nmake clean
	cd stdio& nmake clean
	cd ncrt& nmake clean
	cd nsys& nmake clean
	cd ngen& nmake clean
	cd nstdio& nmake clean

	@del libc.a nlibc.a crt0.b
