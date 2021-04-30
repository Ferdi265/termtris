# vim: ft=make

CC = gcc
CSTD = -std=gnu11
CWARN = -Wall -Wextra
COPT = -Os
CINC = -Iinclude/
CFLAGS = $(CSTD) $(CWARN) $(COPT) $(CINC)

TARGET = termtris
SOURCES = $(wildcard src/*.c)

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^
