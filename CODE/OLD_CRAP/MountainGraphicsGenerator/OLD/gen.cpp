#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"
#include "Buffer.hpp"
#include "Col3f.hpp"

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

struct Circle {
      Vec2f pos;
      float rad;
      
      Circle(Vec2f pos, float rad)
      {
         this->pos = pos;
         this->rad = rad;
      }
      
      Vec2f point(float rs, float a)
      {
         return pos + Vec2f(rad * rs * cos(a), rad * rs * sin(a));
      }
      
      void draw(BITMAP* buffer, int col)
      {
          //if(rad < 23)
          //DrawVec2f::circlefill(buffer, pos + Vec2f(2, 5), int(rad + 4), makecol(200, 200, 200));
          //if(rad > 15)
          DrawVec2f::circlefill(buffer, pos, int(rad), makecol(255, 255, 255));
      }
};

typedef std::list<Circle> CircleList;
typedef CircleList::iterator CircleIter;

void gen(CircleList& circleList, Vec2f pos, float rad, int n)
{
     if(n < 0)
        return;
 
     circleList.push_back(Circle(pos, rad * randf(1.f, 1.5) ));
     
     
     
     float a = 0.3, da = (M_PI - a * 2.f) / 3.f;
     float r;
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen(circleList, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
}

struct ClipRect {
       int minx, miny, maxx, maxy;
       
       void initDims(int x, int y, int w, int h)
       {
           this->minx = x;
           this->miny = y;
           this->maxx = x + ABS(w);
           this->maxy = y + ABS(h);
       }
       
       void initCorners(int x1, int y1, int x2, int y2)
       {
           this->minx = MIN(x1, x2);
           this->miny = MIN(y1, y2);
           this->maxx = MAX(x1, x2);
           this->maxy = MAX(y1, y2);
       }
       
       ClipRect()
       {
           minx = 0;
           miny = 0;
           maxx = 0;
           maxy = 0;
       }
       
       ClipRect(int x1, int y1, int x2, int y2)
       {
           initCorners(x1, y1, x2, y2);
       }
       
       int getWidth() { return maxx - minx; }
       int getHeight() { return maxy - miny; }
       
       static ClipRect intersect(ClipRect const& a, ClipRect const& b)
       {
           return ClipRect(MAX(a.minx, b.minx), MAX(a.miny, b.miny),
                           MIN(a.maxx, b.maxx), MIN(a.maxy, b.maxy));
       }
};

float pow2(float x)
{
   return x * x;
}

void drawBump(BITMAP* bmp, Vec2f pos, float rad)
{
   ClipRect clip = ClipRect::intersect(ClipRect(0, 0, bmp->w, bmp->h),
                                       ClipRect(int(pos.x - rad), int(pos.y - rad),
                                                int(pos.x + rad), int(pos.y + rad)));
   
   for(int y = clip.miny; y < clip.maxy; y++)
      for(int x = clip.minx; x < clip.maxx; x++)
      {
          Vec2f v = pos - Vec2f(x, y);
          int z = clamp255(sqrt(MAX(0.f, pow2(rad) - v.dot(v))));
          //Col3f src = PIXEL(b
          
          PIXEL(bmp, x, y) = makecol(z, z, z);
      }
}

void gen2(BITMAP* bmp, Vec2f pos, float rad, int n)
{
      if(n < 0)
        return;
     
     DrawVec2f::circlefill(bmp, pos + Vec2f(bmp->w / 2, bmp->h / 2),
                           int(rad * randf(1.f, 1.5)), makecol(255, 255, 255));
     
     float a = 0.3, da = (M_PI - a * 2.f) / 3.f;
     float r;
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen2(bmp, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen2(bmp, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.2f);
     gen2(bmp, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.6, n - 1);
     a += da * randf(0.8, 1.2);
     
     r = rad * randf(0.9f, 1.f + sqrt(n));
     gen2(bmp, pos + Vec2f(r * cos(a), r * sin(a)), rad * 0.7, n - 1);
     a += da * randf(0.8, 1.2);
 
}

int main()
{   
    bool exit = false;
    
    init();
    
    std::list<Circle> circleList, circleList2;
    BITMAP* bmp = create_bitmap(256, 256);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     if(key[KEY_SPACE])
     {
     
         clear_to_color(bmp, 0);
         gen2(bmp, Vec2f(), 10.f, 3);
     }


     clear_to_color(buffer, 0);
     
     //for(CircleIter i = circleList2.begin(); i != circleList2.end(); ++i)
       //i->draw(buffer, makecol(128, 128, 128));
     
     //for(CircleIter i = circleList.begin(); i != circleList.end(); ++i)
       //i->draw(buffer, makecol(255, 255, 255));
     
     
     draw_sprite(buffer, bmp, 0, 0);
     
     drawBump(buffer, Vec2f(300, 300), 10);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(bmp);
    
    deinit();
    return 0;
}END_OF_MAIN()
