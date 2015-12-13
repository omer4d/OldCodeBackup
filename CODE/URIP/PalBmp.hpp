#ifndef _PAL_BMP_HPP_
#define _PAL_BMP_HPP_

#include <vector>
#include <map>
#include <allegro.h>

#include "Buffer.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

struct PalBmp {
    Buffer<int> buff;
    std::vector<int> pal;
    
    PalBmp(BITMAP* src): buff(src->w, src->h)
    {
       std::map<int, int> colToPal;
       
       for(int y = 0; y < src->h; ++y)
         for(int x = 0; x < src->w; ++x)
         {
            int col = PIXEL(src, x, y);
            auto iter = colToPal.find(col);
            
            if(iter == colToPal.end()) // Found a new color?
            {
               pal.push_back(col);
               colToPal[col] = pal.size() - 1;
               buff.at(x, y) = pal.size() - 1;
            }
          
            else
               buff.at(x, y) = iter->second;
         }
         
         printf("Pal size: %d\n", pal.size());
    }
    
    void draw(BITMAP* dest) const
    {
        for(int y = 0; y < buff.getHeight(); ++y)
          for(int x = 0; x < buff.getWidth(); ++x)
          {
              PIXEL(dest, x, y) = pal[buff.at(x, y)];
          }
    }
    
    std::vector<int> getRow(int y) const
    {
        std::vector<int> vec;
        vec.reserve(buff.getWidth());
        
        for(int x = 0; x < buff.getWidth(); ++x)
          vec.push_back(buff.at(x, y));
          
        return vec;
    }
    
    std::vector<int> getCol(int x) const
    {
        std::vector<int> vec;
        vec.reserve(buff.getHeight());
        
        for(int y = 0; y < buff.getHeight(); ++y)
          vec.push_back(buff.at(x, y));
        
        return vec;
    }
};

#endif
