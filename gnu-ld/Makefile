CC = gcc
OBJ = cplus-dem.o getopt.o getopt1.o ld.o alloca.o
CFLAGS = -I. -DUSG -DHZ=60

ld.exe: $(OBJ)
	$(CC) -o ld.exe $(OBJ)


clean:
	@del $(OBJ) ld.exe
