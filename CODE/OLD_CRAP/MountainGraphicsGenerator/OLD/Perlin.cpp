#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"

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

void drawBrush(BITMAP* buffer, Vec2f p)
{
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

double cosineInterp(double a, double b, double x)
{
	 double ft = x * M_PI;
	 double f = (1.0 - cos(ft)) * 0.5;
	 return  a * (1.0 - f) + b * f;
}

double noise(int32_t x)			 
{
    x = (x << 13) ^ x;
    return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double noise(int32_t x, int32_t y)
{
    int32_t n = x + y * 57;
    
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

double smoothedNoise(int32_t x)
{
    return noise(x) * 0.5 + noise(x - 1) * 0.25 + noise(x + 1) * 0.25;
}

double smoothedNoise(int32_t x, int32_t y)
{
    double corners = (noise(x - 1, y - 1) + noise(x + 1, y - 1) + noise(x - 1, y + 1) + noise(x + 1, y + 1)) * 0.0625;
    double sides   = (noise(x - 1, y) + noise(x + 1, y) + noise(x, y - 1) + noise(x, y + 1)) * 0.125;
    double center  =  noise(x, y) * 0.25;
    
    return corners + sides + center;
}

double smoothedNoise(double x)
{
    int xi = int(x);
    double k = x - xi;

    double v1 = smoothedNoise(xi);
    double v2 = smoothedNoise(xi + 1);
    
    return cosineInterp(v1, v2, k);
}

double smoothedNoise(double x, double y)
{
    int xi = int(x), yi = int(y);
    double kx = x - xi, ky = y - yi;

    double v1 = smoothedNoise(xi, yi);
    double v2 = smoothedNoise(xi + 1, yi);
    double v3 = smoothedNoise(xi, yi + 1);
    double v4 = smoothedNoise(xi + 1, yi + 1);
    
    double i1 = cosineInterp(v1, v2, kx);
    double i2 = cosineInterp(v3, v4, kx);
    
    return cosineInterp(i1, i2, ky);
}

double perlin(double x, double p, int n)
{
   double total = 0.0, freq = 1.0, amp = 1.0;
   
   for(int i = 0; i < n; i++)
   {
      total += amp * smoothedNoise(x * freq);
      amp *= p;
      freq *= 2.0;
   }
   
   return total;
}

double perlin(double x, double y, double p, int n)
{
   double total = 0.0, freq = 1.0, amp = 1.0;
   
   for(int i = 0; i < n; i++)
   {
      total += amp * smoothedNoise(x * freq, y * freq);
      amp *= p;
      freq *= 2.0;
   }
   
   return total;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* heightMap = create_bitmap(512, 512);
    for(int y = 0; y < heightMap->h; y++)
       for(int x = 0; x < heightMap->w; x++)
       {
           int k = clamp255((perlin(x / 20.0, y / 20.0, 0.9, 3) + 1.0) * 127.0);
           PIXEL(heightMap, x, y) = makecol(k, k, k);
       }
    
    
    BITMAP* normalMap = pixelize(heightMap, 20.f, Vec3f(10, 20, -7).unit() );//genNormalMap(heightMap, 50.f);
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
