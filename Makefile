
OBJS=finder.o finder_event.o finder_list.o finder_thread.o finder_search.o

CFLAGS=-O2 -g -Wall -Wextra -fvisibility=hidden -fPIC

LDFLAGS=

LIBS=

finder.a: $(OBJS)
	$(AR) rvu finder.a $(OBJS)

clean:
	rm -f finder.a $(OBJS)

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $<

