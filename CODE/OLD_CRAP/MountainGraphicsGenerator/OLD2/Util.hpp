#ifndef _Util_hpp_
#define _Util_hpp_

#include <cstdlib>
#include <istream>
#include <sstream>
#include <string>
#include <fstream>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

int parseHexColor(char const* code)
{
    std::stringstream converter;
    int col;
    
    if(code[0] == '#') converter << &code[1];
    else              converter << code;
    
    converter >> std::hex >> col;
    return col;
}

int clamp255(float f)
{
 if(f < 0) return 0;
 if(f > 255) return 255;
 return int(f);
}

float randf()
{
      return float(rand()) / float(RAND_MAX);
}

float randf(float min, float max)
{
      return min + (max - min) * randf();
}

int myRound(float f)
{
    if(f < 0.0) return int(f - 0.5f);
    else return int(f + 0.5f);
}

int randi(int min, int max)
{
    return myRound(randf(min, max));
}

bool readIntoString(std::string& str, std::istream& in)
{
    std::stringstream buffer;
    buffer << in.rdbuf();
    str = buffer.str();
}

template <class T>
T clamp(T n, T min, T max)
{
    if(n < min) n = min;
    else if(n > max) n = max;
    return n;
}

int wrapi(int n, int min, int max)
{
    int t = n - min, l = max - min;
    int mod = t % l;
    return mod >= 0 ? min + mod : min + l + mod;
}

float wrapf(float n, float min, float max)
{
    float t = n - min, l = max - min;
    float mod = t - int(t / l) * l;
    return mod >= 0 ? min + mod : min + l + mod;
}

#endif
