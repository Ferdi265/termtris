# vim: ft=make

CC = gcc
CSTD = -std=gnu11
CWARN = -Wall -Wextra
COPT = -Os
CINC = -iquote include/
CFLAGS = $(CSTD) $(CWARN) $(COPT) $(CINC)

TARGET = termtris
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:%.c=%.o)

.PHONY: all debug sanitize clean

all: $(TARGET)

debug:
	$(MAKE) COPT="-Og -g"

sanitize:
	$(MAKE) COPT="-Og -g -fsanitize=address,undefined"

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)

$(OBJECTS): %.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
