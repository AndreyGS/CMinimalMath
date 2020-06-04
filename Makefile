CC=gcc
OBJECTS=./src/sldouble.c ./src/sldtestunit.c
OBJECTSGL=./src/sldouble.c ./src/sldtestunit_gl.c
CFLAGS=-Wall -O3
CFLAGSGL=$(shell pkg-config --cflags glib-2.0)
LIBSGL=$(shell pkg-config --libs glib-2.0)

sldmult: clean_mult
	$(CC) $(CFLAGS) $(OBJECTS) -o $@
    
glmulttest: clean_test_gl
	$(CC) $(CFLAGSGL) $(OBJECTSGL) $(LIBSGL) -o $@

clean_mult:
	rm -f sldmult

clean_test_gl:
	rm -f glmulttest
    
clean: clean_mult clean_test_gl