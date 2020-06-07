#pragma once

#include <stdint.h> // uint64_t int64_t

typedef struct {
    double _dbl;
    uint64_t _raw;
	int64_t _exp;
    int _len;
    int _nsign;
    int _flags;
} sldouble;

enum { FINAL = 1, HASDOUBLE = 2 };

sldouble getsldouble_d(double d);
sldouble getsldouble_c(const sldouble *restrict sd);
double mult_by_sd(double d1, double d2);
double get_ieee754(sldouble *restrict sd);

int get_number_of_leading_zeros_ui64(uint64_t num);
int get_number_of_trailing_zeros_ui64(uint64_t num);
void switch_sd_sign(sldouble *restrict sd);