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
#define WPIXEL(bmp, x, y) ((long*)(bmp)->line[wrapi(y, 0, bmp->h)])[wrapi(x, 0, bmp->w)]

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

float prng1(int32_t x)			 
{
    x = (x << 13) ^ x;
    return (1.f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.f);
}

float prng2(int32_t x)			 
{
    x = (x << 13) ^ x;
    return (1.f - ((x * (x * x * 79357 + 103867) + 104683) & 0x7fffffff) / 1073741824.f);
}

float cubic(float v0, float v1, float v2, float v3, float x)
{
	  float P = (v3 - v2) - (v0 - v1);
	  float Q = (v0 - v1) - P;
	  float R = v2 - v0;
	  float S = v1;
	  float x2 = x * x;
	  
	  return P*x2*x + Q*x2 + R*x + S;
}

float func(float a, float b, float c, float d, float k)
{
   
}

void normalizeHeightMap(Buffer<float>& map, float min, float max)
{
     for(int y = 0; y < map.getHeight(); y++)
       for(int x = 0; x < map.getWidth(); x++)
          map.at(x, y) = (map.at(x, y) - min) / (max - min);
}

BITMAP* genHeightMap(int w, int h)
{
    Buffer<float> map(w, h);
    
    Perlin perlin1(prng1, 0.6, 100);
    Perlin perlin2(prng2, 0.6, 100);
    float min = FLT_MAX, max = -FLT_MAX;
    
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
          float v = perlin1.getValue(x / 200.0, y / 200.0);
          
          if(v < min) min = v;
          if(v > max) max = v;
          
          map.at(x, y) = v;
       }
       
    normalizeHeightMap(map, min, max);
    
    BITMAP* bmp = create_bitmap(w, h);
    
    for(int y = 0; y < h; y++)
       for(int x = 0; x < w; x++)
       {
           int k = clamp255(map.at(x, y) * 255.f);
           PIXEL(bmp, x, y) = makecol32(k, k, k);
       }
    
    return bmp;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* heightMap = genHeightMap(512, 512);
    
    
    /*create_bitmap(512, 512);
    
    Perlin per1(prng1, 0.4, 100);
    Perlin per2(prng2, 0.5, 100);
    
    for(int y = 0; y < heightMap->h; y++)
       for(int x = 0; x < heightMap->w; x++)
       {
           float v = per2.getValue(x / 100.0, y / 100.0);
        
           int k = clamp255((v + 1.0) * 127.0);
           //int k = clamp255((noise(x / 100.0, y / 100.0) + 1.0) * 127.0 );
           //int k = clamp255((noiseGradient(x + y * heightMap->w) + 1.f) * 127.f);
           PIXEL(heightMap, x, y) = makecol(k, k, k);
       }*/
    
    BITMAP* normalMap = illuminate(heightMap, 20.f, Vec3f(10, 20, -7).unit() );//genNormalMap(heightMap, 50.f);
    ///BITMAP* normalMap = genNormalMap(heightMap, 50.f);
    

    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0);
     
     draw_sprite(buffer, heightMap, 0, 0);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     /*
     for(int i = 0; i < 1000; i++)
     {
        float h = perlin(i / 100.f, 0.5, 10) * 50.0; //smoothedNoise(i / 10.f) * 100;
        
        vline(buffer, 100 + i, int(SCREEN_H / 2 - 100 - h), SCREEN_H / 2 - 100, makecol(255, 255, 255));
     }*/
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(normalMap);
    destroy_bitmap(heightMap);
    
    deinit();
    return 0;
}END_OF_MAIN()
