SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Wsign-compare -DTEST -D_DEBUG -DOSTRUTIL_UTF8 -DLINUX
#CFLAGS=-g -Wsign-compare -DTEST -D_DEBUG -D_UNICODE -DLINUX
LD=ld
LDFLAGS=-g
LIBS=
RCC=$(CC)

PROGRAM=ostrutil.a

OBJS_LIB= lexer.o \
	bm_search.o \
	parser.o \
	regexp.o \
	regexp_str.o \
	mbutil.o \
	replace.o
GBFILES= $(PROGRAM) \
	$(OBJS_LIB) \
	$(LIBRARY) \
	core

#
all: $(PROGRAM)

$(PROGRAM): $(OBJS_LIB)
	$(ECHO) ar rcv $(PROGRAM) $(OBJS_LIB)

clean:
	-rm -f $(GBFILES)

depend:
	makedepend --$(CFLAGS) *.c

# explicit dependencies
lexer.o: lexer.c local.h get_char.h
parser.o: parser.c local.h
regexp.o: regexp.c local.h
regexp_str.o: regexp_str.c local.h
mbutil.o: mbutil.c mbutil.h local.h
replace.o: replace.c local.h
bm_search.o: bm_search.c bm_search.h local.h

# suffix
.SUFFIXES: .o .c

.c.o:
	$(RCC) $(CFLAGS) -c $*.c

#
