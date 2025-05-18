SHELL=/bin/bash
CC=gcc
CFLAGS=-g -I../ostrutil -Wsign-compare -DTEST -D_DEBUG -DOSTRUTIL_UTF8 -DLINUX
LD=ld
LDFLAGS=-g
LIBS=

LIBRARY=../ostrutil/ostrutil.a

PROGRAMS=basic_match capture_groups string_replace callback_replace japanese_text
OBJS=$(PROGRAMS:=.o)

all: $(PROGRAMS)

basic_match: basic_match.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARY) $(LIBS)

capture_groups: capture_groups.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARY) $(LIBS)

string_replace: string_replace.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARY) $(LIBS)

callback_replace: callback_replace.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARY) $(LIBS)

japanese_text: japanese_text.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARY) $(LIBS)

$(LIBRARY):
	cd ../ostrutil && $(MAKE)

clean:
	rm -f $(PROGRAMS) $(OBJS)

# suffix
.SUFFIXES: .o .c

.c.o:
	$(CC) $(CFLAGS) -c $<
