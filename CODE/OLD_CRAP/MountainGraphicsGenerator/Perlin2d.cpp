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

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* bmp = create_bitmap(256, 256);
    
    Perlin per1(0.45f, 100);
    //per1.setScaleX(4, bmp->w);
    //per1.setScaleY(4, bmp->h);
    
    for(int y = 0; y < bmp->h; y++)
       for(int x = 0; x < bmp->w; x++)
       {   
           //int k = clamp255((per1.getValue3f(v.x / 64.f, v.y / 64.f, v.z / 64.f) + 1.0) * 0.5f * 255.f);
           
           int k = clamp255((per1.getValue2f(x / 32.f, y / 32.f) + 1.0) * 0.5f * 255.f);
           PIXEL(bmp, x, y) = makecol32(k, k, k);
       }
    
    
    
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
