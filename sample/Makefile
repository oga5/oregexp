SHELL=/bin/bash
CC=gcc
CFLAGS=-g -I../ostrutil -Wsign-compare -DTEST -D_DEBUG -DOSTRUTIL_UTF8 -DLINUX
LD=ld
LDFLAGS=-g
LIBS=

LIBRARY=../ostrutil/ostrutil.a

PROGRAMS=basic_match capture_groups string_replace callback_replace japanese_text custom_datasource_simple
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

custom_datasource_simple: custom_datasource_simple.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $< $(LIBRARY) $(LIBS)

$(LIBRARY):
	cd ../ostrutil && $(MAKE)

clean:
	rm -f $(PROGRAMS) $(OBJS)

# explicit dependencies
basic_match.o: basic_match.c ../ostrutil/oregexp.h ../ostrutil/local.h
capture_groups.o: capture_groups.c ../ostrutil/oregexp.h ../ostrutil/local.h
string_replace.o: string_replace.c ../ostrutil/oregexp.h ../ostrutil/local.h
callback_replace.o: callback_replace.c ../ostrutil/oregexp.h ../ostrutil/local.h
japanese_text.o: japanese_text.c ../ostrutil/oregexp.h ../ostrutil/local.h

# suffix
.SUFFIXES: .o .c

.c.o:
	$(CC) $(CFLAGS) -c $<
