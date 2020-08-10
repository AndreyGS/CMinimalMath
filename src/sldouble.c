#include <math.h>   // NAN +-INFINITY
#include "sldouble.h"

/* There is also exist negative NAN and some other special cases
 * of it, but in the C there is only one defined */

/* Here is a 5 special cases: +-NAN +-INFINITY +-0 
 *
 * In most times we can handle them all not by selecting in
 * the distant group, but if we do so, that will give us
 * overall speed up by excluding them from many of
 * calculations that are not necessary for them.
 *
 * Also 'SPECIALV' flag is mean that HASDOUBLE flag is also presents.
 * It will simplify many of checkings in related functions */
#define set_special_nan(sdspec, dspec, ns) {    \
    (sdspec)->_dbl = dspec,                     \
    (sdspec)->_raw = 3,                         \
    (sdspec)->_len = 2,                         \
    (sdspec)->_exp = 1024,                      \
    (sdspec)->_flags = SPECIALV | HASDOUBLE,    \
    (sdspec)->_nsign = ns;                      \
}

#define set_special_inf(sdspec, dspec, ns) {    \
    (sdspec)->_dbl = dspec,                     \
    (sdspec)->_raw = 0,                         \
    (sdspec)->_len = 0,                         \
    (sdspec)->_exp = 1024,                      \
    (sdspec)->_flags = SPECIALV | HASDOUBLE,    \
    (sdspec)->_nsign = ns;                      \
}
    
#define set_special_zero(sdspec, dspec, ns) {   \
    (sdspec)->_dbl = dspec,                     \
    (sdspec)->_raw = 0,                         \
    (sdspec)->_len = 0,                         \
    (sdspec)->_exp = 0,                         \
    (sdspec)->_flags = SPECIALV | HASDOUBLE;    \
    if (ns) (sdspec)->_nsign = 1;               \
    else (sdspec)->_nsign = 0;                  \
}

#define set_one(sdone, dspec, ns) {            \
    (sdone)->_dbl = dspec,                     \
    (sdone)->_raw = 1,                         \
    (sdone)->_len = 1,                         \
    (sdone)->_exp = 0,                         \
    (sdone)->_flags = HASDOUBLE;               \
    if (ns) (sdone)->_nsign = 1;               \
    else (sdone)->_nsign = 0;                  \
}

sldouble get_sldouble_fromd(const double d)
{
    sldouble sd;    

    char *restrict p = (char *) &d;
    
    /* We are using this way to find out what the leading bit
     * status instead of simply comparison of double number with zero 
     * through '-0.0' value, that is always shown as equal to '+0.0' */
    if (*(p + 7) & 0x80) sd._nsign = 1;
    else sd._nsign = 0;

    /* Left offset for uint64_t raw fetching, which is default to zero
     * in case of normal (not denormal) numbers */
    unsigned int i = 0;
    /* In the begining, we getting exponent range (with part of mantissa catching) */
    /* Next tricky statement we could split to:
     *      int16_t *e16 = (int16_t *) ps + 3;
     *      int64_t e = (int64_t) ((*e16 & 0x7ff0) >> 4) - 1023; */
    int64_t e = (int64_t) ((*((int16_t *) p + 3) & 0x7ff0) >> 4) - 1023;
    
    /* Cases of NAN and +-INF */
    if (e == 1024) {
        if (d != d) set_special_nan(&sd, d, sd._nsign)
        else set_special_inf(&sd, d, sd._nsign)
        return sd;
    /* Cases of zeros and denormal numbers */
    } else if (e == -1023) {
        if (d == 0.0) {
            set_special_zero(&sd, 0.0, sd._nsign)
            return sd; 
        }
        
        /* Here i is substracted by 11 instead of 12 because of later
         * calculating of sd._len, and i must be bigger on 1 there */
        i = get_number_of_leading_zeros_64bit_var(p) - 11;
        e -= i-1;
    }
    
    sd._dbl = d,
    sd._flags = HASDOUBLE,
    sd._exp = e;
    
    /* Right offset for uint64_t raw fetching */
    unsigned int j = get_number_of_trailing_zeros_64bit_var(p);
    if (j > 52) j = 52;

    /* This statement can be expressed simpler by:
     * uint64_t r = *((uint64_t *) p); r = (r & 0x000fffffffffffff) >> j; */
    uint64_t r = (*((uint64_t *) p) & 0x000fffffffffffff) >> j;
    sd._len = 53 - i - j;
    /* If we've got normal numbers we need to add leading '1' */
    if (e > -1023) r |= 0x0010000000000000 >> j;

    sd._raw = r;

    return sd;
}

