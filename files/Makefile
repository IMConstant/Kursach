CC=g++ -std=gnu++14

all: paint

paint: Main.o Source.o
	$(CC) Main.o Source.o -o paint

Main.o: Main.cpp Header.h
	$(CC) -c Main.cpp

Source.o: Source.cpp Header.h
	$(CC) -c Source.cpp

clean:
	rm -rf *.o
