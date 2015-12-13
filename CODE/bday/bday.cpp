#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include "NoiseBase.hpp"
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
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

float func(float x, float offs)
{
  float z = (1 - x + offs);
  return 1 - z*z*z;
}

int main()
{   
    bool exit = false;
    
    init();
    
    Perlin perlin[8];
    float t = 0, movx = 0, movy = 0, scale = 3.0;
    
    while(!exit)
    {
     t += 0.02;
     movx += 0.05;
     movy += 0.05;
      //perlin.randomize();
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, 0);
     
     for(int j = 0; j < SCREEN_H; ++j)
       for(int i = 0; i < SCREEN_W; ++i)
       {
          float s = perlin[0].at(movx * 1  + i / 128.0 * scale, movy * 1  + j / 128.0 * scale, t * 0.25) + 
                    perlin[1].at(movx * 2  + i / 64.0 * scale,  movy * 2  + j / 64.0 * scale,  t * 0.5) * 0.5 + 
                    perlin[2].at(movx * 4  + i / 32.0 * scale,  movy * 4  + j / 32.0 * scale,  t) * 0.25 +
                    perlin[3].at(movx * 8  + i / 16.0 * scale,  movy * 8  + j / 16.0 * scale,  t * 2) * 0.125 +
                    perlin[4].at(movx * 16 + i / 8.0 * scale,   movy * 16 + j / 8.0 * scale,   t * 4) * 0.125 * 0.5 +
                    perlin[5].at(movx * 32 + i / 4.0 * scale,   movy * 32 + j / 4.0 * scale,   t * 8) * 0.125 * 0.5 * 0.5;
          
          s = func(s, 0.1);
          
          int k = int((s + 0.5) * 170);
          if(k < 0) k = 0;
          if(k > 255) k = 255;
          PIXEL(buffer, i, j) = makecol32(100 + int(k / 1.6), 128 + k / 2, 255);
       }
       
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
