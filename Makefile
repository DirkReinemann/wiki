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
