
all: libc.a crt0.b


libc.a:
	cd crt& nmake
	cd gen& nmake
	cd sys& nmake
	cd stdio& nmake
	ranlib libc.a

crt0.b: crt0.a86
        a86 crt0

clean:
	cd crt& nmake clean
	cd sys& nmake clean
	cd gen& nmake clean
	cd stdio& nmake clean
	@del libc.a crt0.b
