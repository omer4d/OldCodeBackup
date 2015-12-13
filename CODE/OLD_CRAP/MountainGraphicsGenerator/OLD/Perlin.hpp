#ifndef _Perlin_hpp_
#define _Perlin_hpp_

#include "Vec2f.hpp"

struct Perlin {
    typedef float RandFloatGen(int32_t x);

    RandFloatGen* randf;
    float p;
    int n;
    
    Perlin(RandFloatGen* randf, float p, int n)
    {
        this->randf = randf;
        this->p = p;
        this->n = n;
    }
    
    static float blendFunc(float k)
    {
        //float k3 = k * k * k;
        //float k4 = k3 * k;

        //return 6.f * k4 * k - 15.f * k4 + 10.f * k3;
        return k;
    }

    static float blend(float v1, float v2, float k)
    {
        return v1 * blendFunc(1.0 - k) + v2 * blendFunc(k);
    }
    
    // ******
    // * 1D *
    // ******
    
    float noise(float x)
    {
        int xi = int(x);
        float k = x - xi;

        float v1 = (k - 0.f) * randf(xi);
        float v2 = (k - 1.f) * randf(xi + 1);
        
        return Perlin::blend(v1, v2, k);
    }
    
    // ******
    // * 2D *
    // ******

    Vec2f randf2d(int32_t x, int32_t y)
    {
        return Vec2f(randf(x + y * 1024), randf((x + y * 1024) + 100000));
    }

    float noise(float x, float y)
    {
        int xi = int(x), yi = int(y);
        Vec2f k(x - xi, y - yi);

        float v1 = (k - Vec2f(0.f, 0.f)).dot(randf2d(xi, yi));
        float v2 = (k - Vec2f(1.f, 0.f)).dot(randf2d(xi + 1, yi));

        float v3 = (k - Vec2f(0.f, 1.f)).dot(randf2d(xi, yi + 1));
        float v4 = (k - Vec2f(1.f, 1.f)).dot(randf2d(xi + 1, yi + 1));
        
        float i1 = Perlin::blend(v1, v2, k.x);
        float i2 = Perlin::blend(v3, v4, k.x);

        return Perlin::blend(i1, i2, k.y);
    }
    
    float getValue(float x, float y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;

        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise(x * freq, y * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }
};

#endif
