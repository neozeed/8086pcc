OBJ = cplus-dem.obj getopt.obj getopt1.obj ld.obj alloca.obj
CFLAGS = -I. -DUSG -DHZ=60

ld.exe: $(OBJ)
	link -out:ld.exe $(OBJ)


clean:
	@del $(OBJ) ld.exe
