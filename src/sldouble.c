#include <math.h>   // NAN +-INFINITY
#include "sldouble.h"

/* There is also exist negative NAN and some other special cases
 * of it, but in the C there is only one defined */

/* Here is a 5 special cases: NAN +-INFINITY +-0 
 *
 * In most times we can handle them all not by selecting in
 * the distant group, but if we do so, that will give us
 * overall speed up by excluding them from many of
 * calculations that are not necessary for them.
 *
 * Also 'SPECIALV' flag is mean that HASDOUBLE flag is also presents.
 * It will simplify many of checkings in related functions */
#define set_special_nan(sdspec) {               \
    (sdspec)->_dbl = NAN;                       \
    (sdspec)->_raw = 3;                         \
    (sdspec)->_len = 2;                         \
    (sdspec)->_exp = 1024;                      \
    (sdspec)->_flags = SPECIALV | HASDOUBLE;    \
    (sdspec)->_nsign = 0;                       \
}

#define set_special_inf(sdspec, dspec) {        \
    (sdspec)->_dbl = dspec;                     \
    (sdspec)->_raw = 0;                         \
    (sdspec)->_len = 0;                         \
    (sdspec)->_exp = 1024;                      \
    (sdspec)->_flags = SPECIALV | HASDOUBLE;    \
    if (dspec > 0) (sdspec)->_nsign = 0;        \
    else (sdspec)->_nsign = 1;                  \
}
    
#define set_special_zero(sdspec, dspec, ns) {   \
    (sdspec)->_dbl = dspec;                     \
    (sdspec)->_raw = 0;                         \
    (sdspec)->_len = 0;                         \
    (sdspec)->_exp = 0;                         \
    (sdspec)->_flags = SPECIALV | HASDOUBLE;    \
    if (ns) (sdspec)->_nsign = 1;               \
    else (sdspec)->_nsign = 0;                  \
}

sldouble get_sldouble_fromd(const double d)
{
    sldouble sd;    

    char *restrict p = (char *) &d, c;
    
    /* We are using this way to find out what the leading bit
     * status instead of simply comparison of double number with zero 
     * through '-0.0' value, that is always shown as equal to '+0.0' */
    if (*(p + 7) & 0x80) sd._nsign = 1;
    else sd._nsign = 0;

    /* Left offset for uint64_t raw fetching, which is default to zero
     * in case of normal (not denormal) numbers */
    int i = 0;
    /* In the begining, we getting exponent range (with part of mantissa catching) */
    /* Next tricky statement we could split to:
     *      int16_t *e16 = (int16_t *) ps + 3;
     *      int64_t e = (int64_t) ((*e16 & 0x7ff0) >> 4) - 1023; */
    int64_t e = (int64_t) ((*((int16_t *) p + 3) & 0x7ff0) >> 4) - 1023;

    if (e == 1024) {
        if (d != d) set_special_nan(&sd)
        else set_special_inf(&sd, d)
        return sd;
    } else if (e == -1023) {
        if (d == 0.0) {
            set_special_zero(&sd, 0.0, sd._nsign)
            return sd; 
        }
        /* If we've got denormal numbers on input, we need to
         * get a leftmost zeros preceding too, to compute final
         * exponent value and to get second bordrer for raw variable fetching,
         * which is default in case of normal nombers */
        p += 6;
        c = *p;
        /* i already is 0, so we need head condition check
         * only for final 'else' working properly */
        if      (c & 0x08)      ;
        else if (c & 0x04) i = 1;
        else if (c & 0x02) i = 2;
        else if (c & 0x01) i = 3;
        else i = 4;
        
        if (i == 4) {
            /* As it is not zero (as we checked in the beginning
             * of the function, at least last byte must be not equal to 0
             * so the resulting edges in cycle condition operators 
             * are not include such possibilities */
            while(*--p == 0) i += 8;

            c = *p;
            if      (c & 0x80)       ;
            else if (c & 0x40) ++i   ;
            else if (c & 0x20) i += 2;
            else if (c & 0x10) i += 3;
            else if (c & 0x08) i += 4;
            else if (c & 0x04) i += 5;
            else if (c & 0x02) i += 6;
            else               i += 7;
        }
        e -= i;
        
        p = (char *) &d;
    }
    
    sd._dbl = d;
    sd._flags = HASDOUBLE;
    sd._exp = e;
    
    /* Right offset for uint64_t raw fetching */
    int j = 0;
    /* To get the raw bits value, we need to do some tricky manipulations */
    /* First we need to calculate trailing zeros number */
    /* Here is no need to upperbound checking, if it is not 0 or some of other
     * special cases (and it is not, as we checked it in the begining 
     * of the function), than there must be at least one 1 in mantissa */
    --p;
    while(*++p == 0) j+=8;
    
    c = *p;
    if (j >= 48) {
        if      (c & 0x01) j = 48;
        else if (c & 0x02) j = 49;
        else if (c & 0x04) j = 50;
        else if (c & 0x08) j = 51;
        else               j = 52;
    } else {
        if      (c & 0x01)       ;
        else if (c & 0x02) ++j   ;
        else if (c & 0x04) j += 2;
        else if (c & 0x08) j += 3;
        else if (c & 0x10) j += 4;
        else if (c & 0x20) j += 5;
        else if (c & 0x40) j += 6;
        else               j += 7;
    }
    
    p = (char *) &d;
    /* And the second - transform current value to integer raw */
    /* this statement can be expressed simpler by:
     * uint64_t r = *((uint64_t *) p); r = (r & 0x000fffffffffffff) >> j; */
    uint64_t r = (*((uint64_t *) p) & 0x000fffffffffffff) >> j;
    sd._len = 53 - i - j;
    /* If we've got normal numbers we need to add leading '1' */
    if (e > -1022) r |= 0x0010000000000000 >> j;
    else --sd._len;

    sd._raw = r;

    return sd;
}

