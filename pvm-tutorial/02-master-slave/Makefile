CC=gcc
CFLAGS=-Wall
LDFLAGS=-lpvm3

all: master slave
	mv master slave ${PVM_HOME}

clean:
	rm -rf master slave

master: master.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

slave: slave.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
