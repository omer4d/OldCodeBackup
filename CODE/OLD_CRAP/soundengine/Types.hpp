#ifndef _Types_hpp_
#define _Types_hpp_

namespace Audio {

typedef short int16;
typedef unsigned short uint16;

static double s2f(uint16 s)
{
 return s * 2.0 / 65535.0 - 1.0;
}

static uint16 f2s(double f)
{
 return uint16((f + 1.0) * 0.5 * 65535.0);
}

}

#endif
