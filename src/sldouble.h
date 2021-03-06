#pragma once

#include <stdint.h> // uint64_t int64_t

/* We're choosed int instead of char for _len, _nsign and _flags fields
 * for the speed reason. In fact it adds to us around 2% of it. */
typedef struct sldouble {
    double _dbl;
    uint64_t _raw;
    int64_t _exp;
    unsigned int _len;
    unsigned int _nsign;
    unsigned int _flags;
} sldouble;

enum { SPECIALV = 1, HASDOUBLE = 2 };

extern sldouble get_sldouble_fromd(double d);
extern double mult_by_sd(double d1, double d2);
extern double sqrt_by_sd(const double d);
extern double fract_power_by_sd(double source, double power);
extern double int_power_by_sd(double source, double power);
extern double division_by_sd(double dividend, double divisor);
extern double pow_by_sd(double number, double power);
extern double get_double_ieee754(sldouble *sd);

extern int get_number_of_leading_zeros_64bit_var(const void *num);
extern int get_number_of_trailing_zeros_64bit_var(const void *num);
extern void switch_sd_sign(sldouble *sd);
