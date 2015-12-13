/* 
 * File:   FloatMap.hpp
 * Author: Stas
 *
 * Created on February 2, 2013, 4:01 AM
 */

#ifndef FLOATMAP_HPP
#define	FLOATMAP_HPP

#include "Buffer.hpp"
#include <allegro.h>

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

class FloatMap : public Buffer<float> {
    
public:
    typedef float BlendFunc(float a, float b, float k);
    
    static float alpha(float a, float b, float k)
    {
        return a + (b - a) * k;
    }
    
    static float mul(float a, float b, float k)
    {
        return clampf(alpha(a, a * b, k));
    }
    
    static float sum(float a, float b, float k)
    {
        return alpha(a, a + b, k);
    }
    
    static float diff(float a, float b, float k)
    {
        return alpha(a, fabs(a - b), k);
    }
    
    FloatMap(int w, int h): Buffer<float>(w, h)
    {
    }
    
    FloatMap(Buffer<float> const& src): Buffer<float>(src)
    {
    }
    
    FloatMap deepCopy()
    {
        FloatMap res(getWidth(), getHeight());
        
        for(int j = 0; j < getHeight(); j++)
           for(int i = 0; i < getWidth(); i++)
              res.at(i, j) = at(i, j);
        
        return res;
    }
    
    void normalize(float min, float max)
    {
        for(int j = 0; j < getHeight(); j++)
            for(int i = 0; i < getWidth(); i++)
                at(i, j) = (at(i, j) - min) / (max - min);
    }

    void draw(BITMAP* dest)
    {
        int w = MIN(dest->w, getWidth());
        int h = MIN(dest->h, getHeight());

        for(int j = 0; j < h; j++)
            for(int i = 0; i < w; i++)
            {
                int br = clamp255(at(i, j) * 255.f);
                PIXEL(dest, i, j) = makecol32(br, br, br);
            }
    }
    
    void blend(FloatMap const& a, FloatMap const& b, float k=0.5f, BlendFunc* func=&FloatMap::alpha)
    {
        int w = MIN(getWidth(), MIN(a.getWidth(), b.getWidth()));
        int h = MIN(getHeight(), MIN(a.getHeight(), b.getHeight()));
        
        for(int j = 0; j < h; j++)
            for(int i = 0; i < w; i++)
                at(i, j) = func(a.at(i, j), b.at(i, j), k);
    }
};

#endif	/* FLOATMAP_HPP */

