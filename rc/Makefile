CC=gcc

TARGETS=rc

CFLAGS=-ggdb -I. `net-snmp-config --cflags`
BUILDLIBS=-lcsv -ggdb
BUILDAGENTLIBS=`net-snmp-config --agent-libs`

# shared library flags (assumes gcc)
DLFLAGS=-fPIC -shared

all:
	$(CC) $(BUILDLIBS) -o rc r_main.c algorithms.c r_lsdb.c r_lsa.c table.c prefix.c

clean: 
	rm rc
