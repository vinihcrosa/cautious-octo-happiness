CFLAGS = -g -Wall -std=c99 -pedantic -fopenmp

all: main

main: main.o
	gcc $(CFLAGS) -o main main.o

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

clean:
	rm -f *.o main
