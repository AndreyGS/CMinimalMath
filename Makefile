CC=gcc
OBJECTS=./src/sldouble.c ./src/sldtestunit.c
OBJECTSGL=./src/sldouble.c ./src/sldtestunit_gl.c
CFLAGS=-Wall -Wextra
LIBS=-lm
CFLAGSGL=$(CFLAGS) `pkg-config --cflags glib-2.0`
LIBSGL=`pkg-config --libs glib-2.0` -lm

sldtests: clean_sldtests
	$(CC) $(CFLAGS) -O3 -fopenmp $(OBJECTS) $(LIBS) -o $@

sldtests_wo_omp: clean_sldtests
	$(CC) $(CFLAGS) -O3 $(OBJECTS) $(LIBS) -o sldtests

sldtests_db: clean_sldtests
	$(CC) $(CFLAGS) -g -O0 -fopenmp $(OBJECTS) $(LIBS) -o sldtests
	
sldtests_cov: clean_sldtests_cov
	$(CC) $(CFLAGS) -fprofile-arcs -ftest-coverage -O0 -fopenmp $(OBJECTS) $(LIBS) -o $@

glsldtests: clean_test_glsldtests
	$(CC) $(CFLAGSGL) -O3 $(OBJECTSGL) $(LIBSGL) -o $@
	
glsldtests_prof: clean_glsldtests_prof
	$(CC) $(CFLAGSGL) -pg -O0 $(OBJECTSGL) $(LIBSGL) -o $@
	
glsldtests_cov: clean_glsldtests_cov
	$(CC) $(CFLAGSGL) -fprofile-arcs -ftest-coverage -O0 $(OBJECTSGL) $(LIBSGL) -o $@

clean_sldtests:
	rm -f sldtests

clean_sldtests_cov:
	rm -f sldtests_cov sldtestunit.gcno sldouble.gcno sldtestunit.gcda sldouble.gcda sldtestunit.c.gcov sldouble.c.gcov
	
clean_glsldtests:
	rm -f glsldtests
	
clean_glsldtests_prof:
	rm -f glsldtests_prof gmon.out prof
	
clean_glsldtests_cov:
	rm -f glsldtests_cov sldtestunit_gl.gcno sldouble.gcno sldtestunit_gl.gcda sldouble.gcda sldtestunit_gl.c.gcov sldouble.c.gcov

clean: clean_sldtests clean_sldtests_cov clean_glsldtests clean_glsldtests_prof clean_glsldtests_cov
