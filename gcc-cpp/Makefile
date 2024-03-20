
OBJS = cccp.obj version.obj cexp.obj


INC = /u /w /I.
OPT = /G3# /D__STDC__
DEBUG = #/Zi
LDEBUG = #-debug:full


.c.obj:
        $(CC) $(INC) $(OPT) $(DEBUG) /c $*.c


default: cpp.exe

cpp.exe: $(OBJS)
        link -out:cpp.exe cccp.obj cexp.obj version.obj

.asm.obj:
	ml /c $*.asm

clean:
	del $(OBJS)
	del cpp.exe

