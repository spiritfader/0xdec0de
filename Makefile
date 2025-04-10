CC := gcc
CFLAGS := -g -O2 -Os -fdiagnostics-color=always
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti

make: main.o fsinfo.o
	$(CC) $(CFLAGS) main.o fsinfo.o -o fsparse

main.o: main.c fsinfo.h
	$(CC) $(CFLAGS) -c main.c -o main.o

fsinfo.o: fsinfo.c fsinfo.h
	$(CC) $(CFLAGS) -c fsinfo.c -o fsinfo.o

clean:
	rm -f *.o
