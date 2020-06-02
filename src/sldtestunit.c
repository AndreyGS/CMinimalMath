#include <stdio.h>      // printf
#include <inttypes.h>   // uint64_t and int64_t vars displaying on printf
                        // currently unused
#include <stdlib.h>     // atoi atof
#include "sldouble.h"

#ifdef _WIN32
    #include <Windows.h>

    int gettimeofday(struct timeval * tp, struct timezone * tzp)
    {
        static const uint64_t epoch = (uint64_t) 116444736000000000;
        uint64_t time;

        static SYSTEMTIME st;
        static FILETIME ft;
        
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
        time =  (uint64_t) ft.dwLowDateTime;
        time += ((uint64_t) ft.dwHighDateTime) << 32;

        tp->tv_sec  = (long) ((time - epoch) / 10000000);
        /* Despite the fact that time in FILETIME struct is measured 
         * in 1/10 microseconds, 4 least significant decimal digits 
         * are always returns with zero value, so we are
         * using milliseconds multiplied by 1000
         * only for conformity with relative POSIX function */
        tp->tv_usec = (long) (st.wMilliseconds * 1000);
        return 0;
    }
#else
    #include <sys/time.h>
#endif

void test_mult(int acc);
void test_speed_soft(void);
void test_speed_fpu(void);
double get_d_pseudo_random(int i);
int strcmp_max(char *s1, char *s2, int max);

int main(int argc, char **argv)
{
    char argserror[] = "Usage:\n-------\nsldmult\t[big_test_accuracy(int)]\n"
                    "\t|| [-f] || [-s]\n"
                    "\t|| [[factor1(double) factor2(double)]]\n\n"
                    "[big_test_accuracy] - test for 800000 multiplications "
                    "with accuracy as integral with maximum "
                    "of 16 digits after decimal point\n"
                    "[-f] - fpu speed test\n[-s] - sldmult speed test\n"
                    "[factor1 factor2] - test of arbitary numbers "
                    "multimplication\n"
                    "without parameters (default) - same as ./sldmult 12\n\n";
    if (argc > 2) {
        double a,b;
        if (!(a = atof(*++argv)) || !(b = atof(*++argv))) {
            printf("%s", argserror);
            return 1;
        } else {
            printf("sldout: %e\nfpuout: %e\n", mult_by_sd(a, b), a*b);
        }
    } else {
        int i;
        struct timeval tim1, tim2;
        if (argc == 2) {
            if (**++argv == '-') {
                if (*((*argv)+1) == 'f') {
                    gettimeofday(&tim1,NULL);
                    test_speed_fpu();
                    gettimeofday(&tim2,NULL);
                    printf("%ld\n", 
                        tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec - tim1.tv_sec) * 1000000);

                    return 0;
                } else if (*((*argv)+1) == 's') {
                    gettimeofday(&tim1,NULL);
                    test_speed_soft();
                    gettimeofday(&tim2,NULL);
                    printf("%ld\n", 
                        tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec - tim1.tv_sec) * 1000000);

                    return 0;
                } else {
                    printf("%s", argserror);
                    return 1;
                }
            } else if (!(i = atoi(*argv))) {
                printf("%s", argserror);
                return 1;
            }
        } else
            i = 12;
        
        if (i > 16) i = 16;

        gettimeofday(&tim1,NULL);
        test_mult(i);
        gettimeofday(&tim2,NULL);
        printf("%ld\n", 
            tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec - tim1.tv_sec) * 1000000);
    }
}

