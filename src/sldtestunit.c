#include <stdio.h>      // printf
#include <inttypes.h>   // uint64_t and int64_t vars displaying on printf
                        // currently unused
#include <stdlib.h>     // atoi atof
#include <math.h>
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
void test_sqrt(int acc);
void test_pow();
void test_speed_soft(void);
void test_speed_fpu(void);
double get_d_pseudo_random(int i);
int strcmp_max(char *s1, char *s2, int max);

int main(int argc, char **argv)
{
    char argserror[] = "Usage:\n"
                       "-------\n"
                       "./sldmult   mult | sqrt | fp | ip | div | pow   OPTIONS\n\n"
                       "OPTIONS:\n"
                       "mult: -b [acc] | factor1(double) factor2(double) | -f | -s\n"
                       "\t-b - big accuracy test of multiplication with default accuracy 12 digits\n"
                       "\t\tafter decimal point or with acc but not greater than 16\n"
                       "\tfactor1 factor2 - multiplication of 2 arbitary numbers\n"
                       "\t-f - fpu multiplication speed test (it's here for historical reasons)\n"
                       "\t-s - soft multiplication speed test (it's here for historical reasons)\n\n"
                       "sqrt: -b [acc] | number(double)\n"
                       "\t-b - big accuracy test of square root with default accuracy 12 digits\n"
                       "\t\tafter decimal point or with [acc] but not greater than 16\n"
                       "\tnumber - arbitary number for square root test\n\n"
                       "fp: number(double) power(double)\n"
                       "\tnumber power - returns number raised to fraction part of given power\n\n"
                       "ip: number(double) power(double)\n"
                       "\tnumber power - returns number raised to integer part of given power\n\n"
                       "div: dividend(double) divisor(double)\n"
                       "\tdividend divisor - returns quotient of input\n\n"
                       "pow: -b | number(double) power(double)\n"
                       "\t-b - big accuracy test without any limitations of accuracy\n"
                       "\tnumber power - returns number raised to the given power\n\n"
                       "P.S.: All big tests are include 3,200,000 operations for fpu "
                       "and just as much for soft\n\n"
                       "P.P.S.: Currently the only functions that have checkings on input "
                       "values are mult, sqrt and pow. They properly hold any legal "
                       "double value in contrast to fp, ip and div";
    double a,b;
    int i;
    
    struct timeval tim1, tim2;
    gettimeofday(&tim1,NULL);
    
    if (argc < 2) {
        printf("%s", argserror);
        return 1;
    }
    else {
        if (!strcmp_max("mult", *++argv, 5)) {
            if (argc > 2) {
                if (!strcmp_max("-f", *++argv, 3)) test_speed_fpu();
                else if (!strcmp_max("-s", *argv, 3)) test_speed_soft();
                else if (!strcmp_max("-b", *argv, 3)){
                    if (argc > 3 && (i = atoi(*++argv)))
                        test_mult(i);
                    else
                        test_mult(12);
                }
                else if (argc > 3) {
                    a = atof(*argv);
                    b = atof(*++argv);

                    printf("sldout: %.16e\nfpuout: %.16e\n", mult_by_sd(a, b), a*b);
                    return 0;
                } 
                else {
                    printf("%s", argserror);
                    return 1;
                }   
            } else {
                printf("%s", argserror);
                return 1;
            }
        } else if (!strcmp_max("sqrt", *argv, 5)) {
            if (argc > 2) {
                if (!strcmp_max("-b", *++argv, 3)){
                    if (argc > 3 && (i = atoi(*++argv)))
                        test_sqrt(i);
                    else
                        test_sqrt(12);
                }
                else {
                    a = atof(*argv);

                    printf("sldout: %.16e\nfpuout: %.16e\n", sqrt_by_sd(a), sqrt(a));
                    return 0;
                }
            } else {
                printf("%s", argserror);
                return 1;
            }
        } else if (!strcmp_max("fp", *argv, 3)) {
            if (argc > 2) {
                if (argc > 3) {
                    a = atof(*++argv);
                    b = atof(*++argv);

                    printf("sldout: %.16e\nfpuout: %.16e\n", fract_power_by_sd(a, b), pow(a,b));
                    return 0;
                } 
                else {
                    printf("%s", argserror);
                    return 1;
                }   
            } else {
                printf("%s", argserror);
                return 1;
            }
        } else if (!strcmp_max("ip", *argv, 3)) {
            if (argc > 2) {
                if (argc > 3) {
                    a = atof(*++argv);
                    b = atof(*++argv);

                    printf("sldout: %.16e\nfpuout: %.16e\n", int_power_by_sd(a, b), pow(a,b));
                    return 0;
                } 
                else {
                    printf("%s", argserror);
                    return 1;
                }   
            } else {
                printf("%s", argserror);
                return 1;
            }
        } else if (!strcmp_max("div", *argv, 3)) {
            if (argc > 2) {
                if (argc > 3) {
                    a = atof(*++argv);
                    b = atof(*++argv);

                    printf("sldout: %.16e\nfpuout: %.16e\n", division_by_sd(a, b), a/b);
                    return 0;
                } 
                else {
                    printf("%s", argserror);
                    return 1;
                }   
            } else {
                printf("%s", argserror);
                return 1;
            }
        } else if (!strcmp_max("pow", *argv, 3)) {
            if (argc > 2) {
                if (!strcmp_max("-b", *++argv, 3)){
                    test_pow();
                }
                else if (argc > 3) {
                    a = atof(*argv);
                    b = atof(*++argv);

                    printf("sldout: %.16e\nfpuout: %.16e\n", pow_by_sd(a, b), pow(a,b));
                    return 0;
                } 
                else {
                    printf("%s", argserror);
                    return 1;
                }   
            } else {
                printf("%s", argserror);
                return 1;
            }
        }
    }
    gettimeofday(&tim2,NULL);
    printf("%ld\n", tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec - tim1.tv_sec) * 1000000);
}

