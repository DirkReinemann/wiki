CC=gcc
CFLAGS=-g -std=c11 -Wall -Werror -D_GNU_SOURCE
SOURCES=mongoose.c wiki.c
BIN=wiki

compile:
	$(CC) $(CFLAGS) $(SOURCES) -o $(BIN)

clean:
	-rm -f wiki

start:
	./wiki -s

render:
	./wiki -r

clean-html:
	-rm -rf html

install: compile
	-cp wiki.bin /usr/bin/wiki
	-cp wiki.service /etc/systemd/system
	-systemctl enable wiki.service
	-systemctl start wiki.service

uninstall:
	-systemctl stop wiki.service
	-systemctl disable wiki.service
	-rm -f /etc/systemd/system/wiki.service
	-rm -f /usr/bin/wiki
