#pragma once

#include <stdint.h> // uint64_t int64_t

typedef struct {
    double _dbl;
    uint64_t _raw;
    int _len;
    int64_t _exp;
    int _nsign;
    int _flags;
} sldouble;

enum { FINAL = 1, HASDOUBLE = 2 };

double mult_by_sd(double d1, double d2);
double get_ieee754(sldouble *sd);
sldouble getsldouble_d(double d);

int get_number_of_leading_zeros_ui64(uint64_t num);
int get_number_of_trailing_zeros_ui64(uint64_t num);
void switch_sd_sign(sldouble *sd);