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

void initSilhouettes()
{ 
   SILHOUETTES[1] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MIN.x, CELL_MAX.y);
   SILHOUETTES[2] = Line(CELL_MAX.x, CELL_MIN.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[4] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MAX.x, CELL_MIN.y);
   SILHOUETTES[5] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MIN.y);
   SILHOUETTES[6] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[8] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[9] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[10] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MIN.y);
}

Line silhouette(Vec2f const& c, Vec2f p)
{
   p -= c;
 
   int i = (p.x < CELL_MIN.x) +
           ((p.x > CELL_MAX.x) << 1) +
           ((p.y < CELL_MIN.y) << 2) +
           ((p.y > CELL_MAX.y) << 3);
   
   Line result = SILHOUETTES[i];
   result.a += c;
   result.b += c;
   
   return result;
}

Line interval(Vec2f const& c, Vec2f p)
{
     Vec2f d = p - c;
     
     if(d.x * d.y < 0.0)
        return Line(c.x + CELL_MIN.x, c.y + CELL_MIN.y, c.x + CELL_MAX.x, c.y + CELL_MAX.y);
     else
        return Line(c.x + CELL_MAX.x, c.y + CELL_MIN.y, c.x + CELL_MIN.x, c.y + CELL_MAX.y);
}

int main()
{
    bool exit = false;
    
    init();
    
    initSilhouettes();

    while(!exit)
    { 
      if(key[KEY_ESC]) exit = true;
     
      Vec2f src(mouse_x, mouse_y);
      Vec2f c(300, 300);
      
      Line verts = silhouette(c, src);
      Line verts2 = interval(c, src);
     
      clear_to_color(buffer, makecol(64, 64, 64));
     
      DrawVec2f::rect(buffer, c + CELL_MIN, c + CELL_MAX, makecol(255, 0, 0));
      DrawVec2f::line(buffer, src, verts.a, makecol(255, 255, 255));
      DrawVec2f::line(buffer, src, verts.b, makecol(255, 255, 255));
     
      draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
      blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