static void inner_mult(sldouble *const target, 
                       sldouble *const f1,
                       sldouble *const f2)
{
    uint64_t product = 0, unit, raw;
    int leadzeros;
    
    /* This is overall faster to compare only lenghts of sd values
     * rather than calculating the numbers of their ones */
    if (f1->_len < f2->_len) {
        unit = f2->_raw;
        leadzeros = 64 - f2->_len;
        raw = f1->_raw;
    } else {
        unit = f1->_raw;
        leadzeros = 64 - f1->_len;
        raw = f2->_raw;
    }
    
    int biasproduct = 0; 

    /* !leadzeros is only possible if unit is 64 bit long and
     * and unit should ends with one and product must be equal to '0' */
    if (!leadzeros) {
        unit >>= 1;
        ++unit;
        biasproduct = 1;
        product = unit;
    } else {
        /* As last raw bit at the start is always set to '1' we simply
         * skip check of it and put initial unit value to product variable */
        product = unit; 
        for (int shift = 1, check, spaceneed; raw > 0;) {
            if ((raw >>= 1) & 0x01) {
                check = leadzeros - shift;
                if (check > 0) {
                    unit <<= shift;
                    leadzeros -= shift;
                    product += unit;
                    shift = 1;
                } else {
                    unit <<= leadzeros-1,
                    spaceneed = -(check-1);
                    if ((product >> (spaceneed - 1)) & 0x01) {
                        product >>= spaceneed;
                        ++product;
                    } else {
                        product >>= spaceneed;
                    }
                    biasproduct += spaceneed;
                    product += unit;
                    break;
                }
            } else
                ++shift;
        }
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

    if (f1->_nsign == f2->_nsign) target->_nsign = 0;
    else target->_nsign = 1;
    
    const unsigned int i = get_number_of_leading_zeros_64bit_var(&product);
    /* (64 - i + biasproduct) - is result length of virtual product raw;
     * 'virtual' is because actual legth is '64' (as length of uint64_t),
     * but we substracting leading zeros and adding final bias */
    target->_exp = 64 - i + biasproduct 
            - f1->_len - f2->_len + 1 + f1->_exp + f2->_exp;
    
    const unsigned int j = get_number_of_trailing_zeros_64bit_var(&product);
    target->_raw = product >> j,
    target->_len = 64-i-j,
    target->_flags = 0;
}

#define set_special_mult_result(sdspec, f1, f2)                 \
{                                                               \
    if ((f1)->_dbl != (f1)->_dbl)                               \
        set_special_nan(sdspec, NAN, 0)                         \
    else if ((f1)->_dbl == 0.0) {                               \
        if (((f2)->_flags & SPECIALV) && (f2)->_exp == 1024)    \
            set_special_nan(sdspec, NAN, 0)                     \
        else if ((f1)->_nsign == (f2)->_nsign)                  \
            set_special_zero(sdspec, 0.0, 0)                    \
        else                                                    \
            set_special_zero(sdspec, -0.0, 1)                   \
    }                                                           \
    else if (((f2)->_flags & SPECIALV) && (f2)->_exp == 0)      \
        set_special_nan(sdspec, NAN, 0)                         \
    else if ((f1)->_nsign == (f2)->_nsign)                      \
        set_special_inf(sdspec, INFINITY, 0)                    \
    else                                                        \
        set_special_inf(sdspec, -INFINITY, 1)                   \
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

/* To speed up operations in inner_fract_power we must to allow
 * target and source pointers lead to one struct */
void inner_sqrt(sldouble *const target, sldouble *const source)
{
    /* Here last statement is a rounding, and we're 
     * always round up (source->_raw + 1) because 
     * the sldouble raw format must to hold 0 of trailing zeros */
    uint64_t raw = (source->_exp & 1)
                  ? source->_raw << (64 - source->_len)
                  : (source->_len < 64)
                  ? source->_raw << (63 - source->_len)        
                  : (source->_raw + 1) >> 1;
    
    uint64_t minuend = (raw >> 62) - 1,
            subtrahend,
            filt = 0xffffffffffffffff,
            root = 1;
    unsigned int offset = 62;
    
    #define residual_macro                              \
            root <<= 1;                                 \
            if (minuend >= subtrahend) {                \
                minuend = minuend + ~subtrahend + 1;    \
                ++root;                                 \
            }
            
    while (filt >>= 2) {
        minuend = ((filt & raw) >> (offset -= 2)) + (minuend << 2);
        subtrahend = (root << 2) + 1;
        residual_macro
    }

    while (root < 0x8000000000000000 && minuend < 0x4000000000000000) {
        minuend <<= 2;
        subtrahend = (root << 2) + 1;
        residual_macro
    }
    
    /* Workaround for the last two bits values */
    if (root < 0x8000000000000000) {
        if (minuend == 0x4000000000000000) {
            minuend <<= 1;
            subtrahend = (root << 1) + 1;
            residual_macro
        }
        if (root < 0x8000000000000000 && minuend >= (root + 1)) ++root;
    }
    
    #undef residual_macro

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
    target->_exp = (source->_exp == -1 || source->_exp == 0)
                  ? source->_exp
                  : (source->_exp > 0)
                  ? source->_exp >> 1
                  : (source->_exp >> 1) | 0x8000000000000000;
    #pragma GCC diagnostic pop
                  
    const unsigned int j = get_number_of_trailing_zeros_64bit_var(&root);
    
    const unsigned int i = get_number_of_leading_zeros_64bit_var(&root);
    
    target->_raw = root >> j,
    target->_len = 64-i-j,
    target->_nsign = 0,
    target->_flags = 0;
}

#define set_special_root_result(sdspec, num) {  \
    if ((num)->_dbl == NAN)                     \
        set_special_nan(sdspec, NAN, 0)         \
    else if ((num)->_dbl == 0.0)                \
        set_special_zero(sdspec, 0.0, 0)        \
    else if ((num)->_nsign)                     \
        set_special_inf(sdspec, -INFINITY, 1)   \
    else                                        \
        set_special_inf(sdspec, INFINITY, 0)    \
}


double sqrt_by_sd(const double d)
{
    if (d < 0) return -NAN;
    //else if (d == 1) return d;
    
    sldouble source = get_sldouble_fromd(d);
    sldouble target;
    
    if (source._flags & SPECIALV)
        set_special_root_result(&target, &source)
    else
        inner_sqrt(&target, &source);
    
    return get_double_ieee754(&target);
}

/* The source and power numbers must not be equal to any of the special values */
void inner_fract_power(sldouble *restrict const target,
                 const sldouble *restrict const source,
                 const sldouble *restrict const power)
{
    set_one(target, 1, 0)
    
    int64_t e = power->_exp;
    const unsigned int len = power->_len;
    /* It doesn't not make sense to raise to fract power 
     * that have an exp over than 63 or less than 0 or only int part digits */
    if (e > 62 || e >= (len+1) || e < -64) return;
    
    sldouble sdunit = *source;
    
    uint64_t raw;
    int offset = 64 - len + (e+1);
    if (offset < 0)
        raw = power->_raw >> (-offset);
    else
        raw = power->_raw << offset;

    do {
        inner_sqrt(&sdunit, &sdunit);
        if (sdunit._raw == 0x7fffffffffffffff
         || sdunit._raw == 0xffffffffffffffff) {
            sdunit._raw = 1;
            sdunit._len = 1;
            if (!++sdunit._exp) return;
        }
        if (raw & 0x8000000000000000) {
            inner_mult(target, target, &sdunit);
        }
    } while (raw <<= 1);
}

double fract_power_by_sd(const double source, const double power)
{    
    if (source < 0) return -NAN;
    
    sldouble sdpower = get_sldouble_fromd(power),
             sdsource = get_sldouble_fromd(source);
    sldouble target = {};

    if (sdsource._flags & SPECIALV)
        set_special_root_result(&target, &sdpower)
    else
        inner_fract_power(&target, &sdsource, &sdpower);
    
    return get_double_ieee754(&target);
}

/* The source and power numbers must not be equal to any of the special values */
void inner_int_power(sldouble *restrict const target,
               const sldouble *restrict const source,
               const sldouble *restrict const power)
{
    int64_t expplusone = power->_exp+1;
    /* It doesn't not make sense to raise to int power 
     * that have an exp over than 63 and less than 0 */
    if (expplusone > 64) {
        if (((source->_exp > -1) && !power->_nsign)
            || (source->_exp < 0 && power->_nsign))
                set_special_inf(target, INFINITY, 0)
        else
                set_special_zero(target, 0.0, 0)
        return;
    } else if (expplusone < 1) {
        set_one(target, 1, 0)
        return;
    }
    
    set_one(target, 1, 0)
    
    uint64_t raw;
    if (expplusone > power->_len)
        raw = power->_raw << (expplusone - power->_len);
    else
        raw = power->_raw >> (power->_len - expplusone);
    
    uint64_t filt = 0x8000000000000000 >> (64 - expplusone);

    #define check_int_power_overflow_macro \
        if (target->_exp > 1023) { \
            if (!power->_nsign) { \
                if (source->_nsign && (raw & 1)) \
                    set_special_inf(target, -INFINITY, 1) \
                else \
                    set_special_inf(target, INFINITY, 0) \
            } else { \
                if (source->_nsign && (raw & 1)) \
                    set_special_zero(target, -0.0, 1) \
                else \
                    set_special_zero(target, 0.0, 0) \
            } \
            return; \
        } else if (target->_exp < -1074) { \
            if (!power->_nsign) { \
                if (source->_nsign && (raw & 1)) \
                    set_special_zero(target, -0.0, 1) \
                else \
                    set_special_zero(target, 0.0, 0) \
            } else { \
                if (source->_nsign && (raw & 1)) \
                    set_special_inf(target, -INFINITY, 1) \
                else \
                    set_special_inf(target, INFINITY, 0) \
            } \
            return; \
        }
    
    /* Current using of pragma directives is because of we put 'source' variable
     * to inner_mult call, which do not have 'const' qualifiers on its inputs.
     * But as we know, nothing in 'source' will change through this
     * calls - and we don't need to create additional variable to copy it
     * contents to pass it to inner_mult instead of 'source'
     * variable which is constant. So we just ask compiler to be quiet about that */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
    if (raw > 1) inner_mult(target, source, source);
    check_int_power_overflow_macro
    while ((filt >>= 1) > 1) {
        if (raw & filt) {
            inner_mult(target, target, source);
            check_int_power_overflow_macro
        }
        inner_mult(target, target, target);
        check_int_power_overflow_macro
    }
    if (raw & 1)
        inner_mult(target, target, source);
    #pragma GCC diagnostic pop
    
    #undef check_int_power_overflow_macro
}

double int_power_by_sd(const double source, const double power)
{    
    sldouble sdpower = get_sldouble_fromd(power),
             sdsource = get_sldouble_fromd(source);
    sldouble target = {};

    if (sdsource._flags & SPECIALV)
        set_special_root_result(&target, &sdpower)
    else
        inner_int_power(&target, &sdsource, &sdpower);
    
    return get_double_ieee754(&target);
}

void inner_division(sldouble *const target,
              const sldouble *const dividend,
              const sldouble *const divisor)
{
    uint64_t dividendraw = dividend->_raw << (64 - dividend->_len), quotient = 0;
    
    const uint64_t divisorraw_big = divisor->_raw << (64 - divisor->_len),
                   divisorraw_big_neg = ~divisorraw_big + 1,
                   divisorraw_small_neg = (divisor->_len == 64)
                                       ? ~((divisorraw_big >> 1) + 1) + 1
                                       : ~(divisorraw_big >> 1) + 1;
    
    int initialbias, offset = 0, suboffset = 0, quotoffset;
    if (dividendraw < divisorraw_big) {
        initialbias = 1;
        quotoffset = 64;
    } else {
        initialbias = 0;
        quotoffset = 63;
    }

    while (quotoffset && dividendraw) {
        if (quotoffset < offset) {
            quotient <<= quotoffset;
            break;
        } else {
            quotient <<= offset;
            quotoffset -= offset;
        }

        if (dividendraw < divisorraw_big) {
            if (!quotoffset) break;
            quotient <<= 1;
            dividendraw = dividendraw + divisorraw_small_neg;
            suboffset = 1;
            quotoffset--;
        } else {
            dividendraw = dividendraw + divisorraw_big_neg;
            suboffset = 0;
        }
        ++quotient;
        
        offset = get_number_of_leading_zeros_64bit_var(&dividendraw);
        dividendraw <<= offset;
        offset -= suboffset;
    }
    
    if (dividend->_nsign == divisor->_nsign) target->_nsign = 0;
    else target->_nsign = 1;
    
    target->_exp = dividend->_exp - divisor->_exp - initialbias;
    const unsigned int j = get_number_of_trailing_zeros_64bit_var(&quotient);
    target->_raw = quotient >> j;
    const unsigned int i = get_number_of_leading_zeros_64bit_var(&quotient);
    target->_len = 64-i-j;
    target->_flags = 0;
}

double division_by_sd(const double dividend, const double divisor)
{    
    sldouble sddividend = get_sldouble_fromd(dividend),
             sddivisor = get_sldouble_fromd(divisor);
    sldouble target;
    
    /* There is no check of inputs, for now */
    inner_division(&target, &sddividend, &sddivisor);
    
    return get_double_ieee754(&target);
}

#define set_special_pow_result_pow(sdspec, sdn, sdp) { \
    /* Power NANs */ \
    if ((sdp)->_len == 2) { \
        if ((sdn)->_dbl == 1) \
            set_one(sdspec, 1, 0) \
        else if ((sdn)->_dbl != (sdn)->_dbl) \
            set_special_nan(sdspec, (sdn)->_dbl, (sdn)->_nsign) \
        else \
            set_special_nan(sdspec, (sdp)->_dbl, (sdp)->_nsign) \
    /* Power ZEROs */ \
    } else if (!(sdp)->_exp) { \
        set_one(sdspec, 1, 0) \
    /* Power INFs */ \
    } else { \
        if ((sdn)->_dbl != (sdn)->_dbl) \
            set_special_nan(sdspec, (sdn)->_dbl, (sdn)->_nsign) \
        else if ((sdn)->_dbl == 1 || (sdn)->_dbl == -1) \
            set_one(sdspec, 1, 0) \
        /* if num is not zero and pow -inf or \
         * num is zero and pow isinf result is zero */ \
        else if (((sdp)->_nsign && !((sdn)->_dbl == 0.0)) \
                || (!(sdp)->_nsign && (sdn)->_dbl == 0.0)) { \
            set_special_zero(sdspec, 0.0, 0) \
        } else { \
            set_special_inf(sdspec, INFINITY, 0) \
        } \
    } \
}

