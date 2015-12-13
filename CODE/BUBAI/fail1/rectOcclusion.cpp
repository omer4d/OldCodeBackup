#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>
#include <array>
#include <random>

#include "Buffer.hpp"
#include "GridMatrix.hpp"
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
     
     //install_int(tick, 1);
     
     srand(time(NULL));
     //srand(100);
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct Line {
   Vec2f a, b;
   
   Line(float x1, float y1, float x2, float y2): a(x1, y1), b(x2, y2)
   {
   }
   
   Line()
   {
   }
};

Line SILHOUETTES[16];
float CELL_W = 100.f;
float CELL_H = 50.f;
Vec2f CELL_MIN(-CELL_W / 2, -CELL_H / 2);
Vec2f CELL_MAX(CELL_W / 2, CELL_H / 2);

bool lineVsBox(Vec2f const& a, Vec2f const& b, Vec2f const& boxCenter)
{
   Vec2f n = (b - a).normal();
   Vec2f v1, v2;
   
   if(n.x * n.y > 0)
   {
       v1.init(boxCenter.x + CELL_MIN.x, boxCenter.y + CELL_MIN.y);
       v2.init(boxCenter.x + CELL_MAX.x, boxCenter.y + CELL_MAX.y);
   }
   
   else
   {
       v1.init(boxCenter.x + CELL_MAX.x, boxCenter.y + CELL_MIN.y);
       v2.init(boxCenter.x + CELL_MIN.x, boxCenter.y + CELL_MAX.y);
   }
   
   float pa = (v1 - a).dot(n);
   float pb = (v2 - a).dot(n);
   return pa < pb ? pa < 0 && pb > 0 : pb < 0 && pa > 0;
}

void initSilhouettes()
{ 
   SILHOUETTES[1] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MIN.x, CELL_MAX.y);
   SILHOUETTES[2] = Line(CELL_MAX.x, CELL_MAX.y, CELL_MAX.x, CELL_MIN.y);
   SILHOUETTES[4] = Line(CELL_MAX.x, CELL_MIN.y, CELL_MIN.x, CELL_MIN.y);
   SILHOUETTES[5] = Line(CELL_MAX.x, CELL_MIN.y, CELL_MIN.x, CELL_MAX.y);
   SILHOUETTES[6] = Line(CELL_MAX.x, CELL_MAX.y, CELL_MIN.x, CELL_MIN.y);
   SILHOUETTES[8] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[9] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[10] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MIN.y);
}

// Returned points are in clockwise order relative to p!
Line silhouette(Vec2f p, Vec2f const& boxCenter)
{
   p -= boxCenter;
 
   int i = (p.x < CELL_MIN.x) +
           ((p.x > CELL_MAX.x) << 1) +
           ((p.y < CELL_MIN.y) << 2) +
           ((p.y > CELL_MAX.y) << 3);
   
   Line result = SILHOUETTES[i];
   result.a += boxCenter;
   result.b += boxCenter;
   
   return result;
}

bool between(Vec2f const& v, Vec2f const& a, Vec2f const& b)
{
    return a.normal().dot(v) >= 0.f && b.normal().dot(v) <= 0.f;
}

bool occlude(Vec2f& dest1, Vec2f& dest2, Vec2f const& src, Vec2f const& occCenter)
{
    Line sil = silhouette(src, occCenter);
    Vec2f a = dest1 - src, b = dest2 - src, c = sil.a - src, d = sil.b - src;
    
    if(between(a, c, d))
    {
        if(between(b, c, d))
           return true;
        else
        {
           dest1 = sil.b;
           return false;
        }
    }
    
    else
    {
        if(between(b, c, d))
            dest2 = sil.a;
        return false;
    }
}

int main()
{
    bool exit = false;
    
    init();
    
    initSilhouettes();
    
    Vec2f occ(200, 200);
    
    while(!exit)
    { 
      if(key[KEY_ESC]) exit = true;
      
      Vec2f src(mouse_x, mouse_y);
      Vec2f c(300, 300);
      
      Line verts = silhouette(src, c);
     
      clear_to_color(buffer, makecol(64, 64, 64));
     
      DrawVec2f::rect(buffer, c + CELL_MIN, c + CELL_MAX, makecol(0, 255, 0));
      DrawVec2f::rect(buffer, occ + CELL_MIN, occ + CELL_MAX, makecol(255, 0, 0));
      
      
      if(!occlude(verts.a, verts.b, src, occ))
      {
         DrawVec2f::line(buffer, src, src + (verts.a - src).unit() * 1000.f, makecol(255, 255, 255));
         DrawVec2f::line(buffer, src, src + (verts.b - src).unit() * 1000.f, makecol(255, 255, 255));
      
         /*
         Vec2f n1 = (verts.a - src).normal().unit() * 10;
         Vec2f n2 = (verts.b - src).normal().unit() * 10;
         DrawVec2f::line(buffer, (src + verts.a) * 0.5f, (src + verts.a) * 0.5f + n1, makecol(255, 255, 255));
         DrawVec2f::line(buffer, (src + verts.b) * 0.5f, (src + verts.b) * 0.5f - n2, makecol(255, 255, 255));*/
      }
     
      draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
      blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
