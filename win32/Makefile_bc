
OBJS=finder_gui.obj

CFLAGS=-O2 -I..

LDFLAGS=

LIBS=

all: finder.exe

finder.exe: $(OBJS) ../finder.lib
	$(CC) -WM -W -efinder.exe $(OBJS) ../finder.lib

clean:
	del finder.exe $(OBJS)

.c.obj:
	$(CC) $(CFLAGS) -q -c $?
