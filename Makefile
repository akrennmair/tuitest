CC=gcc
CFLAGS=-g
LDFLAGS=
LIBS=-lutil -lncurses

ROTESRC=$(wildcard rote/*.c)
RECORDSRC=tt-record.c lib.c record.c $(ROTESRC)

RECORDOBJS=$(patsubst %.c,%.o,$(RECORDSRC))

TARGET=tt-record

all: $(TARGET)

$(TARGET): $(RECORDOBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	$(RM) $(TARGET) $(RECORDOBJS)

.PHONY: clean all
