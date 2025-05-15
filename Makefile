SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Iostrutil -Wsign-compare -DTEST -D_DEBUG -DOSTRUTIL_EUC -DLINUX
LD=ld
LDFLAGS=-g
LIBS=
RCC=$(CC)

LIBRARY=ostrutil/ostrutil.a

PROGRAM1=regtest
SAMPLES=sample/basic_match sample/capture_groups sample/string_replace sample/callback_replace sample/japanese_text

OBJS1=regtest.o \
	$(LIBRARY)

GBFILES= $(PROGRAM1) \
	$(OBJS1) \
	$(SAMPLES) \
	core

#
all: $(PROGRAM1) samples

$(LIBRARY): ostrutil/*.c ostrutil/*.h
	cd ./ostrutil; make

regtest_euc.h: regtest_utf8.h
	iconv -f utf-8 -t eucjp -o regtest_euc.h regtest_utf8.h

test/perl_re_tests_utf8.h: test/make_re_test.pl test/perl_re_tests.txt
	cd test; ./make_re_test.pl perl_re_test_data < perl_re_tests.txt > perl_re_tests_utf8.h

test/perl_re_tests_euc.h: test/perl_re_tests_utf8.h
	cd test; iconv -f utf-8 -t eucjp -o perl_re_tests_euc.h perl_re_tests_utf8.h

test/org_re_tests_utf8.h: test/make_re_test.pl test/org_re_tests.txt
	cd test; ./make_re_test.pl org_re_test_data < org_re_tests.txt > org_re_tests_utf8.h

test/org_re_tests_euc.h: test/org_re_tests_utf8.h
	cd test; iconv -f utf-8 -t eucjp -o org_re_tests_euc.h org_re_tests_utf8.h

test: $(PROGRAM1)
	$(ECHO) ./$(PROGRAM1)

$(PROGRAM1): $(OBJS1)
	$(ECHO) $(CC) $(LDFLAGS) -o $(PROGRAM1) $(OBJS1) $(LIBRARY)

samples: $(SAMPLES)

sample/basic_match: sample/basic_match.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ sample/basic_match.o $(LIBRARY) $(LIBS)

sample/capture_groups: sample/capture_groups.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ sample/capture_groups.o $(LIBRARY) $(LIBS)

sample/string_replace: sample/string_replace.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ sample/string_replace.o $(LIBRARY) $(LIBS)

sample/callback_replace: sample/callback_replace.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ sample/callback_replace.o $(LIBRARY) $(LIBS)

sample/japanese_text: sample/japanese_text.o $(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ sample/japanese_text.o $(LIBRARY) $(LIBS)

clean:
	cd ./ostrutil; make clean
	-rm -f $(GBFILES)
	-rm -f sample/*.o

depend:
	makedepend --$(CFLAGS) *.c

# suffix
.SUFFIXES: .o .c

.c.o:
	$(RCC) $(CFLAGS) -c $*.c


