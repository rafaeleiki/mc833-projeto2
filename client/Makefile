LIBS  =
CFLAGS = -Wall

SRC=$(wildcard *.c)
TCP=$(subst udp_client.c,,${SRC})
UDP=$(subst tcp_client.c,,${SRC})

tcp_client: $(TCP)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

udp_client: $(UDP)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
