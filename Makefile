CC = gcc
CFLAGS = -Wall -O2 $(DEBUG)

all: itemexport d2stoxml

debug: 
	$(MAKE) $(MAKEFILE) DEBUG="-g -D DEBUG=1" 
clean: 
	rm -f *.o itemexport d2stoxml *.a

itemexport: itemexport.c d2slib
	$(CC) $(CFLAGS) itemexport.c d2slib.a -o itemexport
	
d2stoxml: d2stoxml.c d2slib
	$(CC) $(CFLAGS) d2stoxml.c d2slib.a -o d2stoxml

d2slib: d2slib.h d2slib.c d2s_constans
	$(CC) $(CFLAGS) -c d2slib.c -o d2slib.o
	ar rcs d2slib.a d2slib.o d2s_misc_desc.o d2s_armors_desc.o d2s_weapons_desc.o d2s_property_table.o
	
d2s_constans: d2s_constans.h d2s_misc_desc.c d2s_armors_desc.c d2s_weapons_desc.c d2s_property_table.c
	$(CC) $(CFLAGS) -c d2s_misc_desc.c 
	$(CC) $(CFLAGS) -c d2s_armors_desc.c 
	$(CC) $(CFLAGS) -c d2s_weapons_desc.c 
	$(CC) $(CFLAGS) -c d2s_property_table.c
	
	
	
