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

all: static

debug: 
	$(MAKE) $(MAKEFILE) DEBUG=1
	
clean: 
	rm -f *.o *.a
	
static: d2slib.h d2slib.c d2s_constans
	$(CC) $(CFLAGS) -c d2slib.c -o d2slib.o
	ar rcs d2slib.a d2slib.o d2s_misc_desc.o d2s_armors_desc.o d2s_weapons_desc.o d2s_property_table.o
	
d2s_constans: d2s_constans.h d2s_misc_desc.c d2s_armors_desc.c d2s_weapons_desc.c d2s_property_table.c
	$(CC) $(CFLAGS) -c d2s_misc_desc.c 
	$(CC) $(CFLAGS) -c d2s_armors_desc.c 
	$(CC) $(CFLAGS) -c d2s_weapons_desc.c 
	$(CC) $(CFLAGS) -c d2s_property_table.c

	