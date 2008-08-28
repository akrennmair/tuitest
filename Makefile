CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
LDFLAGS=
LIBS=-lutil -lncurses

ROTESRC=$(wildcard rote/*.c)
RECORDSRC=tt-record.c lib.c record.c $(ROTESRC)
RUBYMODSRC=lib.o replay.o $(ROTESRC)

RECORDOBJS=$(patsubst %.c,%.o,$(RECORDSRC))
RUBYMODOBJS=$(patsubst %.c,%.o,$(RUBYMODSRC))

TARGET=tt-record
RUBYMOD=swig/tuitest.so

all: $(TARGET) $(RUBYMOD)

$(TARGET): $(RECORDOBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

$(RUBYMOD): $(RUBYMODOBJS)
	cd swig && swig -ruby tuitest.i && ruby extconf.rb
	$(MAKE) -C swig clean && $(MAKE) -C swig LIBS+="$(patsubst %,../%,$(RUBYMODOBJS)) -lncursesw -lutil"

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	$(RM) $(TARGET) $(RECORDOBJS) $(RUBYMOD) $(RUBYMODOBJS) swig/tuitest_wrap.c
	$(MAKE) -C swig clean

.PHONY: clean all
