#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>
#include <functional>

#include "DirCrawler.hpp"

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



int main()
{
    bool exit = false;
    
    init();
    
    DirCrawler::crawlFiles("root/", [](char const* path) {
      printf("%s\n", path);
    });
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));

     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
