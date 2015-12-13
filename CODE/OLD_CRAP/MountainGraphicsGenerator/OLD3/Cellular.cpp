#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>
#include <float.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"
#include "Perlin.hpp"
#include "Buffer.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

void wrap(BITMAP* dest, BITMAP* src, int wx, int wy)
{
     for(int j = 0; j < dest->h; j++)
       for(int i = 0; i < dest->w; i++)
          PIXEL(dest, i, j) = PIXEL(src, wrapi(i + wx, 0, src->w), wrapi(j + wy, 0, src->h));
}

struct Square {
       static const int n = 1;
       Vec2f point[n];
       
       Square()
       {
           for(int i = 0; i < Square::n; i++)
              point[i].init(randf(), randf());
       }
       
       /*
       float getMinDist(Vec2f const& v)
       {
          Vec2f delta = v - point[0];
          //return kaka ? fabs(delta.x) : fabs(delta.y);
          return MAX(fabs(delta.x), fabs(delta.y));// + (fabs(delta.x) + fabs(delta.y)) / 1.0;
          //return sqrt(delta.dot(delta));
       }*/
       
       
       
       float getMinDist(Vec2f v)
       {
           float min = FLT_MAX;
           
           for(int i = 0; i < Square::n; i++)
           {
               Vec2f delta = v - point[i];
               //float d = delta.dot(delta);
               float d = fabs(delta.x) + fabs(delta.y);
               
               if(d < min)
                  min = d;
           }
           
           return min;
       }
};

struct Cellular {
       static const int tableSize = 2048;
       
       Square square[tableSize];
       int offs[tableSize];
       
       Cellular()
       {
           for(int i = 0; i < tableSize; i++)
               offs[i] = i;
           
           for(int i = 0; i < tableSize * 3; i++)
               std::swap(offs[rand() % tableSize], offs[rand() % tableSize]);
       }
       
       Square& randomSquare(int32_t x, int32_t y)
       {
           return square[wrapi(y + offs[wrapi(x, 0, tableSize)], 0, tableSize)];
       }
       
       float noise(float x, float y)
       {
           int xi = int(x), yi = int(y);
           Vec2f k(x - xi, y - yi);
           
           float min1 = FLT_MAX;
           float min2 = FLT_MAX;
           
           for(int j = -3; j <= 3; j++)
             for(int i = -3; i <= 3; i++)
             {
               float d = randomSquare(xi + i, yi + j).getMinDist(k - Vec2f(i, j));
               
               if(d < min1)
               {
                  min2 = min1;
                  min1 = d;
               }
               else if(d < min2)
               {
                  min2 = d;
               }
             }
           
           //float dist1 = sqrt(min1);
           //float dist2 = sqrt(min2);
           
           return min2 - min1;    //1.f - (2.f *dist1) / (dist1 + dist2);
       }
       
       float getValue2f(float x, float y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        int n = 2;
        float p = 0.5;
        
        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise((x + n * 31) * freq, (y + n * 51) * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }
};

BITMAP* genHeightMap()
{
    Cellular cell;
    int w = 512, h = 512;
    
    Buffer<float> heightMap(w, h);
    BITMAP* bmp = create_bitmap(w, h);
    
    float min = FLT_MAX, max = -FLT_MAX;
    
    for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
      {
        float h = cell.noise(x / 100.f, y / 100.f);
        if(h < min) min = h;
        if(h > max) max = h;
        heightMap.at(x, y) = h;
      }
      
    for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
      {
        float v = (heightMap.at(x, y) - min) / (max - min);
        
        int k = clamp255(v * 255.f);
        //k = k > 30 ? 100 : k;
        
        /*
         if(k > 45)
            PIXEL(bmp, x, y) = makecol32(100, 100, 100);*/
            
         if(k > 25)
            PIXEL(bmp, x, y) = makecol32(50, 50, 50);
        
        //PIXEL(bmp, x, y) = makecol32(k, k, k);
      }
    
    return bmp;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* bmp = genHeightMap();
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
    

     clear_to_color(buffer, 0);

     draw_sprite(buffer, bmp, 0, 0);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(bmp);
    deinit();
    return 0;
}END_OF_MAIN()
