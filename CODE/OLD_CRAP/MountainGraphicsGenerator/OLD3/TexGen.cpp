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

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 800, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}


struct Square {
       static const int n = 1;
       Vec2f point[n];
       
       Square()
       {
           for(int i = 0; i < Square::n; i++)
              point[i].init(randf(), randf());
       }
       
       float getMinDist(Vec2f const& v)
       {
          Vec2f delta = v - point[0];
          return delta.dot(delta);
       }
       
       /*
       float getMinDist(Vec2f v)
       {
           float min = FLT_MAX;
           
           for(int i = 0; i < Square::n; i++)
           {
               Vec2f delta = v - point[i];
               float d = delta.dot(delta);
               //float d = fabs(delta.x) + fabs(delta.y);
               
               if(d < min)
                  min = d;
           }
           
           return min;
       }*/
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
           
           return 1.f - (2.f *dist1) / (dist1 + dist2);
       }
       
    float getValue2f(float x, float y)
    {
        float total = 0.0, freq = 1.0, amp = 1.0;
        int n = 100;
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


void normalizeHeightMap(Buffer<float>& map, float min, float max)
{
     for(int y = 0; y < map.getHeight(); y++)
       for(int x = 0; x < map.getWidth(); x++)
          map.at(x, y) = (map.at(x, y) - min) / (max - min);
}

float contrast(float v, float br, float cont)
{   
    v -= (1.f - br);
    if(v < 0.f) v = 0.f;
    return 1.f - pow(1 - cont, v);
}

float clampf(float x)
{
   if(x < 0.f) return 0.f;
   if(x > 1.f) return 1.f;
   return x;
}

BITMAP* genHeightMap(int w, int h)
{
    Buffer<float> map(w, h);
    Buffer<float> temp(w, h);
    
    float min = FLT_MAX, max = -FLT_MAX;
    
    Cellular cell;
    
    
    //normalizeHeightMap(map, min, max);
    
    
/*
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
          float v = cell.getValue2f(x / 50.0, y / 50.0);
          
          //v=sqrt(v);
          
          if(v < min) min = v;
          if(v > max) max = v;
          
          map.at(x, y) = v;
       }
       
       gaussBlur<float>(map, temp, 8.f);
       normalizeHeightMap(map, min, max);*/
       
       
       
       
       for(int i = 0; i < 1; i++)
    {
     Perlin perlin1(0.7, 100);
     
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
          float v = perlin1.getValue2f(x / 300.0, y / 300.0);
          
          if(v < min) min = v;
          if(v > max) max = v;
          
          map.at(x, y) = v; //fabs(map.at(x, y) - clampf((v + 1.f) * 0.5f));
       }
    }
    
       
             normalizeHeightMap(map, min, max);
    
    
    BITMAP* bmp = create_bitmap(w, h);
    
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
           int k = clamp255(contrast(map.at(x, y), 1.f, 0.9f) * 255.f);
           PIXEL(bmp, x, y) = makecol32(k, k, k);
       }
    
    return bmp;
}

float getHeight(BITMAP* bmp, int x, int y)
{
    int h = ((long*)(bmp)->line[wrapi(y, 0, bmp->h)])[wrapi(x, 0, bmp->w)];
    return getr32(h) / 255.f;
}

Vec3f getVertex(BITMAP* bmp, int x, int y, float k)
{
    return Vec3f(x, y, getHeight(bmp, x, y) * k);
}

BITMAP* genNormalMap(BITMAP* heightMap, float k)
{
    BITMAP* normalMap = create_bitmap(heightMap->w, heightMap->h);
    
    for(int j = 0; j < normalMap->h; j++)
       for(int i = 0; i < normalMap->w; i++)
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
                     
          Vec3f v = (Vec3f(1, 1, 1) - n) * 0.5f;
          PIXEL(normalMap, i, j) = makecol32(int(v.x * 255.f), int(v.y * 255.f), int(v.z * 255.f));
       }
       
    return normalMap;
}

BITMAP* illuminate(BITMAP* heightMap, float k, Vec3f dir)
{
    BITMAP* normalMap = create_bitmap(heightMap->w, heightMap->h);
    
    for(int j = 0; j < normalMap->h; j++)
       for(int i = 0; i < normalMap->w; i++)
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
                     
          int br = clamp255(n.dot(dir) * 255.f);
          
          PIXEL(normalMap, i, j) = makecol32(br, br, br);
       }
       
    return normalMap;
}

BITMAP* pixelize(BITMAP* heightMap, float k, Vec3f dir)
{
    BITMAP* normalMap = create_bitmap(heightMap->w, heightMap->h);
    
    for(int j = 0; j < normalMap->h; j++)
       for(int i = 0; i < normalMap->w; i++)
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
                     
          int br = clamp255(n.dot(dir) * 255.f);
          
          if(br > 120)
              PIXEL(normalMap, i, j) = makecol32(128, 128, 128);
          else if(br > 70)
              PIXEL(normalMap, i, j) = makecol32(80, 80, 80);
          else
              PIXEL(normalMap, i, j) = makecol32(32, 32, 32);
       }
       
    return normalMap;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* heightMap = genHeightMap(512, 512);
    BITMAP* result = illuminate(heightMap, 20.f, Vec3f(10, 20, -7).unit());
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0);
     
     if(key[KEY_SPACE])
     draw_sprite(buffer, heightMap, 0, 0);
     else
     draw_sprite(buffer, result, 0, 0);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(result);
    destroy_bitmap(heightMap);
    
    deinit();
    return 0;
}END_OF_MAIN()
