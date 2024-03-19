# File: Makefile (unix version)

CC=gcc
AS=ax386
CPP=cpp

OBJ = error.o init.o ins.o ps.o rel.o sdi.o sym.o scan.o


CFLAGS= 
CPPFLAGS= -lang-c-c++-comments -nostdinc -I/xenixnt/h
LIBS = 

default: cvt86.exe as86.exe ld86.exe

as86.exe: $(OBJ)
	$(CC) -o as86.exe $(OBJ) $(LIBS)

cvt86.exe: cvt86.o
	$(CC) -o cvt86.exe cvt86.o $(LIBS)

ld86.exe: ld86.o
	$(CC) -o ld86.exe ld86.o $(LIBS)


nc86.exe: $(OBJ0) $(OBJ1) comm1.o nlocal2.o ntable.o
	$(CC) -o nc86.exe $(OBJ0) $(OBJ1) comm1.o nlocal2.o ntable.o $(LIBS)

c86.exe: $(OBJ0) $(OBJ1) comm1.o local2.o table.o
	$(CC) -o c86.exe $(OBJ0) $(OBJ1) comm1.o local2.o table.o $(LIBS)

nlocal2.o: local2.c
	$(CPP) $(CPPFLAGS) local2.c local2.i
	$(CC) $(CFLAGS) $(INCLUDE) -S -o local2.S local2.i
	$(AS) local2.S -o nlocal2.o

ntable.o: table.c
	$(CPP) $(CPPFLAGS) table.c table.i
	$(CC) $(CFLAGS) $(INCLUDE) -S -o table.S table.i
	$(AS) table.S -o ntable.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< 


clean:
	@del /f as86.exe cvt86.exe ld86.exe $(OBJ) *.o *.S *.i


dclean:
	@del /f *.o *.S *.i




