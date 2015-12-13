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
#include "Perlin.hpp"

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

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* heightMap = create_bitmap(512, 512);
    Perlin per1(prng1, 0.6, 100);
    Perlin per2(prng2, 0.4, 100);
    
    for(int y = 0; y < heightMap->h; y++)
       for(int x = 0; x < heightMap->w; x++)
       {
           float v = per1.getValue(x / 100.0, y / 100.0) + 
                     per2.getValue(x / 10.0, y / 10.0) * 0.5;
        
           int k = clamp255((v + 1.0) * 127.0);
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
