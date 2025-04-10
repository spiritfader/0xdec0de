CC := gcc
CFLAGS := -g -O2 -Os -fdiagnostics-color=always
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti

make:
	$(CC) $(CFLAGS) main.c -o fsparse