#define set_special_pow_result_num(sdspec, sdn, sdp) { \
    /* Number NANs */ \
    if ((sdn)->_dbl != (sdn)->_dbl) { \
        set_special_nan(sdspec, (sdn)->_dbl, (sdn)->_nsign) \
    /* Number ZEROs */ \
    } else if (!(sdn)->_dbl) { \
        if ((sdp)->_nsign) { \
            if ((sdp)->_exp < 64 && ((sdp)->_exp + 1 == (sdp)->_len) && (sdn)->_nsign) \
                set_special_inf(sdspec, -INFINITY, 1) \
            else \
                set_special_inf(sdspec, INFINITY, 0) \
        } else { \
            if ((sdp)->_exp < 64 && ((sdp)->_exp + 1 == (sdp)->_len) && (sdn)->_nsign) \
                set_special_zero(sdspec, -0.0, 1) \
            else \
                set_special_zero(sdspec, 0.0, 0) \
        } \
    /* Number INFs */ \
    } else { \
        if ((sdp)->_nsign) { \
            if ((sdp)->_exp < 64 && ((sdp)->_exp + 1 == (sdp)->_len) && (sdn)->_nsign) \
                set_special_zero(sdspec, -0.0, 1) \
            else \
                set_special_zero(sdspec, 0.0, 0) \
        } else { \
            if ((sdp)->_exp < 64 && ((sdp)->_exp + 1 == (sdp)->_len) && (sdn)->_nsign) \
                set_special_inf(sdspec, -INFINITY, 1) \
            else \
                set_special_inf(sdspec, INFINITY, 0) \
        } \
    } \
}

