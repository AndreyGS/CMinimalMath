# CMinimalMath

Current project is some kind of fork of my another project MinimalMath that I was writing on Java. But with only one method - that perfoming multiplication. This project was built only as subject of self-training on C skills and for test - how fast C can be relatively of Java.

This program defines a surrogate type (structure) - sldouble, that holds different parts of original double in its fields. The parts are:

- \_dbl - the double itself (if it was builded from it or, was compiled already);
- \_raw - unsigned 64 bit variable that represent matissa of the number (with cutted zeros from right side);
- \_len - holds length of mantissa (from the most significant one to the right end);
- \_exp - exponent of number in 64 bit signed integer representation;
- \_nsign - the sign of the number (1 - is for negative | 0 - is for positive);
- \_flags - currently only two flags in this implementation availible - FINAL and HASDOUBLE;

The challenge was to achive the preferable range of correctness on perfoming calculations on double values in the form of sldouble structure. And the final results are close enough.

So, refered (sldouble) type is taking a part in math calculations that only include summing and bitwise operators, to perform multiplication, division and raising number to given power. In some cases even substraction was replaced with the summing with complement and 1 addition.

The Java version have the several other functions implemented.

Also current implementation is slightly different from that in Java. There, raw was implemented as String, and now as an unsigned integral. Current change was made because I wished to try something new ^). Actually I made on C for testing proposes a few another fully working verions. Some of them were with raw as a char \* and one of it is used dynamic memory allocation (at the expense of 10-20% speed). But the current version is the most "proper" from my perspective.

Compilated program is performing test on 3,200,000 multiplications with random numbers and adjusted precision (numbers after decimal point). If you wish to compare it with Java - it was upgraded and it now includes the same multiplication test and the same random number generator as it applied here. Yes, we can't directly compare this result as of slightly different implementation. But I've compared version that was made almost one by one as it present in Java, and may say that with Clang (or GCC) compiler optimization C was faster up to 9-10 times (3.5 times without it). Current versions are different in 15 times if no C optimization is performed and up to 26 times with -O3 option (depending on which compiler you prefer) (comparisons are made with multiplication test, with pow method the numbers must be much more different).

## How to use

Simple usage:\
1.\
make\
&nbsp;&nbsp;&nbsp;&nbsp;or\
cc -Wall -Wextra -O3 ./src/sldouble.c ./src/sldtestunit.c -lm -o sldtests\
2.\
./sldtests   mult | sqrt | fp | ip | div | pow   OPTIONS

OPTIONS: \
mult: -b \[acc] | factor1(double) factor2(double) | -f | -s \
&nbsp;&nbsp;&nbsp;&nbsp;-b - big accuracy test of multiplication with default accuracy 12 digits \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;after decimal point or with acc but not greater than 16 \
&nbsp;&nbsp;&nbsp;&nbsp;factor1 factor2 - multiplication of 2 arbitary numbers \
&nbsp;&nbsp;&nbsp;&nbsp;-f - fpu multiplication speed test (it's here for historical reasons) \
&nbsp;&nbsp;&nbsp;&nbsp;-s - soft multiplication speed test (it's here for historical reasons)

sqrt: -b \[acc] | number \
&nbsp;&nbsp;&nbsp;&nbsp;-b - big accuracy test of square root with default accuracy 12 digits \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;after decimal point or with \[acc] but not greater than 16 \
&nbsp;&nbsp;&nbsp;&nbsp;number - arbitary number for square root test

fp: number(double) power(double) \
&nbsp;&nbsp;&nbsp;&nbsp;number power - returns number raised to fraction part of given power

ip: number(double) power(double) \
&nbsp;&nbsp;&nbsp;&nbsp;number power - returns number raised to integer part of given power

div: dividend(double) divisor(double) \
&nbsp;&nbsp;&nbsp;&nbsp;dividend divisor - returns quotient of input

pow: -b | number(double) power(double) \
&nbsp;&nbsp;&nbsp;&nbsp;-b - big accuracy test without any limitations of accuracy \
&nbsp;&nbsp;&nbsp;&nbsp;number power - returns number raised to the given power

P.S.: All big tests are include 3,200,000 operations for fpu and just as much for soft

P.P.S.: Currently the only functions that have checkings on input values are mult, sqrt and pow. They properly hold any legal double value in contrast to fp, ip and div

GLib tests (if you have glib-2.0 library):\
1.\
make glsldtests\
&nbsp;&nbsp;&nbsp;&nbsp;or\
cc -Wall -Wextra -O3 $(shell pkg-config --cflags glib-2.0) ./src/sldouble.c ./src/sldtestunit_gl.c $(shell pkg-config --libs glib-2.0) -lm -o glsldtests\
2.\
./glsldtests

## License

Copyright (c) 2020 Andrey Grabov-Smetankin

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
