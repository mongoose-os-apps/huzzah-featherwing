TARGET = test
LIBS =
CC = gcc
CFLAGS = -g -Wall
INCLUDE = ../include

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
SRCS    = frozen/frozen.c
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(OBJECTS) $(LIBS) $(SRCS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

