#ifndef _Util_hpp_
#define _Util_hpp_

#include <cstdlib>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace Util {
// *********
// * Misc. *
// *********
    inline int round(float f)
    {
        if(f < 0.0) return int(f - 0.5f);
        else return int(f + 0.5f);
    }
    
// **********
// * Random *
// **********

    inline float randf() // 0 to 1
    {
        return float(rand()) / float(RAND_MAX);
    }

    inline float randf(float min, float max)
    {
        return min + (max - min) * randf();
    }

    inline float normalRandf()
    {
        return randf(-1.f, 1.f) + randf(-1.f, 1.f) + randf(-1.f, 1.f);
    }

    inline float normalRandf(float mean, float stdev)
    {
        return mean + normalRandf() * stdev;
    }

    inline int randi(int min, int max)
    {
        return round(randf(min, max));
    }
    
    inline int randomDither(float v)
    {
        int vi = int(v);
        float error = v - vi;
        return randf() > error ? vi : (vi + 1);
    }
    
// ************
// * Clamping *
// ************

    inline int clamp255(float f)
    {
        if(f < 0) return 0;
        if(f > 255) return 255;
        return int(f);
    }
    
    inline float clampf(float x)
    {
        if(x < 0.f) return 0.f;
        if(x > 1.f) return 1.f;
        return x;
    }

    template <class T>
    T clamp(T n, T min, T max)
    {
        if(n < min) n = min;
        else if(n > max) n = max;
        return n;
    }

// ************
// * Wrapping *
// ************

    inline int wrapi(int n, int min, int max)
    {
        int t = n - min, l = max - min;
        int mod = t % l;
        return mod >= 0 ? min + mod : min + l + mod;
    }

    inline int wrapi(int n, int max)
    {
        int mod = n % max;
        return mod >= 0 ? mod : max + mod;
    }

    inline float wrapf(float n, float min, float max)
    {
        float t = n - min, l = max - min;
        float mod = t - int(t / l) * l;
        return mod >= 0 ? min + mod : min + l + mod;
    }
}

#endif
