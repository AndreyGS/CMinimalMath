#include <glib.h>
#include <stdio.h>
#include <math.h>
#include "sldouble.h"

double r1, r2;
double d1, d2;
char a1[25], a2[25];

/* Current macros was made with approach to emulate  
 * g_assert_cmpfloat_with_epsilon() if its missing 
 * on current glib version that is very likely */
#define comp(func1, Accuracy) \
    for (int i = 0; i++ < 10000;) { \
        d1 = g_test_rand_double(); \
        d2 = g_test_rand_double(); \
        r1 = func1(d1, d2); \
        if (func1 == pow_by_sd) \
            r2 = pow(d1, d2); \
        else \
            r2 = d1 * d2; \
        if (r1 > 0 && r1 < r2) { \
            r1 += r1 * Accuracy; \
            g_assert_cmpfloat(r1, >=, r2); \
        } else if (r1 < 0 && r1 > r2) { \
            r1 += r1 * Accuracy; \
            g_assert_cmpfloat(r1, <=, r2); \
        } else if (r1 > 0 && r1 > r2) { \
            r2 += r2 * Accuracy; \
            g_assert_cmpfloat(r2, >=, r1); \
        } else if (r1 < 0 && r1 < r2) { \
            r2 += r2 * Accuracy; \
            g_assert_cmpfloat(r2, <=, r1); \
        } else \
            g_assert_cmpfloat(r1, ==, r2); \
        if (g_test_failed()) { \
            printf("---------------\n" \
                   "Current input with accuracy %.1e leads to fail:\n" \
                   "%.16e * %.16e\nsld: %.16e\nfpu: %.16e\n%d operations " \
                   "were successfully executed in current test before\n" \
                   "---------------\n" \
                   , Accuracy, d1, d2, r1, r2, i); \
                break; \
        } \
    }

void test_01() {
    comp(mult_by_sd, 1.0e-12)
}

void test_02() {
    comp(mult_by_sd, 1.0e-13)
}

void test_03() {
    comp(mult_by_sd, 1.0e-14)
}

void test_04() {
    comp(mult_by_sd,1.0e-15)
}

void test_05() {
    comp(mult_by_sd, 1.0e-16)
}

void test_06() {
    r1 = mult_by_sd(1.0, 100.0);
    r2 = 1.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, 1.0);
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-1.0, 100.0);
    r2 = -1.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, -1.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = mult_by_sd(16.0, 100.0);
    r2 = 16.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, 16.0);
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-16.0, 100.0);
    r2 = -16.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, -16.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = mult_by_sd(0.5, 100.0);
    r2 = 0.5 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, 0.5);
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-0.5, 100.0);
    r2 = -0.5 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, -0.5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_07() {
    r1 = mult_by_sd(0.0, 100.0);
    r2 = 0.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-0.0, 100);
    r2 = -0.0 * 100.0;
    
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    r1 = mult_by_sd(100.0, -0.0);
    sprintf(a1, "%.16e", r1);
    g_assert_cmpstr(a1, ==, a2);
}

void test_08() {
    r1 = mult_by_sd(INFINITY, 100.0);
    r2 = INFINITY * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-INFINITY, 100.0);
    r2 = -INFINITY * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(100.0, -INFINITY);
    
    r1 = mult_by_sd(INFINITY, -INFINITY);
    r2 = INFINITY * -INFINITY;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-INFINITY, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(INFINITY, INFINITY);
    r2 = INFINITY * INFINITY;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-INFINITY, -INFINITY);
    r2 = -INFINITY * -INFINITY;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_09() {
    r1 = mult_by_sd(INFINITY, 0.0);
    r2 = INFINITY * 0.0;
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);

    r1 = mult_by_sd(0.0, INFINITY);
    r2 = 0.0 * INFINITY;
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
}

void test_10() {
    r1 = mult_by_sd(NAN, 100.0);
    r2 = NAN * 100.0;
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = mult_by_sd(100.0, NAN);
    r2 = 100.0 * NAN;
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
}

