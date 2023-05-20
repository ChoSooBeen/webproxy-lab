CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: server

echoserver.o: echoserver.c ../csapp.h
	$(CC) $(CFLAGS) -c echoserver.c

echo.o: echo.c ../csapp.h
	$(CC) $(CFLAGS) -c echo.c

server: echoserver.o echo.o ../csapp.o
	$(CC) $(CFLAGS) echoserver.o echo.o ../csapp.o -o server $(LDFLAGS)

clean:
	rm -f *~ *.o server