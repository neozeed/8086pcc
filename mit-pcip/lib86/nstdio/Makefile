all:
	cd ../stdio; make
	cp ../stdio/*.b .
	rm doscan.b doprnt.b gcvt.b
	cc86 -c doscan.c
	cc86 -c doprnt.c
