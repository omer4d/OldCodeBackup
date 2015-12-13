#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include "Vec2f.hpp"

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
    
    Vec2f c(SCREEN_W / 2, SCREEN_H / 2), p;
    float r = 100;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     rest(1);
     
     p.init(mouse_x, mouse_y);
     
     Vec2f delta = p - c;
     float h = delta.length();
     float alpha = acos(r / h);
     Vec2f t1 = c + (delta * (r / h)).rotated(alpha);
     Vec2f t2 = c + (delta * (r / h)).rotated(-alpha);
      
     clear_to_color(buffer, 0);
     
     circlefill(buffer, int(p.x), int(p.y), 2, makecol(255, 0, 0));
     circle(buffer, int(c.x), int(c.y), int(r), makecol(0, 255, 0));
     
     circlefill(buffer, int(t1.x), int(t1.y), 2, makecol(255, 0, 0));
     circlefill(buffer, int(t2.x), int(t2.y), 2, makecol(255, 0, 0));
     
     line(buffer, int(c.x), int(c.y), int(t1.x), int(t1.y), makecol(255, 255, 255));
     line(buffer, int(c.x), int(c.y), int(t2.x), int(t2.y), makecol(255, 255, 255));
     
     line(buffer, int(p.x), int(p.y), int(t1.x), int(t1.y), makecol(255, 255, 255));
     line(buffer, int(p.x), int(p.y), int(t2.x), int(t2.y), makecol(255, 255, 255));
     
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
