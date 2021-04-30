# vim: ft=make

CC = gcc
CSTD = -std=gnu11
CWARN = -Wall -Wextra
COPT = -Os
CFLAGS = $(CSTD) $(CWARN) $(COPT)

TARGET = termtris
SOURCES = $(wildcard *.c)

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^
