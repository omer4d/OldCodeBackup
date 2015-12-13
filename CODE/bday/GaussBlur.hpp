/* 
 * File:   GaussBlur.hpp
 * Author: Stas
 *
 * Created on January 31, 2013, 3:55 AM
 */

#ifndef GAUSSBLUR_HPP
#define	GAUSSBLUR_HPP

#include <vector>
#include <math.h>
#include "Buffer.hpp"

class GaussBlur {
    static float gauss(float x, float o)
    {
        float t = 2.0f * o * o;
        return 1.0f / sqrt(t * M_PI) * pow(M_E, -(x * x / t));
    }

    static void genGaussKernel(std::vector<float>& kernel, float r)
    {
        float sum = 0.f;
        kernel.clear();

        for(int i = 0; i < r; i++)
        {
            kernel.push_back(gauss(i, r / 3.0f));

            sum += kernel[i];
        }

        for(int i = 0; i < r; i++)
            kernel[i] /= sum;
    }
    
    static int mod(int x, int n)
    {
        int r = x % n;
        if(r < 0)
            r += n;
        return r;
    }

public:
    template <class T>
    static void applyW(Buffer<T>& buff, Buffer<T>& temp, float rad)
    {
        std::vector<float> kernel;
        int w = buff.getWidth(), h = buff.getHeight();

        genGaussKernel(kernel, rad);

        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
            {
                T sum = buff.at(x, y) * kernel[0];

                for(int z = 1; z < kernel.size(); z++)
                {
                    T v1 = buff.at(mod(x + z, w), y);
                    T v2 = buff.at(mod(x - z, w), y);

                    sum += (v1 + v2) * kernel[z];
                }

                temp.at(x, y) = sum;
            }

        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
            {
                T sum = buff.at(x, y) * kernel[0];

                for(int z = 1; z < kernel.size(); z++)
                {
                    T v1 = temp.at(x, mod(y + z, h));
                    T v2 = temp.at(x, mod(y - z, h));

                    sum += (v1 + v2) * kernel[z];
                }

                buff.at(x, y) = sum;
            }
    }
       
    
    template <class T>
    static void apply(Buffer<T>& buff, Buffer<T>& temp, float rad)
    {
        std::vector<float> kernel;
        int w = buff.getWidth(), h = buff.getHeight();

        genGaussKernel(kernel, rad);

        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
            {
                T sum = buff.at(x, y) * kernel[0];

                for(int z = 1; z < kernel.size(); z++)
                {
                    T v1 = buff.at(mod(x + z, w), y);
                    T v2 = buff.at(mod(x - z, w), y);

                    sum += (v1 + v2) * kernel[z];
                }

                temp.at(x, y) = sum;
            }

        for(int y = 0; y < h; y++)
            for(int x = 0; x < w; x++)
            {
                T sum = buff.at(x, y) * kernel[0];

                for(int z = 1; z < kernel.size(); z++)
                {
                    T v1 = temp.at(x, mod(y + z, h));
                    T v2 = temp.at(x, mod(y - z, h));

                    sum += (v1 + v2) * kernel[z];
                }

                buff.at(x, y) = sum;
            }
    }
    
    template <class T>
    static void apply(Buffer<T>& buff, float rad)
    {
        Buffer<T> temp(buff.getWidth(), buff.getHeight());
        apply(buff, temp, rad);
    }
};

#endif	/* GAUSSBLUR_HPP */
