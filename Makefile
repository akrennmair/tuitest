CC=gcc
CFLAGS=-g -fPIC -Wall -Wextra
LDFLAGS=
LIBS=-lutil -lncurses

DESTDIR=
prefix=/usr/local

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
	$(MAKE) -C swig clean && $(MAKE) -C swig LIBS+="$(patsubst %,../%,$(RUBYMODOBJS)) $(LIBS)"

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

install:
	mkdir -p $(DESTDIR)$(prefix)/bin
	install -m 755 tt-record $(DESTDIR)$(prefix)/bin
	$(MAKE) -C swig DESTDIR=$(DESTDIR) prefix=$(prefix) sitedir='$(DESTDIR)$(prefix)/lib/ruby' install

distclean clean:
	$(MAKE) -C swig clean
	$(RM) $(TARGET) $(RECORDOBJS) $(RUBYMOD) $(RUBYMODOBJS) swig/tuitest_wrap.c swig/Makefile

doc:
	rdoc

.PHONY: distclean clean all doc
