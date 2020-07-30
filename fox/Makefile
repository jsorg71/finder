
OBJS=finder_gui.o finder_gui_about.o finder_mainwindow.o

FOX_CFLAGS=$(shell fox-config --cflags)
FOX_LIBS=$(shell fox-config --libs)

CXXFLAGS=-O2 -g -Wall -Wextra -I.. $(FOX_CFLAGS) -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=$(FOX_LIBS) ../finder.a -lpthread

all:
	$(MAKE) -C ..
	$(MAKE) finder

finder: $(OBJS) ../finder.a
	$(CXX) -o finder $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	$(MAKE) -C .. clean
	rm -f finder $(OBJS)

%.o: %.cpp *.h ../finder.a
	$(CXX) $(CXXFLAGS) -c $<

