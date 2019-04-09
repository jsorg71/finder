
OBJS=finder_gui.o finder_gui_about.o

CXXFLAGS=-O2 -g -Wall -I.. `fox-config --cflags` -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=`fox-config --libs` ../finder.a -lpthread

all:
	$(MAKE) -C ..
	$(MAKE) finder

finder: $(OBJS) ../finder.a
	$(CXX) -o finder $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	$(MAKE) -C .. clean
	rm -f finder $(OBJS)

