# File: Makefile (unix version)

CC=gcc
AS=ax386
CPP=cpp

OBJ0 = trees.obj optim.obj pftn.obj code.obj local.obj scan.obj xdefs.obj cgram.obj
OBJ1 = reader.obj order.obj match.obj allo.obj mktemp.obj


CFLAGS= -O
CPPFLAGS= -lang-c-c++-comments -nostdinc -I/xenixnt/h
LIBS = libgcc.lib LIBC.LIB KERNEL32.LIB

default: c86.exe nc86.exe

nc86.exe: $(OBJ0) $(OBJ1) comm1.obj nlocal2.obj ntable.obj
	link /NODEFAULTLIB:libc.lib /NODEFAULTLIB:OLDNAMES.LIB  -out:nc86.exe $(OBJ0) $(OBJ1) comm1.obj nlocal2.obj ntable.obj -entry:mainCRTStartup $(LIBS)

c86.exe: $(OBJ0) $(OBJ1) comm1.obj local2.obj table.obj
	link /NODEFAULTLIB:libc.lib /NODEFAULTLIB:OLDNAMES.LIB  -out:c86.exe $(OBJ0) $(OBJ1) comm1.obj local2.obj table.obj -entry:mainCRTStartup $(LIBS)

nlocal2.obj: local2.c
	$(CPP) $(CPPFLAGS) local2.c local2.i
	$(CC) $(CFLAGS) $(INCLUDE) -S -o local2.S local2.i
	$(AS) local2.S -o nlocal2.obj

ntable.obj: table.c
	$(CPP) $(CPPFLAGS) table.c table.i
	$(CC) $(CFLAGS) $(INCLUDE) -S -o table.S table.i
	$(AS) table.S -o ntable.obj

%.obj: %.c
	$(CPP) $(CPPFLAGS) $< $*.i
	$(CC) $(CFLAGS) $(INCLUDE) -S -o $*.S $*.i
	$(AS) $*.S -o $*.obj


clean:
	@del /f c86.exe nc86.exe $(OBJ0) $(OBJ1) *.S *.i
	@del /f comm1.obj     local2.obj    nlocal2.obj   ntable.obj    table.obj

dclean:
	@del /f *.obj *.S *.i




