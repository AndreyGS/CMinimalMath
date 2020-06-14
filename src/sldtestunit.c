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
void test_pow(int acc);
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
       "div:  dividend(double) divisor(double)\n"
       "      dividend divisor - returns quotient of input\n\n"
       "fp:   number(double) power(double)\n"
       "      number power - returns number raised to fraction part of given power\n\n"
       "ip:   number(double) power(double)\n"
       "      number power - returns number raised to integer part of given power\n\n"
       "mult: -b [precision] | factor1(double) factor2(double) | -f | -s\n"
       "      -b - big accuracy test of multiplication with default precision\n"
       "           12 digits after decimal point or with\n"
       "           selected precision  but not greater than 16\n"
       "      factor1 factor2 - multiplication of 2 selected numbers\n"
       "      -f - fpu multiplication speed test (it's here for historical reasons)\n"
       "      -s - soft multiplication speed test (it's here for historical reasons)\n\n"
       "pow:  -b [precision] | number(double) power(double)\n"
       "      -b - big accuracy test of raising number to given power \n"
       "           with default precision 12 digits after decimal point\n"
       "           or with selected precision  but not greater than 16\n"
       "      number power - returns number raised to the given power\n\n"
       "sqrt: -b [precision] | number(double)\n"
       "      -b - big accuracy test of square root with default precision 12 digits\n"
       "           after decimal point or with selected precision\n"
       "           but not greater than 16\n"
       "      number - selected number for square root test\n\n"
       "P.S.: All big tests are include 64,000,000 operations for fpu "
       "and just as much for soft. Also, if you wish to see where result "
       "between fpu and sldouble calculations would be different you need "
       "to run one of the big tests with precision 16 - that is now "
       "the only precision by which you will see that. And even with it, "
       "the total accuracy is greater that 99,99% in all tests.\n\n"
       "P.P.S.: Currently the only functions that have checkings on input "
       "values are mult, sqrt and pow. They properly hold any legal "
       "double value in contrast to fp, ip and div";
       
    double a,b,acc;
    
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
                    if (argc > 3 && (acc = atoi(*++argv)))
                        test_mult(acc);
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
                    if (argc > 3 && (acc = atoi(*++argv)))
                        test_sqrt(acc);
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
                    if (argc > 3 && (acc = atoi(*++argv)))
                        test_pow(acc);
                    else
                        test_pow(12);
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
    printf("Time elapsed: %ldns\n", tim2.tv_usec - tim1.tv_usec + (tim2.tv_sec - tim1.tv_sec) * 1000000);
}
            
void test_mult(int acc)
{
    if (acc > 16) acc = 16;   
    double accuracy = 1.0;
    for (int z = 0; z < acc; z++, accuracy /= 10.0);
    
    int counter = 0;
    double factor1 = 1.0e+200,
           factor2;

    for (int i = 0; i < 400; ++i) {                                 
        factor1 /= 10;
        printf("***\n%.2e first random factor\n\n", factor1);
        factor2 = 1.0e+200;
        for (int j = 0; j < 400; ++j) {
            
            #pragma omp parallel for \
                reduction(+:counter)
            for (int z = 0; z < 400; ++z) {
                double d1, d2, r1, r2, r;
                
                d1 = get_d_pseudo_random(z % 16) * factor1;
                d2 = get_d_pseudo_random((j+z+31) % 16) * factor2;
                
                if (z % 2 == 0) d1 = -d1;
                if (z % 3 == 0) d2 = -d2;
                
                r1 = mult_by_sd(d1, d2);
                r2 = d1*d2;
                
                if    (((r1 > 0 && r1 < r2) && ((r1 + r1 * accuracy) < r2))
                    || ((r1 < 0 && r1 > r2) && ((r1 + r1 * accuracy) > r2))
                    || ((r1 > 0 && r1 > r2) && ((r2 + r2 * accuracy) < r1))
                    || ((r1 < 0 && r1 < r2) && ((r2 + r2 * accuracy) > r1)))
                    r = 1;
                else
                    r = 0;

                if (r) {
                    printf("factor1: %.20e factor2: %.20e\nsld: %.20e\nfpu: %.20e\n"
                           "\n------------------------------\n",
                            d1, d2, r1, r2);
                    counter++;
                }
            }
            factor2 /= 10;
        }
    }
    
    printf("SUMMARY:\n--------\n"
           "Tests of multiplication two random numbers was made: 64,000,000\n"
           "Used precision: +-%.2e\n"
           "Outputs that had missed accuracy: %d\n"
           "Total accuracy: %.8g%%\n",
           accuracy, counter, 100 - (counter/64000000.0) * 100);
}