void test_11() {
    r1 = mult_by_sd(1.1125369292536007e-308, 1.9999999999999999);
    r2 = 1.1125369292536007e-308 * 1.9999999999999999;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_12() {
    r1 = mult_by_sd(1.0e-323, 0.5);
    r2 = 1.0e-323 * 0.5;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_13() {
    r1 = mult_by_sd(1.0e-323, 0.3);
    r2 = 1.0e-323 * 0.3;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_14() {
    r1 = mult_by_sd(1.0e-323, 0.2);
    r2 = 1.0e-323 * 0.2;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_15() {
    r1 = mult_by_sd(1.234523236e-310, 1.2256236236);
    r2 = 1.234523236e-310 * 1.2256236236;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_16() {
    r1 = pow_by_sd(-5, NAN);
    r2 = pow(-5, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-5, -NAN);
    r2 = pow(-5, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);

    r1 = pow_by_sd(-5, INFINITY);
    r2 = pow(-5, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, -INFINITY);
    r2 = pow(-5, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, 0.0);
    r2 = pow(-5, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, -0.0);
    r2 = pow(-5, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, 1);
    r2 = pow(-5, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, -1);
    r2 = pow(-5, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, 5);
    r2 = pow(-5, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-5, -5);
    r2 = pow(-5, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_17() {
    r1 = pow_by_sd(5, NAN);
    r2 = pow(5, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(5, -NAN);
    r2 = pow(5, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(5, INFINITY);
    r2 = pow(5, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, -INFINITY);
    r2 = pow(5, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, 0.0);
    r2 = pow(5, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, -0.0);
    r2 = pow(5, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, 1);
    r2 = pow(5, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, -1);
    r2 = pow(5, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, 5);
    r2 = pow(5, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(5, -5);
    r2 = pow(5, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_18() {
    r1 = pow_by_sd(-1, NAN);
    r2 = pow(-1, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-1, -NAN);
    r2 = pow(-1, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-1, INFINITY);
    r2 = pow(-1, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, -INFINITY);
    r2 = pow(-1, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, 0.0);
    r2 = pow(-1, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, -0.0);
    r2 = pow(-1, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, 1);
    r2 = pow(-1, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, -1);
    r2 = pow(-1, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, 5);
    r2 = pow(-1, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-1, -5);
    r2 = pow(-1, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_19() {
    r1 = pow_by_sd(1, NAN);
    r2 = pow(1, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(1, -NAN);
    r2 = pow(1, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(1, INFINITY);
    r2 = pow(1, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, -INFINITY);
    r2 = pow(1, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, 0.0);
    r2 = pow(1, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, -0.0);
    r2 = pow(1, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, 1);
    r2 = pow(1, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, -1);
    r2 = pow(1, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, 5);
    r2 = pow(1, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(1, -5);
    r2 = pow(1, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_20() {
    r1 = pow_by_sd(-0.0, NAN);
    r2 = pow(-0.0, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-0.0, -NAN);
    r2 = pow(-0.0, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-0.0, INFINITY);
    r2 = pow(-0.0, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, -INFINITY);
    r2 = pow(-0.0, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, 0.0);
    r2 = pow(-0.0, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, -0.0);
    r2 = pow(-0.0, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, 1);
    r2 = pow(-0.0, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, -1);
    r2 = pow(-0.0, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, -1.2);
    r2 = pow(-0.0, -1.2);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, 5);
    r2 = pow(-0.0, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-0.0, -5);
    r2 = pow(-0.0, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_21() {
    r1 = pow_by_sd(0.0, NAN);
    r2 = pow(0.0, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(0.0, -NAN);
    r2 = pow(0.0, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(0.0, INFINITY);
    r2 = pow(0.0, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, -INFINITY);
    r2 = pow(0.0, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, 0.0);
    r2 = pow(0.0, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, -0.0);
    r2 = pow(0.0, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, 1);
    r2 = pow(0.0, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, -1);
    r2 = pow(0.0, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, 5);
    r2 = pow(0.0, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(0.0, -5);
    r2 = pow(0.0, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_22() {
    r1 = pow_by_sd(-INFINITY, NAN);
    r2 = pow(-INFINITY, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-INFINITY, -NAN);
    r2 = pow(-INFINITY, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-INFINITY, INFINITY);
    r2 = pow(-INFINITY, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, -INFINITY);
    r2 = pow(-INFINITY, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, 0.0);
    r2 = pow(-INFINITY, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, -0.0);
    r2 = pow(-INFINITY, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, 1);
    r2 = pow(-INFINITY, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, -1);
    r2 = pow(-INFINITY, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, 5);
    r2 = pow(-INFINITY, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-INFINITY, -5);
    r2 = pow(-INFINITY, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_23() {
    r1 = pow_by_sd(INFINITY, NAN);
    r2 = pow(INFINITY, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(INFINITY, -NAN);
    r2 = pow(INFINITY, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(INFINITY, INFINITY);
    r2 = pow(INFINITY, INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, -INFINITY);
    r2 = pow(INFINITY, -INFINITY);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, 0.0);
    r2 = pow(INFINITY, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, -0.0);
    r2 = pow(INFINITY, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, 1);
    r2 = pow(INFINITY, 1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, -1);
    r2 = pow(INFINITY, -1);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, 5);
    r2 = pow(INFINITY, 5);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(INFINITY, -5);
    r2 = pow(INFINITY, -5);
    g_assert_cmpfloat(r1, ==, r2);
}

void test_24() {
    r1 = pow_by_sd(-NAN, NAN);
    r2 = pow(-NAN, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, -NAN);
    r2 = pow(-NAN, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, INFINITY);
    r2 = pow(-NAN, INFINITY);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, -INFINITY);
    r2 = pow(-NAN, -INFINITY);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, 0.0);
    r2 = pow(-NAN, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-NAN, -0.0);
    r2 = pow(-NAN, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(-NAN, 1);
    r2 = pow(-NAN, 1);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, -1);
    r2 = pow(-NAN, -1);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, 5);
    r2 = pow(-NAN, 5);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(-NAN, -5);
    r2 = pow(-NAN, -5);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
}

void test_25() {
    r1 = pow_by_sd(NAN, NAN);
    r2 = pow(NAN, NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, -NAN);
    r2 = pow(NAN, -NAN);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, INFINITY);
    r2 = pow(NAN, INFINITY);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, -INFINITY);
    r2 = pow(NAN, -INFINITY);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, 0.0);
    r2 = pow(NAN, 0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(NAN, -0.0);
    r2 = pow(NAN, -0.0);
    g_assert_cmpfloat(r1, ==, r2);
    
    r1 = pow_by_sd(NAN, 1);
    r2 = pow(NAN, 1);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, -1);
    r2 = pow(NAN, -1);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, 5);
    r2 = pow(NAN, 5);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
    
    r1 = pow_by_sd(NAN, -5);
    r2 = pow(NAN, -5);
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
}

void test_26() {
    comp(pow_by_sd, 1.0e-12)
}

void test_27() {
    comp(pow_by_sd, 1.0e-13)
}

void test_28() {
    comp(pow_by_sd, 1.0e-14)
}

void test_29() {
    comp(pow_by_sd, 1.0e-15)
}

void test_30() {
    comp(pow_by_sd, 1.0e-16)
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    
    g_test_set_nonfatal_assertions();
    g_test_add_func("/test_01/inner_mult/random/accuracy-12", test_01);
    g_test_add_func("/test_02/inner_mult/random/accuracy-13", test_02);
    g_test_add_func("/test_03/inner_mult/random/accuracy-14", test_03);
    g_test_add_func("/test_04/inner_mult/random/accuracy-15", test_04);
    g_test_add_func("/test_05/inner_mult/random/accuracy-16", test_05);
    g_test_add_func("/test_06/inner_mult/simplenums", test_07);

    g_test_add_func("/test_07/mult/special_cases/zeros", test_06);
    g_test_add_func("/test_08/mult/special_cases/infinity", test_08);
    g_test_add_func("/test_09/mult/special_cases/infinity_zero", test_09);
    g_test_add_func("/test_10/mult/special_cases/nan", test_10);

    g_test_add_func("/test_11/get_double_ieee754/denormal/overflow", test_11);
    g_test_add_func("/test_12/get_double_ieee754/denormal/rounding_before_the_edge", test_12);
    g_test_add_func("/test_13/get_double_ieee754/denormal/rounding_on_the_edge", test_13);
    g_test_add_func("/test_14/get_double_ieee754/denormal/rounding_over_the_edge", test_14);
    g_test_add_func("/test_15/get_double_ieee754/denormal/general_operation", test_15);
    
    g_test_add_func("/test_16/pow_by_sd/negpower", test_16);
    g_test_add_func("/test_17/pow_by_sd/pospower", test_17);
    g_test_add_func("/test_18/pow_by_sd/power == -1", test_18);
    g_test_add_func("/test_19/pow_by_sd/power == 1", test_19);
    g_test_add_func("/test_20/pow_by_sd/power == -0.0", test_20);
    g_test_add_func("/test_21/pow_by_sd/power == 0.0", test_21);
    g_test_add_func("/test_22/pow_by_sd/power == -INFINITY", test_22);
    g_test_add_func("/test_23/pow_by_sd/power == INFINITY", test_23);
    g_test_add_func("/test_24/pow_by_sd/power is -NAN", test_24);
    g_test_add_func("/test_25/pow_by_sd/power is NAN", test_25);
    
    g_test_add_func("/test_26/pow_by_sd/random/accuracy-12", test_26);
    g_test_add_func("/test_27/pow_by_sd/random/accuracy-13", test_27);
    g_test_add_func("/test_28/pow_by_sd/random/accuracy-14", test_28);
    g_test_add_func("/test_29/pow_by_sd/random/accuracy-15", test_29);
    g_test_add_func("/test_30/pow_by_sd/random/accuracy-16", test_30);
    
    return g_test_run();
}