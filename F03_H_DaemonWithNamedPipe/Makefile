# automatically generated makefile by ./create-make

CFLAGS=-Wall -g -O2 -std=gnu99 -I./include -L./lib

LIBS=-lpthread -lrt -litsky


all: main

clean:
	rm -f lib/libitsky.a  main

main: main.c lib/libitsky.a include/itskylib.h
	gcc $(CFLAGS) main.c $(LIBS) -o main



lib/libitsky.a:
	ar crs lib/libitsky.a
