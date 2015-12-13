#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include <string>
#include <map>

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
    
    Vec2f c1(100, 100), c2(SCREEN_W / 2, SCREEN_H / 2), v;
    float r1 = 20, r2 = 1;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 2)
       c1.init(mouse_x, mouse_y);
     
     v = Vec2f(mouse_x, mouse_y) - c1;
     
     clear_to_color(buffer, 0);
     
     DrawVec2f::circle(buffer, c1, int(r1), makecol(255, 255, 255));
     DrawVec2f::circle(buffer, c2, int(r2), makecol(255, 0, 0));
     DrawVec2f::line(buffer, c1, c1 + v, makecol(255, 255, 255));
     DrawVec2f::circle(buffer, Vec2f(mouse_x, mouse_y), int(r1), makecol(255, 255, 255));
     
     Vec2f p = c1 - c2;
     float a = v.dot(v);
     float b = 2 * p.dot(v);
     float c = p.dot(p) - (r1 + r2) * (r1 + r2);
     float d = b * b - 4 * a * c;
    
     if(d >= 0)
     {
       float k1 = (-b + sqrt(d)) / (2 * a); 
       float k2 = (-b - sqrt(d)) / (2 * a);
       float k = MIN(k1, k2);
      
       if(k >= 0 && k <= 1)
         DrawVec2f::circle(buffer, c1 + v * k, int(r1), makecol(0, 255, 255));
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
