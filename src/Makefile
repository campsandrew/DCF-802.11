CC = g++
CFLAGS = -g -Wall -std=c++11

all: run

run: main.o
	$(CC) -o $@ $^

main.o: main.cpp
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o run