void test_pow(int acc)
{
    if (acc > 16) acc = 16;   
    double accuracy = 1.0;
    for (int z = 0; z < acc; z++, accuracy /= 10.0);
    
    int counter = 0;
    double factor1 = 1.0e+200,
           factor2;
    
    for (int i = 0; i < 400; ++i) {
        factor1 /= 10;
        printf("***\n%.2e number random factor\n\n", factor1);
        factor2 = 1.0e+200;
        for (int j = 0; j < 400; ++j) {
            
            #pragma omp parallel for \
                reduction(+:counter)
            for (int z = 0; z < 400; ++z) {
                double d1, d2, r1, r2, r;
                
                d1 = get_d_pseudo_random(z % 16) * factor1;
                d2 = get_d_pseudo_random((j+z+31) % 16) * factor2;
                
                if (z % 2 == 0) d1 = -d1;
                if (z % 3 == 0) d2 = -d2;
                
                r1 = pow_by_sd(d1, d2);
                r2 = pow(d1,d2);
                
                if    (((r1 > 0 && r1 < r2) && ((r1 + r1 * accuracy) < r2))
                    || ((r1 < 0 && r1 > r2) && ((r1 + r1 * accuracy) > r2))
                    || ((r1 > 0 && r1 > r2) && ((r2 + r2 * accuracy) < r1))
                    || ((r1 < 0 && r1 < r2) && ((r2 + r2 * accuracy) > r1))
                    || (r1 != r1 && r2 == r2) || (r1 == r1 && r2 != r2))
                    r = 1;
                else
                    r = 0;
                
                if (r) {
                    printf("number: %.20e power: %.20e\nsld: %.20e\nfpu: %.20e\n"
                                   "\n------------------------------\n",
                                    d1, d2, r1, r2);
                    counter++;
                }
            }
            factor2 /= 10;
        }
    }
    
    
    printf("SUMMARY:\n--------\n"
           "Tests of raising random number to random power was made: 64,000,000\n"
           "Used precision: +-%.2e\n"
           "Outputs that had missed accuracy: %d\n"
           "Total accuracy: %.8g%%\n",
           accuracy, counter, 100 - (counter/64000000.0) * 100);
}

void test_sqrt(int acc)
{
    if (acc > 16) acc = 16;   
    double accuracy = 1.0;
    for (int z = 0; z < acc; z++, accuracy /= 10.0);
    
    int counter = 0;
    double factor1 = 1.0e+200,
           factor2;

    for (int i = 0; i < 400; ++i) {                                 
        factor1 /= 10;
        printf("***\n%.2e first random factor\n\n", factor1);
        factor2 = 1.0e+200;
        for (int j = 0; j < 400; ++j) {
            
            #pragma omp parallel for \
                reduction(+:counter)
            for (int z = 0; z < 400; ++z) {
                double d, r1, r2, r;
                
                d = get_d_pseudo_random(z % 16) * factor1 * factor2;
            
                r1 = sqrt_by_sd(d);
                r2 = sqrt(d);
                
                if    (((r1 > 0 && r1 < r2) && ((r1 + r1 * accuracy) < r2))
                    || ((r1 < 0 && r1 > r2) && ((r1 + r1 * accuracy) > r2))
                    || ((r1 > 0 && r1 > r2) && ((r2 + r2 * accuracy) < r1))
                    || ((r1 < 0 && r1 < r2) && ((r2 + r2 * accuracy) > r1)))
                    r = 1;
                else
                    r = 0;

                if (r) {
                    printf("in: %.20e\nsld: %.20e\nfpu: %.20e\n"
                           "\n------------------------------\n",
                            d, r1, r2);
                    counter++;
                }
            }
            factor2 /= 10;
        }
    }
    
    printf("SUMMARY:\n--------\n"
           "Tests of extracting square root from random number was made: 64,000,000\n"
           "Used precision: +-%.2e\n"
           "Outputs that had missed accuracy: %d\n"
           "Total accuracy: %.8g%%\n",
           accuracy, counter, 100 - (counter/64000000.0) * 100);
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