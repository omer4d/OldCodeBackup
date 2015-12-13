#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

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



int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* map = load_bitmap("map.bmp", NULL);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;


     clear_to_color(buffer, makecol(128, 128, 128));
     draw_sprite(buffer, map, 0, 0);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(map);
    deinit();
    return 0;
}END_OF_MAIN()
