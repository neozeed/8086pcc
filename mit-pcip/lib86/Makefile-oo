remake:
	- rm libc.a nlibc.a
	make all

all: libc.a nlibc.a crt0.b

libc.a:
	cd crt; make
	cd sys; make
	cd gen; make
	cd stdio; make
	ls crt/*.b gen/*.b sys/*.b stdio/*.b | sort -t/ +1 > t1
	ar cr libc.a `lorder \`cat t1\` | tsort`;
	ranlib libc.a
	rm -f t1

nlibc.a:
	cd ncrt; make
	cd sys; make
	cd ngen; make
	cd nstdio; make
	ls ncrt/*.b ngen/*.b sys/*.b nstdio/*.b | sort -t/ +1 > t1
	ar cr nlibc.a `lorder \`cat t1\` | tsort`;
	ranlib nlibc.a
	rm -f t1

crt0.b:	crt0.a86
	a86 crt0

