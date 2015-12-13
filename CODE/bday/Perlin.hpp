/* 
 * File:   Perlin.hpp
 * Author: Stas
 *
 * Created on January 31, 2013, 8:04 AM
 */

#ifndef PERLIN_HPP
#define	PERLIN_HPP

#include <algorithm>

#include "NoiseBase.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "Util.hpp"

using namespace Util;

class Perlin: public Noise1D, public Noise2D, public Noise3D {
    static const int tableSize = 2048 * 2;
    typedef unsigned int uint;
    
    float* grad1d;
    Vec2f* grad2d;
    Vec3f* grad3d;

    int* offs1;
    int* offs2;
    
    static float blendFunc(float k)
    {
        float k3 = k * k * k;
        float k4 = k3 * k;
        float f = 6.f * k4 * k - 15.f * k4 + 10.f * k3;

        return f;
    }

    static float blend(float v1, float v2, float k)
    {
        return v1 * blendFunc(1.f - k) + v2 * blendFunc(k);
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
    
    float rand1f(int x) const
    {
        int s = Perlin::tableSize - 1;
        return grad1d[uint(x) & s];
    }
    
    Vec2f const& rand2f(int x, int y) const
    {
        int s = Perlin::tableSize - 1;
        return grad2d[uint(y + offs1[uint(x) & s]) & s];
    }

    Vec3f const& rand3f(int x, int y, int z) const
    {
        int s = Perlin::tableSize - 1;
        return grad3d[uint(z + offs1[uint(y + offs2[uint(x) & s]) & s]) & s];
    }
    
public:
    void randomize()
    {
        int tsize = Perlin::tableSize;
        
        for(int i = 0; i < tsize * 3; i++)
        {
            std::swap<float>(grad1d[rand() % tsize], grad1d[rand() % tsize]);
            std::swap<Vec2f > (grad2d[rand() % tsize], grad2d[rand() % tsize]);
            std::swap<Vec3f > (grad3d[rand() % tsize], grad3d[rand() % tsize]);
            
            std::swap<int> (offs1[rand() % tsize], offs1[rand() % tsize]);
            std::swap<int> (offs2[rand() % tsize], offs2[rand() % tsize]);
        }
    }
    
    Perlin()
    {
        int tsize = Perlin::tableSize;
        
        offs1 = new int[tsize];
        offs2 = new int[tsize];
        grad1d = new float[tsize];
        grad2d = new Vec2f[tsize];
        grad3d = new Vec3f[tsize];
        
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
        
        randomize();
    }
    
    ~Perlin()
    {
        delete[] offs1;
        delete[] offs2;
        delete[] grad1d;
        delete[] grad2d;
        delete[] grad3d;
    }
    
    float at(float x)  const
    {
        int xi = int(x);
        float k = x - xi;
        
        if(x < 0.f) xi -= 1, k += 1.f;
        
        float v1 = k * rand1f(xi);
        float v2 = (k - 1.f) * rand1f(xi + 1);
        
        return Perlin::blend(v1, v2, k);
    }
    
    float at(float x, float y) const
    {
        int xi = int(x), yi = int(y);
        Vec2f k(x - xi, y - yi);
        
        if(x < 0.f) xi -= 1, k.x += 1.f;
        if(y < 0.f) yi -= 1, k.y += 1.f;
        
        float v00 = k.dot(rand2f(xi, yi));
        float v10 = (k - Vec2f(1.f, 0.f)).dot(rand2f(xi + 1, yi));
        
        float v01 = (k - Vec2f(0.f, 1.f)).dot(rand2f(xi, yi + 1));
        float v11 = (k - Vec2f(1.f, 1.f)).dot(rand2f(xi + 1, yi + 1));
        
        return Perlin::blend2d(v00, v10, v01, v11, k.x, k.y);
    }
    
    float at(float x, float y, float z) const
    {
        int xi = int(x), yi = int(y), zi = int(z);
        Vec3f k(x - xi, y - yi, z - zi);
        
        if(x < 0.f) xi -= 1, k.x += 1.f;
        if(y < 0.f) yi -= 1, k.y += 1.f;
        if(z < 0.f) zi -= 1, k.z += 1.f;
        
        float v000 = k.dot(rand3f(xi, yi, zi));
        float v100 = (k - Vec3f(1.f, 0.f, 0.f)).dot(rand3f(xi + 1, yi, zi));

        float v010 = (k - Vec3f(0.f, 1.f, 0.f)).dot(rand3f(xi, yi + 1, zi));
        float v110 = (k - Vec3f(1.f, 1.f, 0.f)).dot(rand3f(xi + 1, yi + 1, zi));
        
        float v001 = (k - Vec3f(0.f, 0.f, 1.f)).dot(rand3f(xi, yi, zi + 1));
        float v101 = (k - Vec3f(1.f, 0.f, 1.f)).dot(rand3f(xi + 1, yi, zi + 1));

        float v011 = (k - Vec3f(0.f, 1.f, 1.f)).dot(rand3f(xi, yi + 1, zi + 1));
        float v111 = (k - Vec3f(1.f, 1.f, 1.f)).dot(rand3f(xi + 1, yi + 1, zi + 1));
        
        return Perlin::blend3d(v000, v100, v010, v110,
                               v001, v101, v011, v111, k.x, k.y, k.z);
    }
};

#endif	/* PERLIN_HPP */

