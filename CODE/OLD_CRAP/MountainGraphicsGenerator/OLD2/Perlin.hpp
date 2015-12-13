#ifndef _Perlin_hpp_
#define _Perlin_hpp_

#include "Vec2f.hpp"
#include "Vec3f.hpp"

struct Perlin {
    static const int tableSize = 300;
    float p;
    int n;
    float sx, sy;
    int baseFreqX, baseFreqY;
    
    float table1d[tableSize];
    Vec2f table2d[tableSize];
    
    static int prng(int n)
    {
        n = (n >> 13) ^ n;
        return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    }
    
    Perlin(float p, int n)
    {
        this->p = p;
        this->n = n;
        this->sx = 1.f;
        this->sy = 1.f;
        this->baseFreqX = 1;
        this->baseFreqY = 1;
        
        int tsize = Perlin::tableSize;
        
        for(int i = 0; i < tsize; i++)
        {
            float k = float(i) / float(tsize - 1);
            float a = 2.f * M_PI * k;
            
            table1d[i] = -1.f + 2.f * k;
            table2d[i].init(cos(a), sin(a));
        }
        
        for(int i = 0; i < tsize; i++)
        {
             std::swap<float>(table1d[rand() % tsize], table1d[rand() % tsize]);
             std::swap<Vec2f>(table2d[rand() % tsize], table2d[rand() % tsize]);
        }
    }
    
    void setScaleX(int baseFreq, int width)
    {
         baseFreqX = baseFreq;
         sx = float(baseFreq) / float(width);
    }
    
    void setScaleY(int baseFreq, int height)
    {
         baseFreqY = baseFreq;
         sy = float(baseFreq) / float(height);
    }
    
    static float blendFunc(float k)
    {
        float k3 = k * k * k;
        float k4 = k3 * k;
        float f = 6.f * k4 * k - 15.f * k4 + 10.f * k3;

        return f;
    }

    static float blend(float v1, float v2, float k)
    {
        return v1 * blendFunc(1.0 - k) + v2 * blendFunc(k);
    }
    
    // ******
    // * 1D *
    // ******
    
    float randf(int x)
    {
        return table1d[Perlin::prng(x) % Perlin::tableSize];
    }
    
    float noise1f(float x)
    {
        int xi = int(x);
        float k = x - xi;

        float v1 = (k - 0.f) * randf(xi);
        float v2 = (k - 1.f) * randf(xi + 1);
        
        return Perlin::blend(v1, v2, k);
    }
    
    float tiledNoise1f(float x, int wx)
    {
        int xi = int(x);
        float k = x - xi;
        
        float v1 = (k - 0.f) * randf(xi % wx);
        float v2 = (k - 1.f) * randf((xi + 1) % wx);
        
        return Perlin::blend(v1, v2, k);
    }
    
    float getValue1f(float x)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        
        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise1f(x * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }
    
    float getValue1i(int x)
    {
        return getValue1f(x * sx);
    }
    
    float getTiledValue1i(int x)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        int wx = baseFreqX;
        
        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * tiledNoise1f(x * sx * freq, wx);
            amp *= p;
            freq *= 2.0;
            wx *= 2;
        }

        return total;
    }
    
    // ******
    // * 2D *
    // ******
    
    Vec2f const& randf2d(int32_t x, int32_t y)
    {
        return table2d[Perlin::prng(x + (y << 16)) % Perlin::tableSize];
    }

    float noise2f(float x, float y)
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
    
    float tiledNoise2f(float x, float y, int wx, int wy)
    {
        int xi = int(x), yi = int(y);
        Vec2f k(x - xi, y - yi);

        float v1 = (k - Vec2f(0.f, 0.f)).dot(randf2d(xi % wx, yi % wy));
        float v2 = (k - Vec2f(1.f, 0.f)).dot(randf2d((xi + 1) % wx, yi % wy));

        float v3 = (k - Vec2f(0.f, 1.f)).dot(randf2d(xi % wx, (yi + 1) % wy));
        float v4 = (k - Vec2f(1.f, 1.f)).dot(randf2d((xi + 1) % wx, (yi + 1) % wy));
        
        float i1 = Perlin::blend(v1, v2, k.x);
        float i2 = Perlin::blend(v3, v4, k.x);

        return Perlin::blend(i1, i2, k.y);
    }
    
    float getValue2f(float x, float y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        
        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise2f(x * freq, y * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }
    
    float getValue2i(int x, int y)
    {
        return getValue2f(x * sx, y * sy);
    }
    
    float getTiledValue2i(int x, int y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        int wx = baseFreqX, wy = baseFreqY;
        
        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * tiledNoise2f(x * sx * freq, y * sy * freq, wx, wy);
            amp *= p;
            freq *= 2.0;
            wx *= 2;
            wy *= 2;
        }
        
        return total;
    }
    
    // ******
    // * 3D *
    // ******
    
    
};

#endif
