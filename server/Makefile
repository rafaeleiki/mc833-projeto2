LIBS  =
CFLAGS = -Wall

SRC=$(wildcard *.c)
TCP=$(subst udp_server.c,,${SRC})
UDP=$(subst tcp_server.c,,${SRC})

tcp_server: $(TCP)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

udp_server: $(UDP)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)