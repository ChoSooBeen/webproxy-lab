CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: client

echoclient.o: echoclient.c ../csapp.h
	$(CC) $(CFLAGS) -c echoclient.c

client: echoclient.o ../csapp.o
	$(CC) $(CFLAGS) echoclient.o ../csapp.o -o client $(LDFLAGS)

clean:
	rm -f *~ *.o client