double pow_by_sd(const double number, const double power)
{
    if (power == 1) return number;
    
    sldouble sdnum = get_sldouble_fromd(number),
             sdpow = get_sldouble_fromd(power);
    sldouble target;
    
    if (sdpow._flags & SPECIALV)
        set_special_pow_result_pow(&target, &sdnum, &sdpow)
    else if (sdnum._flags & SPECIALV)
        set_special_pow_result_num(&target, &sdnum, &sdpow)
    /* if there is a root operation with negative number */
    else if (number < 0 && (sdpow._len > sdpow._exp+1))
        set_special_nan(&target, -NAN, 1)
    else {
        sldouble fppart;
        inner_fract_power(&fppart, &sdnum, &sdpow);
        inner_int_power(&target, &sdnum, &sdpow);
        /* if here is an overflow after the int power operation */
        if (target._flags & 1)
            return target._dbl;
        else
            inner_mult(&target, &fppart, &target);
            
        if (power < 0) {
            sldouble sd_one;
            set_one(&sd_one, 1, 0)
            inner_division(&target, &sd_one, &target);
        }
    }
    
    return get_double_ieee754(&target);
}

double get_double_ieee754(sldouble *restrict const sd)
{
    if (sd->_flags & HASDOUBLE) return sd->_dbl;
    
    int64_t exp = sd->_exp + 1023;
    
    if (exp >= 2047) {
        /* We don't need to check for NAN, because we already have checked
         * above HASDOUBLE flag, and in NAN case it must return true */
        if (sd->_nsign) set_special_inf(sd, -INFINITY, 1)
        else set_special_inf(sd, INFINITY, 0)
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
                    if (sd->_nsign) set_special_inf(sd, -INFINITY, 1)
                    else set_special_inf(sd, INFINITY, 0)
                    return sd->_dbl;
                }
                ++sd->_exp;
                if (exp > 0) ++addoffset;
            }
            ++raw;
        }
        raw >>= addoffset;
        sd->_raw = raw >> get_number_of_trailing_zeros_64bit_var(&raw);
    } else {
        raw <<= leftmost_fraction_significant_bit - len;
    }

    /* Clearing leading bit and adding exp value in raw of normal number */
    if (exp > 0) {
        raw &= 0x000fffffffffffff;
        raw |= exp << 52;
    }

    if (sd->_nsign) raw |= 0x8000000000000000;

    /* Compiler put a warning about strict aliasing on -O2 and -O3 
     * optimization levels when we directly cast vars 
     * if we do like this:
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
    register char *restrict p = (char *) num + 8, c;
    register int n = 0;
    while (n < 64 && !*--p) n += 8;
   
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
    register char *restrict p = (char *) num - 1, c;
    register int n = 0;
    while (n < 64 && !*++p) n += 8;

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
