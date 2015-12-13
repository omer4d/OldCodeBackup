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
     
     //set_add_blender(255, 255, 255, 255);
     //drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

void deinit()
{
     destroy_bitmap(buffer);
}

bool isOutlinePixel(BITMAP* src, int x, int y)
{
   return  PIXEL(src, x, y) != 0 &&
             (PIXEL(src, x + 1, y) == 0 ||
              PIXEL(src, x - 1, y) == 0 ||
              PIXEL(src, x, y + 1) == 0 ||
              PIXEL(src, x, y - 1) == 0);
}

int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* terrain = load_bitmap("terrain2.bmp", NULL);
    BITMAP* mink = create_bitmap(terrain->w, terrain->h);
    
    draw_sprite(mink, terrain, 0, 0);
    
    for(int x = 0; x < terrain->w; ++x)
      for(int y = 0; y < terrain->h; ++y)
        if(isOutlinePixel(terrain, x, y))
          ellipse(mink, x, y, 5, 10, makecol(255, 0, 0));
    
    while(!exit)
    {           
        if(key[KEY_ESC]) exit = true;
 
        clear_to_color(buffer, makecol(0, 0,0));
        draw_sprite(buffer, mink, 0, 0);
        
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(terrain);
    deinit();
    return 0;
}END_OF_MAIN()
