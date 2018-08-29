
OBJS=finder.o finder_event.o finder_list.o finder_thread.o finder_search.o

CFLAGS=-O2 -Wall -Ifox -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=

finder.a: $(OBJS)
	$(AR) rvu finder.a $(OBJS)
	ranlib finder.a

clean:
	rm -f finder.a $(OBJS)

