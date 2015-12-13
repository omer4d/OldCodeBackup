#ifndef _Complex_hpp_
#define _Complex_hpp_

#include <fftw3.h>

namespace Audio {

typedef fftw_complex Complex;

static void Complex_set(Complex c, double r, double i)
{
 c[0] = r;
 c[1] = i;
}

static void Complex_mul(Complex c, Complex const a, Complex const b)
{
 c[0] = a[0] * b[0] - a[1] * b[1];
 c[1] = a[0] * b[1] + a[1] * b[0];
}

static void Complex_blend(Complex c, Complex const a, Complex const b, double k)
{
 c[0] = a[0] * (1.0 - k) + b[0] * k;
 c[1] = a[1] * (1.0 - k) + b[1] * k;
}

}

#endif
