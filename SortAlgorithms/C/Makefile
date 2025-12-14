CFLAGS = -g -Wall -std=c99 -pedantic -fopenmp

all: main

main: main.o utils.o sortAlgorithms.o
	gcc $(CFLAGS) -o main main.o utils.o sortAlgorithms.o

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

utils.o: utils.c
	gcc $(CFLAGS) -c utils.c -o utils.o

sortAlgorithms.o: sortAlgorithms.c
	gcc $(CFLAGS) -c sortAlgorithms.c -o sortAlgorithms.o

clean:
	rm -f *.o main
