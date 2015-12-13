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
     
     //install_timer();
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

bool contained(float i1, float i2, float p)
{
 return fabs(p - i1) + fabs(p - i2) <= fabs(i2 - i1);
}

bool lineVsAABB(Vec2f p1, Vec2f p2, Vec2f boxMin, Vec2f boxMax)
{
 Vec2f lineMin, lineMax, n = (p2 - p1).normal();
 
 lineMin.x = p1.x < p2.x ? p1.x : p2.x;
 lineMin.y = p1.y < p2.y ? p1.y : p2.y;
 lineMax.x = p1.x > p2.x ? p1.x : p2.x;
 lineMax.y = p1.y > p2.y ? p1.y : p2.y;
 
 if(boxMin.x > lineMax.x || boxMin.y > lineMax.y || lineMin.x > boxMax.x || lineMin.y > boxMax.y)
  return false;
 
 if(n.x * n.y > 0)
 {
  float i1 = boxMin.dot(n), i2 = boxMax.dot(n), p = p1.dot(n);
  return contained(i1, i2, p);
 }
 
 else
 {
  float i1 = Vec2f(boxMax.x, boxMin.y).dot(n), i2 = Vec2f(boxMin.x, boxMax.y).dot(n), p = p1.dot(n);
  return contained(i1, i2, p);
 }
}

int main()
{
    bool exit = false;
    
    init();

    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
     Vec2f m(mouse_x, mouse_y);
     Vec2f d = m - c;
     Vec2f n = d.normal() * 200.0;
      
      /*
      bool crap;
     float res = 0;
     for(int i = 0; i < 128; i++)
      for(int j = 0; j < 128; j++)
       for(int k = 0; k < 180; k++)
       {
         for(int z = 0; z < 10; z++)
         crap = test(i, z, k);
        }
        
        printf("%d", crap);*/

        
     Vec2f p1(-100, -100),
           p2(100, -100),
           p3(100, 100),
           p4(-100, 100);
       
     p1 += c;
     p2 += c;
     p3 += c;
     p4 += c;
     
     printf("%d", lineVsAABB(c, m, p1, p3));
     
     clear_to_color(buffer, 0);
     
     rect(buffer, int(p1.x), int(p1.y), int(p3.x), int(p3.y), makecol(255, 0, 0));
     DrawVec2f::line(buffer, m, c, makecol(0, 255, 0));
     DrawVec2f::line(buffer, m, m + n, makecol(0, 255, 0));
     DrawVec2f::line(buffer, m, m - n, makecol(0, 255, 0));
     
     if(n.x * n.y > 0.0)
     {
      DrawVec2f::line(buffer, p1, p1 + d * 200, makecol(0, 255, 0));
      DrawVec2f::line(buffer, p3, p3 + d * 200, makecol(0, 255, 0));
      DrawVec2f::circlefill(buffer, p1, 2, makecol(255, 0, 0));
      DrawVec2f::circlefill(buffer, p3, 2, makecol(255, 0, 0));
      
      //printf("%d\n", p1.dot(n) > p3.dot(n));
     }
     
     else
     {
      DrawVec2f::line(buffer, p2, p2 + d * 200, makecol(0, 255, 0));
      DrawVec2f::line(buffer, p4, p4 + d * 200, makecol(0, 255, 0));
      DrawVec2f::circlefill(buffer, p2, 2, makecol(255, 0, 0));
      DrawVec2f::circlefill(buffer, p4, 2, makecol(255, 0, 0));
      
      //printf("%d\n", p2.dot(n) > p4.dot(n));
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
