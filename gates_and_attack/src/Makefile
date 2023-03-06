CC=clang++
ECC=em++

IDIR =../include
BINDIR=../build
CFLAGS=-I$(IDIR) -O3 -g -lm -ffast-math -std=c++20

out: $(BINDIR)/main
wasm: $(BINDIR)/main.html
all: out wasm

$(BINDIR)/main: $(wildcard *.cc ev/* gates/* amplification/* experiments/* )
	$(CC) -o $@ $^ $(CFLAGS)

$(BINDIR)/main.html: $(wildcard *.cc ev/* gates/* amplification/* experiments/* )
	$(ECC) -DWASM -o $@ $^ $(CFLAGS) -s ASSERTIONS=1 -s INITIAL_MEMORY=400mb --shell-file custom_shell.html

.PHONY: clean

clean:
	rm -f $(BINDIR)/main