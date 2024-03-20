CFLAGS=-O

OBJ0 = trees.o optim.o pftn.o code.o local.o scan.o xdefs.o cgram.o
OBJ1 = reader.o order.o match.o allo.o

c86:	$(OBJ0) $(OBJ1) comm1.o local2.o table.o
	cc -o c86 $(OBJ0) $(OBJ1) comm1.o local2.o table.o

local2.o: local2.c
	cc -c -O -DX8087 local2.c

table.o: table.c
	cc -c -O -DX8087 table.c

nc86:	$(OBJ0) $(OBJ1) comm1.o nlocal2.o ntable.o
	cc -o nc86 $(OBJ0) $(OBJ1) comm1.o nlocal2.o ntable.o

nlocal2.o: local2.c
	cc -c -O local2.c
	mv local2.o nlocal2.o

ntable.o: table.c
	cc -c -O table.c
	mv table.o ntable.o

comm1.o: manifest mfile1 common macdefs
$(OBJ0): manifest macdefs mfile1
$(OBJ1) local2.o nlocal2.o table.o ntable.o: manifest mfile2 mac2defs macdefs
