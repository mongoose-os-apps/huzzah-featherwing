TARGET = test
LIBS =
CC = gcc
CFLAGS = -g -Wall
INCLUDE = ../include

.PHONY: default all clean

default: $(TARGET)
all: default

OBJ_WIDGET = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJ_WIDGET)

$(TARGET): $(OBJ_WIDGET)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(OBJ_WIDGET) $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)

