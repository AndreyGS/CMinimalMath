# CMinimalMath

Current project is some kind of fork of my another project MinimalMath that I was writing on Java. But with only one method - that perfoming multiplication. This project was built only as subject of self-training on C skills and for test - how fast C can be relatively of Java. And for this meanings, one working method on hand is pretty enough.

This program defines a surrogate type (structure) - sldouble, that holds different parts of original double in its fields. The parts are:

- \_dbl - the double itself (if it was builded from it or, was compiled already);
- \_raw - unsigned 64 bit variable that represent matissa of the number (with cutted zeros from right side);
- \_len - holds length of mantissa (from the most significant one to the right end);
- \_exp - exponent of number in 64 bit signed integer representation;
- \_nsign - the sign of the number (1 - is for negative | 0 - is for positive);
- \_flags - currently only two flags in this implementation availible - FINAL and HASDOUBLE;

Java version has more fields and more functionality. More of it, there was done almost all main math operations including pow() as analogue of Math.pow(). So if I was planning to transfer all of the rest functions that are presents in Java version, more fields and flags are also should be here.

So, refered type is taking a part in math calculations that only include summing and bitwise operators, to perform multiplication here. I know, I know - it's not so hard to implement such logic, but as I said before - here presents only this function. In Java version there is all gamma of all main math operations performed on similar way.

The challenge was to achive the preferable range of correctness on perfoming calculations on double values in the form of sldouble structure. Unforunately, the length of 64 bit of mantissa can't give a 100% correctness of rounding in calculations. Moreover I was implemented rounding function (on Java) in a manner of it present in the standart, but the lack of bits leave final results on the same precision. But it is still very impressive, and I don't want to extend calculations to two separate raws of 64 bits to get ideal correctness.

Also this implementation is slightly different from that in Java. There, raw was implemented as String, and now as an unsigned integral. Current change was made because diferent tools are availible on these languages, and ISO C have better one (at least for current project). Actually I made on C for testing proposes a few another fully working verions. They was with raw as a char \* and one of these versions even used dynamic memory allocation :)(bad idea - you're right). But the current, is the most "proper" from my perspective.

Compilated program is performing test on 800000 multiplications with random numbers and adjusted precision (numbers after decimal point). If you wish to compare it with Java - it was upgraded and it now includes the same test and the same random number generator as it applied here. Yes, we can't directly compare this result as of slightly different implementation. But I've compared version that was made almost one by one as it present in Java, and may say that with Clang (or GCC) compiler optimization C was faster up to 9-10 times (3.5 times without it). Current versions are different in 14 times if no C optimization is performed and up to 25 times with -O3 option (depending on wich compiler you prefer).

## How to use

Simple usage:
1.
make
    or
cc -Wall -O3 ./src/sldouble.c ./src/sldtestunit.c -o sldmult 
2.
./sldmult \[big_test_accuracy(int)] || \[-f] || \[-s]
      || \[\[factor1(double) factor2(double)]]
\[big_test_accuracy] - test for 800000 multiplications with accuracy as integral with maximum 
of 16 digits after decimal point. This test is analogue for 
\[-f] - fpu speed test
\[-s] - sldmult speed test
\[factor1 factor2] - test of arbitary numbers multimplication
without parameters (default) - same as ./sldmult 12

GLib test (if you have glib-2.0 library):
1.
make gltest
    or
cc -Wall $(shell pkg-config --cflags glib-2.0) ./src/sldouble.c ./src/sldtestunit_gl.c $(shell pkg-config --libs glib-2.0) -o glmulttest
2.
./glmulttest

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