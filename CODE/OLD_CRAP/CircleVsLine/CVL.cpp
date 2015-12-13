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
    
    Vec2f orig(300.f, 300.f);
    Vec2f p(300.f, 300.f);
    float r = 100.f;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     Vec2f dir = (Vec2f(mouse_x, mouse_y) - orig).unit();
     
     clear_to_color(buffer, 0);
     
     DrawVec2f::circle(buffer, p, int(r), makecol(0, 255, 255));
     DrawVec2f::line(buffer, orig, orig + dir * 1000.f, makecol(255, 255, 255));
     
     
     Vec2f pr = p - orig;
     float a = dir.dot(dir);
     float b = -2.f * dir.dot(pr);
     float c = pr.dot(pr) - r * r;
     float d = b * b - 4 * a * c;
    
     if(d >= 0)
     {
       float sqrtd = sqrt(d);
       float k1 = (-b + sqrtd) / (2 * a); 
       float k2 = (-b - sqrtd) / (2 * a);
      
       if(k2 >= 0.f)
          DrawVec2f::circlefill(buffer, orig + dir * k2, 2, makecol(255, 0, 0));
       else if(k1 >= 0.f)
          DrawVec2f::circlefill(buffer, orig + dir * k1, 2, makecol(255, 0, 0));
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