void get_sldouble_copy(sldouble *restrict const sd1, 
                 const sldouble *restrict const sd2)
{
    sd1->_dbl = sd2->_dbl;
    sd1->_raw = sd2->_raw;
    sd1->_exp = sd2->_exp;
    sd1->_len = sd2->_len; 
    sd1->_nsign = sd2->_nsign;
    sd1->_flags = sd2->_flags;
}

static void inner_mult(sldouble *restrict const sd, 
                 const sldouble *restrict const sd1,
                 const sldouble *restrict const sd2)
{
    uint64_t product = 0, unit, raw;
    
    /* This is overall faster to compare only lenghts of sd values
     * rather than calculating the numbers of their ones */
    if (sd1->_len < sd2->_len) {
        unit = sd2->_raw;
        raw = sd1->_raw;
    } else {
        unit = sd1->_raw;
        raw = sd2->_raw;
    }
    int biasproduct = 0; 
    int leadzeros = get_number_of_leading_zeros_64bit_var(&unit);

    for (int shift = 0, check, spaceneed; raw > 0;) {
        if (raw & 0x01) {
            check = leadzeros - shift;
            if (check > 0) {
                unit <<= shift;
                leadzeros -= shift;
            } else {
                if (!leadzeros) {
                    if (unit & 0x01) { unit >>= 1; ++unit; }
                    else unit >>= 1;
                    if (product) {
                        if (product & 0x01) { product >>= 1; ++product; }
                        else product >>= 1;
                    }
                    biasproduct += shift + 1;
                } else {
                    unit <<= leadzeros-1;
                    spaceneed = -(check-1);
                    if ((product >> (spaceneed - 1)) & 0x01) {
                        product >>= spaceneed;
                        ++product;
                    } else {
                        product >>= spaceneed;
                    }
                    biasproduct += spaceneed;
                }
                product += unit;

                break;
            }
            product += unit;
            shift = 1;
        } else {
            ++shift;
        }
        raw >>= 1;
    }

    for (int shift = 1; raw > 0;) {
        if ((raw >>= 1) & 0x01) {
            if ((product >> (shift - 1)) & 0x01) {
                product >>= shift;
                ++product;
            } else {
                product >>= shift;
            }
            biasproduct += shift;

            product += unit;
            shift = 1;
        } else {
            ++shift;
        }
    }

    if (sd1->_nsign == sd2->_nsign) sd->_nsign = 0;
    else sd->_nsign = 1;
    
    const unsigned int i = get_number_of_leading_zeros_64bit_var(&product);
    /* (64 - i + biasproduct) - is result length of virtual product raw;
     * 'virtual' is because actual legth is '64' (as length of uint64_t),
     * but we substracting leading zeros and adding final bias */
    sd->_exp = 64 - i + biasproduct 
            - sd1->_len - sd2->_len + 1 + sd1->_exp + sd2->_exp;
    
    const unsigned int j = get_number_of_trailing_zeros_64bit_var(&product);
    sd->_raw = product >> j;
    sd->_len = 64-i-j;
    sd->_flags = 0;
}

#define set_special_mult_result(sdspec, f1, f2)                 \
{                                                               \
    if ((f1)->_dbl != (f1)->_dbl)                               \
        set_special_nan(sdspec)                                 \
    else if ((f1)->_dbl == 0.0) {                               \
        if (((f2)->_flags & SPECIALV) && (f2)->_exp == 1024)    \
            set_special_nan(sdspec)                             \
        else if ((f1)->_nsign == (f2)->_nsign)                  \
            set_special_zero(sdspec, 0.0, 0)                    \
        else                                                    \
            set_special_zero(sdspec, -0.0, 1)                   \
    }                                                           \
    else if (((f2)->_flags & SPECIALV) && (f2)->_exp == 0)      \
        set_special_nan(sdspec)                                 \
    else if ((f1)->_nsign == (f2)->_nsign)                      \
        set_special_inf(sdspec, INFINITY)                       \
    else                                                        \
        set_special_inf(sdspec, -INFINITY)                      \
}

