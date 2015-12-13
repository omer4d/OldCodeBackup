#ifndef _GAUSSBLUR_HPP_
#define _GAUSSBLUR_HPP_

#include <math.h>
#include "Buffer.hpp"

float gauss(float x, float o)
{
      float t = 2.0f * o * o;
      return 1.0f / sqrt(t * M_PI) * pow(M_E, -(x * x / t));
}

void genGaussKernel(std::vector<float>& kernel, float r)
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

int mod(int x, int n)
{
    int r = x % n;
    if(r < 0)
     r += n;
    return r;
}

template <class T>
void gaussBlur(Buffer<T>& bmp, Buffer<T>& temp, float rad)
{
     std::vector<float> kernel;
     int w = bmp.getWidth(), h = bmp.getHeight();
     
     genGaussKernel(kernel, rad);
     
     for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
      {
       T sum = bmp.at(x, y) * kernel[0];
       
       for(int z = 1; z < kernel.size(); z++)
       {
        T v1 = bmp.at(mod(x + z, w), y);
        T v2 = bmp.at(mod(x - z, w), y);
        
        sum += (v1 + v2) * kernel[z];
       }
       
       temp.at(x, y) = sum;
      }
      
     for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
      {
       T sum = bmp.at(x, y) * kernel[0];

       for(int z = 1; z < kernel.size(); z++)
       {
        T v1 = temp.at(x, mod(y + z, h));
        T v2 = temp.at(x, mod(y - z, h));
        
        sum += (v1 + v2) * kernel[z];
       }
       
       bmp.at(x, y) = sum;
      }
}

#endif
