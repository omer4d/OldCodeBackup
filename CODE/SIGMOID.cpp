#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <vector>

#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void deinit()
{
     destroy_bitmap(buffer);
}

float sig(float x, float k)
{
	float d = -k * 0.5f;
	return 1.f / (1.f + pow(2.71f, -(x * k + d)));
}

int main()
{
    bool exit = false;
    
    init();
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     for(float x = 0.f; x < 1.f; x += 0.01f)
     {
     	float y = sig(x, 20.f);
     	putpixel(buffer, int(x * 100), int(-y * 100) + 100, makecol(255, 0, 0));
     }

     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
