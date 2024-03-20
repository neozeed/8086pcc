
LIB = ../../lib
OBJ = ../../obj

netdev.sys: netdev.b
#	ld86 -X -N -r netdev.b
	ld -X -r netdev.b
	cvt86 -O0 a.out netdev.sys

all: netdev

#install:
#	mv netdev.sys ${OBJ}



netdev.b:
	a86 $*


clean:
	@del netdev.sys a.out netdev.b
