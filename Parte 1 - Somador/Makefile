CC=gcc
CFLAGS=-Wall -Wextra -pthread -std=c99

all: somador

somador: somador.o
		$(CC) $(CFLAGS) -o somador somador.o

somador.o: somador.c
		$(CC) $(CFLAGS) -c somador.c

clean:
		rm -f *.o somador