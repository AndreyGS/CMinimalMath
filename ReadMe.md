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

So, refered (sldouble) type is taking a part in math calculations that only include summing and bitwise operators, to perform multiplication, division and raising number to given power. In some cases even substraction was replaced with the summing with complement and addition of one.

The Java version have the several other functions implemented.

Also current implementation is slightly different from that in Java. There, raw was implemented as String, and now as an unsigned integral. Current change was made because I wished to try something new ^). Actually I made on C for testing proposes a few another fully working verions. Some of them were with raw as a char \* and one of it is used dynamic memory allocation (at the expense of 10-20% speed). But the current version is the most "proper" from my perspective.

## How to use

Simple usage:\
1.\
make\
&nbsp;&nbsp;&nbsp;&nbsp;or\
cc -Wall -Wextra -fopenmp -O3 ./src/sldouble.c ./src/sldtestunit.c -lm -o sldtests\
where cc is your compiler\
(you can omit -fopenmp flag if you having an issues with OpenMP library)\
2.\
./sldtests&nbsp;&nbsp;&nbsp;mult | sqrt | fp | ip | div | pow&nbsp;&nbsp;&nbsp;OPTIONS

div:  dividend(double) divisor(double) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;dividend divisor - returns quotient of input

fp:   number(double) power(double) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;number power - returns number raised to fraction part of given power

ip:   number(double) power(double) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;number power - returns number raised to integer part of given power

mult: -b [precision] | factor1(double) factor2(double) | -f | -s \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-b - big accuracy test of multiplication with default precision \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;12 digits after decimal point or with \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;selected precision  but not greater than 16 \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;factor1 factor2 - multiplication of 2 selected numbers \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-f - fpu multiplication speed test (it's here for historical reasons) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-s - soft multiplication speed test (it's here for historical reasons)

pow:  -b [precision] | number(double) power(double) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-b - big accuracy test of raising number to given power \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;with default precision 12 digits after decimal point \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;or with selected precision  but not greater than 16 \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;number power - returns number raised to the given power

sqrt: -b [precision] | number(double) \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-b - big accuracy test of square root with default precision 12 digits \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;after decimal point or with selected precision \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;but not greater than 16 \
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;number - selected number for square root test

P.S.: All big tests are include 64,000,000 operations on fpu and just as much on soft. Also, if you wish to see where result between fpu and sldouble calculations would be different you need to run one of the big tests with precision 16 - that is now the only precision by which you will see that. And even with it, the total accuracy is greater that 99,99% in all tests.

P.P.S.: Currently the only functions that have examinations of input values are mult, sqrt and pow. They properly hold any legal double value in contrast to fp, ip and div.

GLib tests (if you have glib-2.0 library):\
1.\
make glsldtests\
&nbsp;&nbsp;&nbsp;&nbsp;or\
cc -Wall -Wextra -O3 $(shell pkg-config --cflags glib-2.0) ./src/sldouble.c ./src/sldtestunit_gl.c $(shell pkg-config --libs glib-2.0) -lm -o glsldtests\
where cc is your compiler\
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
