
OBJS=finder.o

CFLAGS=-O2 -Wall

LDFLAGS=

LIBS=fox/finder_gui.a `fox-config-1.6 --libs`

all: finder

finder: $(OBJS)
	$(CC) -o finder $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f finder $(OBJS)

