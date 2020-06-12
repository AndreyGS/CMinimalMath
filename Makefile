CC=gcc
OBJECTS=./src/sldouble.c ./src/sldtestunit.c
OBJECTSGL=./src/sldouble.c ./src/sldtestunit_gl.c
CFLAGS=-g -Wall -Wextra -O3
LIBS=-lm
CFLAGSGL=$(CFLAGS) `pkg-config --cflags glib-2.0`
LIBSGL=`pkg-config --libs glib-2.0` -lm

sldtests: clean_mult
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o $@

glsldtests: clean_test_gl
	$(CC) $(CFLAGSGL) $(OBJECTSGL) $(LIBSGL) -o $@

clean_mult:
	rm -f sldtests

clean_test_gl:
	rm -f glsldtests

clean: clean_mult clean_test_gl