LIBS  =
CFLAGS = -Wall

SRC=$(wildcard *.c)

tcp_client: $(SRC)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

udp_client: $(SRC)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)