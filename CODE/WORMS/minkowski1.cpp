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

int main()
{
    bool exit = false;
    
    init();
    
    int rx1 = 50, ry1 = 100;
    int rx2 = 200, ry2 = 30;
    int cx = 320, cy = 240;

    while(!exit)
    {           
        if(key[KEY_ESC]) exit = true;
 
        clear_to_color(buffer, makecol(0, 0,0));
        
        for(int i = 0; i < 100; ++i)
        {
           float alpha = (i / 100.0) * 2 * M_PI;
           float x = cx + rx1 * cos(alpha);
           float y = cy + ry1 * sin(alpha);
           ellipse(buffer, (int)x, (int)y, rx2, ry2, makecol(0, 0, 100));
        }
        
        
        ellipse(buffer, cx, cy, rx1,  ry1, makecol(255, 0, 0));
        ellipse(buffer, cx, cy, rx1 + rx2,  ry1 + ry2, makecol(255, 0, 0));
        
        //draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
