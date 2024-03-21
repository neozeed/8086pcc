# Description to build the network customizer
include ..\howto.mak



LIB = \pcc\lib
OBJ = ../../obj
INCLUDE = ../../include
OBJS = custom.b menu.b menu_def.b res.b
LIBS = -L$(LIB) -lh19 -lpc -lnet $(LIB)\netlibc.a

all: custom.com

install: custom.com
#	mv custom.com $(OBJ)

custom.com: $(OBJS)
	$(LD) $(LDFLAGS) -o custom.out $(LIB)\netcrt.b $(OBJS) $(LIBS)
	$(CVT) custom.out custom.com
 
#custom.b: custom.c menu.h $(INCLUDE)/custom.h

menu_def.b: menu_def.c menu.h $(INCLUDE)/attrib.h

menu.b: menu.c menu.h $(INCLUDE)/attrib.h

lint:
	lint86 custom.c menu.c menu_def.c res.c -lh19 -lnet -lc -lpc


.a86.b:
	a86 $*

clean:
	@del $(OBJS)
	@del custom.com custom.out
