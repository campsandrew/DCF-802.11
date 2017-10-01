CC = g++
CFLAGS = -Wall

all: run

run: main.o station.o 
	$(CC) -o $@ $^

main.o: main.cpp parameters.h
	$(CC) -c $(CFLAGS) $<

station.o: station.cpp station.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o run