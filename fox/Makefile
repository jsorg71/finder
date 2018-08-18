
OBJS=finder_gui.o

CXXFLAGS=-O2 -Wall -I.. `fox-config --cflags` -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=

all: finder_gui.a

finder_gui.a: $(OBJS)
	$(AR) rvu finder_gui.a $(OBJS)
	ranlib finder_gui.a

clean:
	rm -f finder_gui.a $(OBJS)