#define test_macro_start                                        \
    if (acc > 16) acc = 16;                                     \
    int i, j, counter = 0;                                      \
    double factor1 = 1.0e+200,                                  \
           factor2,                                             \
           r1, r2;                                              \
    for (i = 0; i < 400; ++i) {                                 \
        factor1 = factor1 / 10;                                 \
        printf("***\n%.2e power random factor\n\n", factor1);   \
        factor2 = 1.0e+200;                                     \
        for (j = 0; j < 8000; ++j) {                            \
            if (j % 50 == 0) factor2 = factor2 / 10;
            
#define test_macro_middle                                                       \
            if (r1 != r2 && (r1 == r1 && r2 == r2)) {                           \
                if (!(r1 == 0.9999999999999999 && r2 == 1.0)                    \
                 && !(r1 == 1.0 && r2 == 0.9999999999999999)                    \
                 && !(r1 == 1.0000000000000002 && r2 == 1.0)                    \
                 && !(r1 == 1.0 && r2 == 1.0000000000000002)) {                 \
                    sprintf(a1, "%.20e", r1);                                   \
                    sprintf(a2, "%.20e", r2);                                   \
                    if (r1 > 0 && r2 > 0) {                                     \
                        r = strcmp_max(a1, a2, acc+2);                          \
                    } else if (r1 < 0 && r2 < 0) {                              \
                        r = strcmp_max(a1, a2, acc+3);                          \
                    } else {                                                    \
                        r = -1;                                                 \
                    }
                    
void test_mult(int acc)
{
    double d1, d2;
    int r;
    char a1[29];
    char a2[29];
    
    test_macro_start
            
            d1 = get_d_pseudo_random(j % 16) * factor1;
            d2 = get_d_pseudo_random((i+j+31) % 16) * factor2;
            
            if (j % 2 == 0) d1 = -d1;
            if (j % 3 == 0) d2 = -d2;
            
            r1 = mult_by_sd(d1, d2);
            r2 = d1*d2;
            
    test_macro_middle        
                    if (r) {
                        printf("in1: %.20e in2: %.20e\nsld: %.20e\nfpu: %.20e\n"
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

void test_pow()
{
    int acc = 16; // macro dummy
    double d1, d2;
    
    test_macro_start
            
            d1 = get_d_pseudo_random(j % 16) * factor1;
            d2 = get_d_pseudo_random((i+j+31) % 16) * factor2;
            
            if (j % 2 == 0) d1 = -d1;
            if (j % 3 == 0) d2 = -d2;
            
            r1 = pow_by_sd(d1, d2);
            r2 = pow(d1,d2);
            if (r1 != r2 && r1 == r1) {
                printf("in1: %.20e in2: %.20e\nsld: %.20e\nfpu: %.20e\n"
                               "\n------------------------------\n",            
                                d1, d2, r1, r2);                                
                        counter++;  
            }
        }
    }
    
    
    printf("\n%d outputs in real number pow test "
           "that have missed accuracy\n", counter);
}

void test_sqrt(int acc)
{
    double d;
    int r;
    char a1[29];
    char a2[29];
    
    test_macro_start
            
            d = get_d_pseudo_random(j % 16) * factor1 * factor2;
            
            r1 = sqrt_by_sd(d);
            r2 = sqrt(d);
              
    test_macro_middle
                    if (r) {
                        printf("in: %.20e\nsld: %.20e\nfpu: %.20e\n"
                               "\n------------------------------\n",
                                d, r1, r2);
                        counter++;
                    }
                }
            }
        }
    }
    
    printf("\n%d outputs in square root test "
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