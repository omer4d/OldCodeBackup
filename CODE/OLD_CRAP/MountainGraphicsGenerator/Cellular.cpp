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

#include "GaussBlur.hpp"
#include "ClipRect.hpp"

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
       int n;
       std::vector<Vec2f> point;
       
       Square()
       {
           n = 1 + rand() % 6;
           for(int i = 0; i < n; i++)
              point.push_back(Vec2f(randf(), randf()));
       }
       
       /*
       float getMinDist(Vec2f const& v)
       {
          Vec2f delta = v - point[0];
          return delta.dot(delta);
       }*/
       
       
       float getMinDist(Vec2f v)
       {
           float min = FLT_MAX;
           
           for(int i = 0; i < n; i++)
           {
               Vec2f delta = v - point[i];
               float d = delta.dot(delta);
               //float d = fabs(delta.x) + fabs(delta.y);
               
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
               else if(d < min2) min2 = d;
             }
           
           float dist1 = sqrt(min1);
           float dist2 = sqrt(min2);
           
           return dist2 - dist1;
       }
       
       float getValue2f(float x, float y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        int n = 5;
        float p = 0.3;
        
        for(int i = 0; i < n && amp > 0.001; i++)
        {
            total += amp * noise((x + n * 31) * freq, (y + n * 51) * freq);
            amp *= p;
            freq *= 2.0;
        }

        return total;
    }
};

Buffer<float>* genHeightMap()
{
    Cellular cell;
    int w = 512, h = 512;
    
    Buffer<float>* heightMap = new Buffer<float>(w, h);
    Buffer<float> temp(512, 512);
    
    float min = FLT_MAX, max = -FLT_MAX;
    
    Perlin perlin(0.5, 2);
    
    for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
      {
        float h = cell.getValue2f(x / 128.f, y / 128.f);
        if(h < min) min = h;
        if(h > max) max = h;
        heightMap->at(x, y) = h;
      }
      
    for(int y = 0; y < h; y++)
      for(int x = 0; x < w; x++)
      {
        float v = (heightMap->at(x, y) - min) / (max - min);
        v *= (2.f + perlin.getValue2f(x / 15.f, y / 15.f)) * 0.5f;
        
        heightMap->at(x, y) = v * 255.f;
      }
    
    gaussBlur<float>(*heightMap, temp, 3.f);
    
    return heightMap;
}

Vec3f getVertex(Buffer<float> const& heightMap, int x, int y, float k)
{
    return Vec3f(x, y, heightMap.wat(x, y) / 255.f * k);
}


void cellShaded(BITMAP* target, Buffer<float> const& heightMap, float k, Vec3f dir)
{   
    int w = heightMap.getWidth(), h = heightMap.getHeight();
    
    ClipRect clip = ClipRect::intersect(ClipRect(0, 0, heightMap.getWidth(), heightMap.getHeight()),
                                        ClipRect(0, 0, target->w, target->h));
    
    for(int j = clip.miny; j < clip.maxy; j++)
       for(int i = clip.minx; i < clip.maxx; i++)
       {
        
             Vec3f o = getVertex(heightMap, i, j, k);
             Vec3f a = getVertex(heightMap, i, j - 1, k);
             Vec3f b = getVertex(heightMap, i + 1, j, k);
             Vec3f c = getVertex(heightMap, i, j + 1, k);
             Vec3f d = getVertex(heightMap, i - 1, j, k);
             
             Vec3f n = (Vec3f::unitNormal(a, b, o) +
                        Vec3f::unitNormal(b, c, o) + 
                        Vec3f::unitNormal(c, d, o) + 
                        Vec3f::unitNormal(d, a, o)) * 0.25f;
             
             n.normalize();
                      
             int br = //clamp255(n.dot(dir) * 255.f);  
             
             /*
             if(br > 60)
                PIXEL(target, i, j) = makecol32(128, 128, 128);
             else
               PIXEL(target, i, j) = makecol32(90, 90, 90);
             */
             
             PIXEL(target, i, j) = makecol32(br, br, br);
       }
}


int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* bmp = create_bitmap(512, 512);
    Buffer<float>* heightMap = genHeightMap();
    
    cellShaded(bmp, *heightMap, 50.f, Vec3f(10.f, 20.f, 7.f).unit());
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
    

     clear_to_color(buffer, 0);

     draw_sprite(buffer, bmp, 0, 0);
     
     //draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(bmp);
    deinit();
    return 0;
}END_OF_MAIN()
