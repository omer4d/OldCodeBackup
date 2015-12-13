#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <vector>
#include <algorithm>

#include "Vec2f.hpp"
#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     set_add_blender(255, 255, 255, 255);
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

void deinit()
{
     destroy_bitmap(buffer);
}

int main()
{
    bool exit = false;
    
    init();

    while(!exit)
    {           
        if(key[KEY_ESC]) exit = true;
 
        clear_to_color(buffer, makecol(0, 0,0));
        
        int tx = 10, ty = 0;
        
        for(int y1 = -20+ty; y1 < 21+ty; ++y1)
           for(int x1 = -20+tx; x1 < 21+tx; ++x1)
              if((x1-tx) * (x1-tx) + (y1-ty) * (y1-ty) <= 20 * 20)
                putpixel(buffer, x1, y1, makecol(255, 0, 0)); 
                
        for(int y1 = -20+ty; y1 < 21+ty; ++y1)
           for(int x1 = -20+tx; x1 < 21+tx; ++x1)
              if((x1-tx) * (x1-tx) + (y1-ty) * (y1-ty) <= 20 * 20)
              {
                 for(int y2 = 100; y2 < 150; ++y2)
                   for(int x2 = 100; x2 < 200; ++x2)
                   {
                     putpixel(buffer, x2 - x1, y2 - y1, makecol(0, 255, 0));
                     putpixel(buffer, x2 + x1, y2 + y1, makecol(0, 0, 255));
                   }
              }
              
        for(int y2 = 100; y2 < 150; ++y2)
           for(int x2 = 100; x2 < 200; ++x2)
                putpixel(buffer, x2, y2, makecol(255, 0, 0));
        
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
