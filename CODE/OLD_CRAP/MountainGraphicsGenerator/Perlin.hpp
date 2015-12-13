#ifndef _Perlin_hpp_
#define _Perlin_hpp_

#include "Vec2f.hpp"
#include "Vec3f.hpp"

struct Perlin {
    static const int tableSize = 2048;
    float p;
    int n;
    float sx, sy;
    int baseFreqX, baseFreqY;

    float grad1d[tableSize];
    Vec2f grad2d[tableSize];
    Vec3f grad3d[tableSize];

    int offs1[tableSize];
    int offs2[tableSize];

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

            offs1[i] = i;
            offs2[i] = i;
            
            grad1d[i] = -1.f + 2.f * k;
            grad2d[i].init(cos(a), sin(a));
            grad3d[i] = Vec3f(normalRandf(), normalRandf(), normalRandf()).unit();
        }

        for(int i = 0; i < tsize * 3; i++)
        {
            std::swap<float>(grad1d[rand() % tsize], grad1d[rand() % tsize]);
            std::swap<Vec2f > (grad2d[rand() % tsize], grad2d[rand() % tsize]);
            std::swap<int> (offs1[rand() %  tsize], offs1[rand() %  tsize]);
            std::swap<int> (offs2[rand() %  tsize], offs2[rand() %  tsize]);
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
    
    static float blend2d(float v00, float v10, float v01, float v11, float kx, float ky)
    {
        float i1 = Perlin::blend(v00, v10, kx);
        float i2 = Perlin::blend(v01, v11, kx);
        
        return Perlin::blend(i1, i2, ky);
    }
    
    static float blend3d(float v000, float v100, float v010, float v110,
                         float v001, float v101, float v011, float v111,
                         float kx, float ky, float kz)
    {
       float i1 = Perlin::blend2d(v000, v100, v010, v110, kx, ky);
       float i2 = Perlin::blend2d(v001, v101, v011, v111, kx, ky);
       
       return Perlin::blend(i1, i2, kz);
    }

    // ******
    // * 1D *
    // ******

    float randf(int x)
    {
        return grad1d[x % Perlin::tableSize];
    }

    float noise1f(float x)
    {
        int xi = int(x);
        float k = x - xi;

        float v1 = (k - 0.f) * randf(xi);
        float v2 = (k - 1.f) * randf(xi + 1);

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

    // ******
    // * 2D *
    // ******

    Vec2f const& randf2d(int32_t x, int32_t y)
    {
        return grad2d[(y + offs1[x % Perlin::tableSize]) % Perlin::tableSize];
    }

    float noise2f(float x, float y)
    {
        int xi = int(x), yi = int(y);
        Vec2f k(x - xi, y - yi);

        float v00 = (k - Vec2f(0.f, 0.f)).dot(randf2d(xi, yi));
        float v10 = (k - Vec2f(1.f, 0.f)).dot(randf2d(xi + 1, yi));

        float v01 = (k - Vec2f(0.f, 1.f)).dot(randf2d(xi, yi + 1));
        float v11 = (k - Vec2f(1.f, 1.f)).dot(randf2d(xi + 1, yi + 1));

        return Perlin::blend2d(v00, v10, v01, v11, k.x, k.y);
    }

    float getValue2f(float x, float y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;

        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise2f((x + n * 31) * freq, (y + n * 51) * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }

    float getValue2i(int x, int y)
    {
        return getValue2f(x * sx, y * sy);
    }

    // ******
    // * 3D *
    // ******
    
    Vec3f const& randf3d(int32_t x, int32_t y, int32_t z)
    {
        return grad3d[(z + offs1[(y + offs2[x % Perlin::tableSize]) % Perlin::tableSize]) % Perlin::tableSize];
    }
    
    float noise3f(float x, float y, float z)
    {
        int xi = int(x), yi = int(y), zi = int(z);
        Vec3f k(x - xi, y - yi, z - zi);
        
        float v000 = (k - Vec3f(0.f, 0.f, 0.f)).dot(randf3d(xi, yi, zi));
        float v100 = (k - Vec3f(1.f, 0.f, 0.f)).dot(randf3d(xi + 1, yi, zi));

        float v010 = (k - Vec3f(0.f, 1.f, 0.f)).dot(randf3d(xi, yi + 1, zi));
        float v110 = (k - Vec3f(1.f, 1.f, 0.f)).dot(randf3d(xi + 1, yi + 1, zi));
        
        float v001 = (k - Vec3f(0.f, 0.f, 1.f)).dot(randf3d(xi, yi, zi + 1));
        float v101 = (k - Vec3f(1.f, 0.f, 1.f)).dot(randf3d(xi + 1, yi, zi + 1));

        float v011 = (k - Vec3f(0.f, 1.f, 1.f)).dot(randf3d(xi, yi + 1, zi + 1));
        float v111 = (k - Vec3f(1.f, 1.f, 1.f)).dot(randf3d(xi + 1, yi + 1, zi + 1));

        return Perlin::blend3d(v000, v100, v010, v110,
                               v001, v101, v011, v111, k.x, k.y, k.z);
    }
    
    float getValue3f(float x, float y, float z)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;

        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise3f((n * 51 + x) * freq, (n * 31 + y) * freq, (n * 113 + z) * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }
};

#endif
