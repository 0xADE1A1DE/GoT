CC=clang++
ECC=em++

IDIR =../include
BINDIR=../build
CFLAGS=-I$(IDIR) -O3 -g -lm -ffast-math -std=c++20

out: $(BINDIR)/main
wasm: $(BINDIR)/main.html
all: out wasm

$(BINDIR)/main: $(wildcard *.cc ev/* gates/* amplification/* experiments/* retention/*)
	$(CC) -o $@$(SWITCH_SIZE) $^ $(CFLAGS) -DSWITCH_SIZE=$(SWITCH_SIZE)

$(BINDIR)/main.html: $(wildcard *.cc ev/* gates/* amplification/* experiments/* retention/*)
	$(ECC) -DWASM -DREDUCE_DBG -o $@ $^ $(CFLAGS) -s ASSERTIONS=1 -s INITIAL_MEMORY=300mb --shell-file custom_shell.html

.PHONY: clean

clean:
	rm -f $(BINDIR)/main