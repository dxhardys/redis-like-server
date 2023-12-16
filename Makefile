CC=gcc
CFLAGS=-Wall
LDFLAGS=-pthread -lcjson

all: server

server: list.o server.o hashmap.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o server