void test_mult(int acc)
{
    int i, j, r, counter = 0;
    double factor1 = 1.0e+200,
           factor2,
           d1, d2,
           r1, r2;
          
    char a1[25];
    char a2[25];
    
    for (i = 0; i < 400; ++i) {
		factor1 = factor1 / 10; 
        
        printf("***\n%.2e power random factor\n\n", factor1);
        
		factor2 = 1.0e+200;
        for (j = 0; j < 2000; ++j) {
			if (j % 50 == 0) factor2 = factor2 / 10;
			
            d1 = get_d_pseudo_random(j % 16) * factor1;
            d2 = get_d_pseudo_random((i+j+31) % 16) * factor2;
            
            if (j % 2 == 0) d1 = -d1;
			if (j % 3 == 0) d2 = -d2;
			
            r1 = mult_by_sd(d1, d2);
            r2 = d1*d2;
            
            if (r1 != r2 && (r1 == r1 && r2 == r2)) {
                if (!(r1 == 0.9999999999999999 && r2 == 1.0) 
                 && !(r1 == 1.0 && r2 == 0.9999999999999999)
                 && !(r1 == 1.0000000000000002 && r2 == 1.0)
                 && !(r1 == 1.0 && r2 == 1.0000000000000002)) {
                    sprintf(a1, "%.16e", r1);
                    sprintf(a2, "%.16e", r2);
                    if (r1 > 0 && r2 > 0) {
                        r = strcmp_max(a1, a2, acc+2);
                    } else if (r1 < 0 && r2 < 0) {
                        r = strcmp_max(a1, a2, acc+3);
                    } else {
                        r = -1;
                    }
                    if (r) {
                        printf("in1: %.16e in2: %.16e\nr1: %.16e\nr2: %.16e\n"
                               "\n------------------------------\n", 
                                d1, d2, r1, r2);
                        counter++;
                    }
                }
            }
        }
    }
    
    printf("\n%d outputs in real number multiplication test "
           "that have missed accuracy\n", counter);
}

double get_d_pseudo_random(int i)
{
    static int s = 0;
    struct timeval tim;
    gettimeofday(&tim,NULL);
    s++;
    double d = tim.tv_usec/1000000.0 * ((s % 100000)+1) * ((i % 65536) + 1) / 6553600000.0;
    if (d >= 1) return d/2;
    else return d;
}

int strcmp_max(char *s1, char *s2, int max)
{
    while (--max >= 0) {
        if      (*s1 > *s2)     return  1;
        else if (*s1 < *s2)     return -1;
        else if (*s1 == '\0')   return  0;
        
        s1++;
        s2++;
    }
    
    return 0;
}

/* When profiling with gprof get_d_pseudo_random() function
 * calls inside test_speed_fpu() are get 66.6% of all time
 * that is 100ms long and without its expense we have
 * 33ms of time to compute 800000 products by fpu 
 * (on my test stand, of course) */
void test_speed_fpu(void)
{
    int i, j, r;
    double factor1 = 1.0e+200,
           factor2,
           d1, d2;
    
    /* Calculation of res is a workaround for optimizator */
    double res = 0.0;

    for (i = 0; i < 400; i++) {
        factor1 = factor1 / 10; 
        factor2 = 1.0e+200;
        for (j = 0; j < 2000; j++) {
            if (j % 50 == 0) factor2 = factor2 / 10;
            
            d1 = get_d_pseudo_random(j % 16) * factor1;
            d2 = get_d_pseudo_random((i+j+31) % 16) * factor2;
            
            if (j % 2 == 0) d1 = -d1;
            if (j % 3 == 0) d2 = -d2;

            r = d1*d2;
            res += r / (i * j + 1);
        }
    }
    printf("%e\n", res);
}

/* When profiling with gprof get_d_pseudo_random() function
 * calls inside test_speed_soft() are get 4.2% of all time
 * that is 600ms long and without its expense we have
 * 575ms of time to compute 800000 products through sldouble
 * (on my test stand, of course) */
/* What is intresting, is that here computings of
 * pseudorandom numbers were 2.5 times faster that
 * they were in the test_speed_fpu() case. Probably some of the
 * executions were going natively in parallel. */
void test_speed_soft(void)
{
    int i, j, r;
    double factor1 = 1.0e+200,
           factor2,
           d1, d2;
    
    /* Calculation of res is a workaround for optimizator */
    double res = 0.0;

    for (i = 0; i < 400; i++) {
        factor1 = factor1 / 10; 
        factor2 = 1.0e+200;
        for (j = 0; j < 2000; j++) {
            if (j % 50 == 0) factor2 = factor2 / 10;
            
            d1 = get_d_pseudo_random(j % 16) * factor1;
            d2 = get_d_pseudo_random((i+j+31) % 16) * factor2;
            
            if (j % 2 == 0) d1 = -d1;
            if (j % 3 == 0) d2 = -d2;
            
            r = mult_by_sd(d1, d2);
            res += r / (i * j + 1);
        }
    }
    printf("%e\n", res);
}