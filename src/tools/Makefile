ifndef CC
CC = gcc
endif
ifndef CDBGFAGS
CDBGFAGS = -g -D DEBUG=1
endif
ifndef CFLAGS
CFLAGS = -Wall -O2
ifdef DEBUG
CFLAGS += $(CDBGFAGS)
endif
endif

all: d2stoxml d2stestwrite d2sshowinvmap d2sdropitem d2spickupitem d2itoxml

debug: 
	$(MAKE) $(MAKEFILE) DEBUG=1
	
clean: 
	rm -f *.o d2stoxml d2stestwrite d2sshowinvmap d2sdropitem d2spickupitem d2itoxml
	
d2stoxml: d2stoxml.c
	$(CC) $(CFLAGS) d2stoxml.c ../lib/d2slib.a -o d2stoxml
	
d2stestwrite: d2stestwrite.c
	$(CC) $(CFLAGS) d2stestwrite.c ../lib/d2slib.a -o d2stestwrite

d2sshowinvmap: d2sshowinvmap.c
	$(CC) $(CFLAGS) d2sshowinvmap.c ../lib/d2slib.a -o d2sshowinvmap
	
d2sdropitem: d2sdropitem.c
	$(CC) $(CFLAGS) d2sdropitem.c ../lib/d2slib.a -o d2sdropitem
	
d2spickupitem: d2spickupitem.c
	$(CC) $(CFLAGS) d2spickupitem.c ../lib/d2slib.a -o d2spickupitem
	
d2itoxml: d2itoxml.c
	$(CC) $(CFLAGS) d2itoxml.c ../lib/d2slib.a -o d2itoxml
