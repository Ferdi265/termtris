# vim: ft=make

CC = gcc
CSTD = -std=gnu11
CWARN = -Wall -Wextra
COPT = -Os
CINC = -Iinclude/
CFLAGS = $(CSTD) $(CWARN) $(COPT) $(CINC)

TARGET = termtris
SOURCES = $(wildcard src/*.c)

.PHONY: all debug clean

all: $(TARGET)

debug:
	$(MAKE) COPT="-Og -g"

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^
