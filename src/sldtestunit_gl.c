#include <glib.h>
#include <stdio.h>
#include <math.h>
#include "sldouble.h"

double r1, r2;
double d1, d2;

/* Current macros was made with approach to emulate  
 * g_assert_cmpfloat_with_epsilon() if its missing 
 * on current glib version that is very likely */
#define comp(Accuracy) \
    for (int i = 0; i < 10000; i++) { \
        d1 = g_test_rand_double(); \
        d2 = g_test_rand_double(); \
        r1 = mult_by_sd(d1, d2); \
        r2 = d1 * d2; \
        if ((r1 > 0 && r1 < r2) || (r1 < 0 && r1 > r2)) { \
            r1 += r1 * Accuracy; \
            g_assert_cmpfloat(r1, >=, r2); \
        } else if ((r1 > 0 && r1 > r2) || (r1 < 0 && r1 < r2)) { \
            r2 += r2 * Accuracy; \
            g_assert_cmpfloat(r2, >=, r1); \
        } else \
            g_assert_cmpfloat(r1, ==, r2); \
        if (g_test_failed()) { \
            printf("---------------\n" \
                   "Current input with accuracy %.1e leads to fail:\n" \
                   "%.16e * %.16e\nr1: %.16e\nr2: %.16e\n%d operations " \
                   "were successfully executed in current test before\n" \
                   "---------------\n" \
                   , Accuracy, d1, d2, r1, r2, i); \
                break; \
        } \
    }

void test_01() {
    comp(1.0e-12)
}

void test_02() {
    comp(1.0e-13)
}

void test_03() {
    comp(1.0e-14)
}

void test_04() {
    comp(1.0e-15)
}

void test_05() {
    comp(1.0e-16)
}

void test_06() {
    r1 = mult_by_sd(0.0, 100.0);
    r2 = 0.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-0.0, 100);
    r2 = -0.0 * 100.0;
    char a1[25], a2[25];
    sprintf(a1, "%.16e", r1);
    sprintf(a2, "%.16e", r2);
    g_assert_cmpstr(a1, ==, a2);
}

void test_07() {
    r1 = mult_by_sd(1.0, 100.0);
    r2 = 1.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-1.0, 100.0);
    r2 = -1.0 * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_08() {
    r1 = mult_by_sd(INFINITY, 100.0);
    r2 = INFINITY * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
    r1 = mult_by_sd(-INFINITY, 100.0);
    r2 = -INFINITY * 100.0;
    g_assert_cmpfloat(r1, ==, r2);
}

void test_09() {
    r1 = mult_by_sd(INFINITY, 0.0);
    g_assert_cmpfloat(r1, !=, r1);
}

void test_10() {
    r1 = mult_by_sd(NAN, 100.0);
    g_assert_cmpfloat(r1, !=, r1);
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


int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);
    
    g_test_set_nonfatal_assertions();
    g_test_add_func("/random/accuracy-12", test_01);
    g_test_add_func("/random/accuracy-13", test_02);
    g_test_add_func("/random/accuracy-14", test_03);
    g_test_add_func("/random/accuracy-15", test_04);
    g_test_add_func("/random/accuracy-16", test_05);

    g_test_add_func("/special_cases/zeros", test_06);
    g_test_add_func("/special_cases/ones", test_07);
    g_test_add_func("/special_cases/infinity", test_08);
    g_test_add_func("/special_cases/infinity_zero", test_09);
    g_test_add_func("/special_cases/nan", test_10);

    g_test_add_func("/denormal/overflow", test_11);
    g_test_add_func("/denormal/rounding_before_the_edge", test_12);
    g_test_add_func("/denormal/rounding_on_the_edge", test_13);
    g_test_add_func("/denormal/rounding_over_the_edge", test_14);
    g_test_add_func("/denormal/general_operation", test_15);
    return g_test_run();
}