double mult_by_sd(const double d1, const double d2)
{
    sldouble sd1 = get_sldouble_fromd(d1),
             sd2 = get_sldouble_fromd(d2);
    sldouble sd;
    
    if (sd1._flags & SPECIALV)
        set_special_mult_result(&sd, &sd1, &sd2)
    else if (sd2._flags & SPECIALV) 
        set_special_mult_result(&sd, &sd2, &sd1)
    else   
        inner_mult(&sd, &sd1, &sd2);

    return get_double_ieee754(&sd);
}

double get_double_ieee754(sldouble *restrict const sd)
{
    if (sd->_flags & HASDOUBLE) return sd->_dbl;
    
    int64_t exp = sd->_exp + 1023;
    
    if (exp >= 2047) {
        /* We don't need to check for NAN, because we already have checked
         * above HASDOUBLE flag, and in NAN case it must return true */
        if (sd->_nsign) set_special_inf(sd, -INFINITY)
        else set_special_inf(sd, INFINITY)
        return sd->_dbl;
    } else if (exp < -52) {
        if (sd->_nsign) set_special_zero(sd, -0.0, 1)
        else set_special_zero(sd, 0.0, 0)
        return sd->_dbl;
    }
    
    uint64_t raw = sd->_raw;
    const int len = sd->_len;
    
    int leftmost_fraction_significant_bit = 53;
    /* Denormal numbers special handling */
    if (exp < 1) leftmost_fraction_significant_bit += exp-1;
    
    /* 'If' statement for rounding if it's need
     * or 'else' - just biasing raw to proper bits positions */
    if (len > leftmost_fraction_significant_bit) {
        raw >>= (len - leftmost_fraction_significant_bit - 1);
        uint64_t filt = 0x01;
        int addoffset = 1;
        if (raw & filt) {
            while (raw & (filt <<= 1));
            /* If raw through the it _len contains only ones
             * (it is true if filt > raw))
             * rounding can significant change it's value */
            if (filt > raw) {
                /* By this checking, we additionaly incrementing
                 * exponent, for the reason that number that consisting
                 * only of ones will increase its length when it will be
                 * incremented, so its exponent also should be incremented */
                
                /* And as a result, we can get an INFINITY*/
                if (++exp == 2047) {
                    if (sd->_nsign) set_special_inf(sd, -INFINITY)
                    else set_special_inf(sd, INFINITY)
                    return sd->_dbl;
                }
                ++sd->_exp;
                if (exp > 0) ++addoffset;
            }
            raw += 1;
        }
        raw >>= addoffset;
    } else {
        raw <<= leftmost_fraction_significant_bit - len;
    }

    sd->_raw = raw >> get_number_of_trailing_zeros_64bit_var(&raw);
   
    /* Clearing leading bit and adding exp value in raw of normal number */
    if (exp > 0) {
        raw &= 0x000fffffffffffff;
        raw |= exp << 52;
    }

    if (sd->_nsign) raw |= 0x8000000000000000;

    /* Compiler put a warning of strict aliasing on -O2 and -O3 
    * optimization levels when we directly cast vars 
    * if we did like this:
    *
    * return sd->_dbl = *(double *) &raw;
    *
    * So to not broke opmization algorithms
    * we need to add an intermidiate  step. */
    const char *restrict const p = (const char *) &raw;
    sd->_flags |= HASDOUBLE;
    return sd->_dbl = *(double *) p;
}

int get_number_of_leading_zeros_64bit_var(const void *restrict const num)
{
    char *restrict p = (char *) num + 8, c;
    int n = 0;
    while (*--p == 0 && n < 64) n += 8;
   
    c = *p;
    if (n < 64 && !(c & 0x80)) {
           if      (c & 0x40) ++n   ;
           else if (c & 0x20) n += 2;
           else if (c & 0x10) n += 3;
           else if (c & 0x08) n += 4;
           else if (c & 0x04) n += 5;
           else if (c & 0x02) n += 6;
           else               n += 7;
    }

    return n;
}

int get_number_of_trailing_zeros_64bit_var(const void *restrict const num)
{   
    char *restrict p = (char *) num - 1, c;
    int n = 0;
    while (*++p == 0 && n < 64) n += 8;

    c = *p;
    if (n < 64 && !(c & 0x01)) {
           if      (c & 0x02) ++n   ;
           else if (c & 0x04) n += 2;
           else if (c & 0x08) n += 3;
           else if (c & 0x10) n += 4;
           else if (c & 0x20) n += 5;
           else if (c & 0x40) n += 6;
           else               n += 7;
    }
   
    return n;
}

void switch_sd_sign(sldouble *restrict const sd)
{
    if ((sd->_flags & SPECIALV) && sd->_dbl != sd->_dbl) return;
    
    if (sd->_flags & HASDOUBLE) sd->_dbl = -sd->_dbl;
    if (sd->_nsign) sd->_nsign = 0;
    else sd->_nsign = 1;
}