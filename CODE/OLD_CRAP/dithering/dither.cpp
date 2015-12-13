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
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

float randomDither(float val, int levels)
{
    float step = 1.f / float(levels);
    float nearest = int(val / step) * step;
    float error = (val - nearest) / step;
    
    return randf() > error ? nearest : (nearest + step);
}

int thresh(int in, int level)
{
   return in < level ? 0 : 255;
}

int lum(int col)
{
   return int(0.2126 * getr(col) + 0.7152 * getg(col) + 0.0722 * getb(col));
}

int clamp0255(int in)
{
   if(in < 0) return 0;
   else if(in > 255) return 255;
   else return in;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* image = load_bitmap("lena.bmp", NULL);
    Buffer<float> error(image->w, image->h);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
    

     clear_to_color(buffer, 0);

     error.fill(0);

     for(int j = 0; j < image->h - 1; ++j)
     {
        //int error = 0;
        
      
        for(int i = 0; i < image->w - 1; ++i)
        {
           //float c = 
         
           //int br = int(randomDither(lum(PIXEL(image, i, j)), 10) * 255);
           //int br = int(thresh(lum(PIXEL(image, i, j)), 0.5) * 255);
           
           int l0 = int(lum(PIXEL(image, i, j)) + error.at(i, j) * 0.1);
           int l1 = thresh(l0, 128);
           
           error.at(i + 1, j) += ((l0 - l1) / 2);
           error.at(i, j + 1) += ((l0 - l1) / 2);
           
           PIXEL(buffer, i, j) = makecol(l1, l1, l1);
        }
     }

     //draw_sprite(buffer, image, 0, 0);
     //draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(image);
    deinit();
    return 0;
}END_OF_MAIN()
