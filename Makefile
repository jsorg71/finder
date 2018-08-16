
OBJS=finder.o

CFLAGS=-O2 -Wall -Ifox -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=fox/finder_gui.a `fox-config-1.6 --libs` -lstdc++

all:
	$(MAKE) -C fox
	$(MAKE) finder

finder: $(OBJS) fox/finder_gui.a
	$(CC) -o finder $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	$(MAKE) -C fox clean
	rm -f finder $(OBJS)

