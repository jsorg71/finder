
OBJS=finder_gui.o finder_gui_about.o finder_mainwindow.o

GTK2_CFLAGS=$(shell pkg-config gtk+-2.0 --cflags)
GTK2_LIBS=$(shell pkg-config gtk+-2.0 --libs)

CFLAGS=-O2 -g -Wall -Wextra -I.. $(GTK2_CFLAGS) -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=$(GTK2_LIBS) ../finder.a -lpthread

all:
	$(MAKE) -C ..
	$(MAKE) finder

finder: $(OBJS) ../finder.a
	$(CC) -o finder $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	$(MAKE) -C .. clean
	rm -f finder $(OBJS)

%.o: %.c *.h ../finder.a
	$(CC) $(CFLAGS) -c $<

