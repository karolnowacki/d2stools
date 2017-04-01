CC = gcc
CFLAGS = -Wall -O2 $(DEBUG)

all: lib tools

debug: 
	$(MAKE) $(MAKEFILE) DEBUG=1 
	
clean: 
	$(MAKE) -C src/lib clean
	$(MAKE) -C src/tools clean

lib:
	$(MAKE) -C src/lib
	
tools:
	$(MAKE) -C src/tools