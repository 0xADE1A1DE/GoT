CC=gcc

IDIR =../include
BINDIR=../build
CFLAGS=-I$(IDIR) -O3 -g -lm

all: $(BINDIR)/main

$(BINDIR)/main: $(wildcard *.c ev/*.c gates/*.c amplification/* experiments/* measure/* retention/*)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(BINDIR)/main