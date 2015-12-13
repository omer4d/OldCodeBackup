#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

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

void ray(BITMAP* buffer, Vec2f o, Vec2f d)
{
     float tmaxx, tmaxy, tdx, tdy;
     int stepx = d.x < 0 ? -1 : 1, stepy = d.y < 0 ? -1 : 1;
}

int main()
{
    bool exit = false;
    
    init();

    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     /* 
     float res = 0;
     for(int i = 0; i < 128; i++)
      for(int j = 0; j < 128; j++)
       for(int k = 0; k < 180; k++)
       {
         for(int z = 0; z < 10; z++)
              res += i * i * i / k + res + j / k;
        }*/
        
     Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
     Vec2f m(mouse_x, mouse_y);
     Vec2f d = m - c;
     Vec2f n = d.normal() * 200.0;
        
     Vec2f p1(-100, -100),
           p2(100, -100),
           p3(100, 100),
           p4(-100, 100);
       
     p1 += c;
     p2 += c;
     p3 += c;
     p4 += c;
     
     clear_to_color(buffer, 0);
     
     rect(buffer, int(p1.x), int(p1.y), int(p3.x), int(p3.y), makecol(255, 0, 0));
     DrawVec2f::line(buffer, m, c, makecol(0, 255, 0));
     DrawVec2f::line(buffer, m, m + n, makecol(0, 255, 0));
     DrawVec2f::line(buffer, m, m - n, makecol(0, 255, 0));
     
     if(d.x * d.y < 0.0)
     {
      DrawVec2f::line(buffer, p1, p1 + d * 200, makecol(0, 255, 0));
      DrawVec2f::line(buffer, p3, p3 + d * 200, makecol(0, 255, 0));
      DrawVec2f::circlefill(buffer, p1, 2, makecol(255, 0, 0));
      DrawVec2f::circlefill(buffer, p3, 2, makecol(255, 0, 0));
     }
     
     else
     {
      DrawVec2f::line(buffer, p2, p2 + d * 200, makecol(0, 255, 0));
      DrawVec2f::line(buffer, p4, p4 + d * 200, makecol(0, 255, 0));
      DrawVec2f::circlefill(buffer, p2, 2, makecol(255, 0, 0));
      DrawVec2f::circlefill(buffer, p4, 2, makecol(255, 0, 0));
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
