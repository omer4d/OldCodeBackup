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
        
        for(int i = 0; i < r + 1; i++)
            kernel.push_back(gauss(i, r / 2.0f));
        
        sum = kernel[0];
        for(int i = 1; i < kernel.size(); i++)
           sum += kernel[i] * 2;
        
        for(int i = 0; i < kernel.size(); i++)
           kernel[i] /= sum;
    }
    
    /*
    static void genGaussKernel(std::vector<float>& kernel, int n)
    {
        float prev = 1.f;
        float sum = pow(2, n - 1);
        int size = n / 2 + 1;
     
        kernel.clear();
        kernel.assign(size, 0.f);
        
        for(int i = 0; i < size; i++)
        {
            kernel[size - 1 - i] = prev / sum;
            prev *= float(n - 1 - i) / float(i + 1);
        }
    }*/
    
    
    /*
    static void genGaussKernel(std::vector<float>& kernel, int n)
    {
        int size = n + 1;
        
        kernel.clear();
        kernel.assign(size, 0.f);
        
        for(int i = 0; i < size; i++)
        {
            kernel[i] = 1.f / float(n * 2 + 1);
        }
    }*/
    
    /*
    static void genGaussKernel(std::vector<float>& kernel, int n)
    {
        int size = n + 1;
        float prev = 1.f;
        float sum = pow(2, n * 2);
        
        
        kernel.clear();
        kernel.assign(size, 0.f);
     
        
        for(int i = 0; i < size; i++)
        {
            kernel[size - 1 - i] = prev / sum;
            prev *= float((size - 1) * 2 - i) / float(i + 1);
        }
    }*/
    
    static int mod(int x, int n)
    {
        int r = x % n;
        if(r < 0)
            r += n;
        return r;
    }

public:
    
    template <class T>
    static void apply(Buffer<T>& buff, Buffer<T>& temp, int rad)
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
                T sum = temp.at(x, y) * kernel[0];

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
