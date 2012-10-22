# Makefile written with the generous help of Aaron Hsu

OFILES = chase_lev.o
glib = `pkg-config --cflags --libs glib-2.0`
CFLAGS = -pthread $(glib) -Wall -std=c99 -O3

test: $(OFILES) test.c
	gcc $(CFLAGS) -o $@ $(OFILES) test.c

clean:
	rm -rf main.o main $(OFILES)


