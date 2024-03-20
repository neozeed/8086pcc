# File: Makefile (unix version)

CC=cl

OBJ0 = trees.obj optim.obj pftn.obj code.obj local.obj scan.obj xdefs.obj cgram.obj
OBJ1 = reader.obj order.obj match.obj allo.obj 


CFLAGS= -O
CPPFLAGS= 
LIBS =  LIBC.LIB KERNEL32.LIB

default: c86.exe nc86.exe

nc86.exe: $(OBJ0) $(OBJ1) comm1.obj nlocal2.obj ntable.obj
	link /NODEFAULTLIB:libc.lib /NODEFAULTLIB:OLDNAMES.LIB  -out:nc86.exe $(OBJ0) $(OBJ1) comm1.obj nlocal2.obj ntable.obj -entry:mainCRTStartup $(LIBS)

c86.exe: $(OBJ0) $(OBJ1) comm1.obj local7.obj table7.obj
	link /NODEFAULTLIB:libc.lib /NODEFAULTLIB:OLDNAMES.LIB  -out:c86.exe $(OBJ0) $(OBJ1) comm1.obj local7.obj table7.obj -entry:mainCRTStartup $(LIBS)


local7.obj: local2.c
	$(CC) $(CFLAGS) /DX8087 /c /Folocal7.obj local2.c

table7.obj: table.c
	$(CC) $(CFLAGS) /DX8087 /c /Fotable7.obj table.c

nlocal2.obj: local2.c
	$(CC) $(CFLAGS) /c /Fonlocal2.obj local2.c

ntable.obj: table.c
	$(CC) $(CFLAGS) /c /Fontable.obj table.c

%.obj: %.c
	$(CPP) $(CPPFLAGS) $< $*.i
	$(CC) $(CFLAGS) $(INCLUDE) -S -o $*.S $*.i
	$(AS) $*.S -o $*.obj


clean:
	@del /f c86.exe nc86.exe $(OBJ0) $(OBJ1) *.S *.i
	@del /f comm1.obj local7.obj nlocal2.obj ntable.obj table7.obj

dclean:
	@del /f *.obj *.S *.i



