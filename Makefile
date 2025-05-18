SHELL=/bin/bash
CC=gcc
CFLAGS=-g -Iostrutil -Wsign-compare -DTEST -D_DEBUG -DOSTRUTIL_UTF8 -DLINUX
LD=ld
LDFLAGS=-g
LIBS=
RCC=$(CC)

LIBRARY=ostrutil/ostrutil.a

PROGRAM1=regtest

OBJS1=regtest.o \
	$(LIBRARY)

GBFILES= $(PROGRAM1) \
	$(OBJS1) \
	core

#
all: $(PROGRAM1) samples

$(LIBRARY):
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

# Make regtest depend on test files
regtest.o: test/perl_re_tests_utf8.h test/perl_re_tests_euc.h test/org_re_tests_utf8.h test/org_re_tests_euc.h

test: $(PROGRAM1)
	$(ECHO) ./$(PROGRAM1)

$(PROGRAM1): $(OBJS1) regtest_utf8.h regtest_euc.h
	$(ECHO) $(CC) $(LDFLAGS) -o $(PROGRAM1) $(OBJS1) $(LIBRARY)

samples:
	$(MAKE) -C sample

clean:
	cd ./ostrutil; make clean
	cd ./sample; make clean
	-rm -f $(GBFILES)
	-rm -f test/*_re_tests_utf8.h test/*_re_tests_euc.h

depend:
	makedepend --$(CFLAGS) *.c

# explicit dependencies
regtest.o: regtest.c regtest_utf8.h regtest_euc.h

# suffix
.SUFFIXES: .o .c

.c.o:
	$(RCC) $(CFLAGS) -c $*.c


