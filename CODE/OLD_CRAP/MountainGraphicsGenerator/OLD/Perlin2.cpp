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

// *************
// * 1D Perlin *
// *************

float noiseGradient(int32_t x)			 
{
    x = (x << 13) ^ x;
    return (1.f - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.f);
}

float blendFunc(float k)
{
   float k3 = k * k * k;
   float k4 = k3 * k;
   
   return 6.f * k4 * k - 15.f * k4 + 10.f * k3; 
}

float blend(float v1, float v2, float k)
{
   return v1 * blendFunc(1.0 - k) + v2 * blendFunc(k);
}

float noise(float x)
{
   int xi = int(x);
   float k = x - xi;
   
   float v1 = (k - 0.f) * noiseGradient(xi);
   float v2 = (k - 1.f) * noiseGradient(xi + 1);
   
   return blend(v1, v2, k);
}

// *************
// * 2D Perlin *
// *************

Vec2f noiseGradient(int32_t x, int32_t y)
{
    return Vec2f(noiseGradient(x + y * 1024), noiseGradient((x + y * 1024) + 100000));
}

float noise(float x, float y)
{
    int xi = int(x), yi = int(y);
    Vec2f k(x - xi, y - yi);
    
    float v1 = (k - Vec2f(0.f, 0.f)).dot(noiseGradient(xi, yi));
    float v2 = (k - Vec2f(1.f, 0.f)).dot(noiseGradient(xi + 1, yi));
    
    float v3 = (k - Vec2f(0.f, 1.f)).dot(noiseGradient(xi, yi + 1));
    float v4 = (k - Vec2f(1.f, 1.f)).dot(noiseGradient(xi + 1, yi + 1));
    
    float i1 = blend(v1, v2, k.x);
    float i2 = blend(v3, v4, k.x);
    
    return blend(i1, i2, k.y);
}

float perlin(float x, float y, float p, int n)
{
    float total = 0.0, freq = 1.0, amp = 1.0;
   
    for(int i = 0; i < n && amp > 0.001; i++)
    {
      total += amp * noise(x * freq, y * freq);
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
           int k = clamp255((perlin(x / 10.0, y / 10.0, 0.6, 1) + 1.0) * 127.0);
           //int k = clamp255((noise(x / 100.0, y / 100.0) + 1.0) * 127.0 );
           //int k = clamp255((noiseGradient(x + y * heightMap->w) + 1.f) * 127.f);
           PIXEL(heightMap, x, y) = makecol(k, k, k);
       }
    
    int mov = 0;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     //if(!key[KEY_SPACE])
        //mov++;

     clear_to_color(buffer, 0);
     
     draw_sprite(buffer, heightMap, 0, 0);
     
     /*
     for(int i = 0; i < 1000; i++)
     {
         double x = (i + mov) / 100.0;
         double h = noise(x) * 200.0;
         vline(buffer, i, SCREEN_H / 2, int(SCREEN_H / 2 - h), makecol(255, 255, 255));
         
         if(x - int(x) < 0.0001)
           vline(buffer, (i - mov), SCREEN_H / 2 - 20, SCREEN_H / 2 + 20, makecol(255, 0, 0));
     }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(heightMap);
    
    deinit();
    return 0;
}END_OF_MAIN()
