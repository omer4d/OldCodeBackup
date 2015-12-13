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
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

void smooth(Buffer<float>& dest, Buffer<float> const& src)
{
   int w = MIN(dest.getWidth(), src.getWidth());
   int h = MIN(dest.getHeight(), src.getHeight());
 
   int vx, vy = 1;
 
   for(int j = 1; j < h - 1; ++j)
     for(int i = 1; i < w - 1; ++i)
       dest.at(i, j) = (src.wat(i, j + vy) + src.wat(i + 1, j + vy) + src.wat(i - 1, j + vy) +
                       src.wat(i, j + 1 + vy) + src.wat(i, j - 1 + vy)) * 0.2 - 4;
}

int main()
{   
    bool exit = false;
    
    init();
    
    Buffer<float> buff1(SCREEN_W, SCREEN_H), buff2(SCREEN_W, SCREEN_H);
    Buffer<float>* curr = &buff1;
    Buffer<float>* next = &buff2;
    Perlin perlin;
    float t = 0;
    
    while(!exit)
    {
      perlin.randomize();
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, 0);
     
     for(int i = 0; i < curr->getWidth(); ++i)
        curr->at(i, curr->getHeight() - 2) = (perlin.at(i * 0.01) + 0.5) * 255;//randf(0, 255);
     
     smooth(*next, *curr);
     
     Buffer<float>* temp = curr;
     curr = next;
     next = temp;
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     for(int j = 0; j < curr->getHeight(); ++j)
       for(int i = 0; i < curr->getWidth(); ++i)
       {
          int k = int(curr->at(i, j));
          
          if(k < 0)
            k = 0;
            if(k > 255)
            k = 255;
          
          putpixel(buffer, i, j, makecol(k, k, k));
       }
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
