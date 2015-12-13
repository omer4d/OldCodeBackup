#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <float.h>

#include "Ray.hpp"
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


/*
bool rayVsAABB(Ray const& ray, Vec2f const& boxMin, Vec2f const& boxMax)
{
 Vec2f const& d = ray.dir;
 Vec2f n = ray.dir.normal();
 bool f = ((boxMin.x - ray.orig.x) * d.x >= 0.f || (boxMax.x - ray.orig.x) * d.x >= 0.f) &&
          ((boxMin.y - ray.orig.y) * d.y >= 0.f || (boxMax.y - ray.orig.y) * d.y >= 0.f);
          
 if(!f)
   return false;
 
 if(n.x * n.y > 0)
 {
  float i1 = boxMin.dot(n), i2 = boxMax.dot(n), p = ray.orig.dot(n);
  return contained(i1, i2, p);
 }
 
 else
 {
  float i1 = Vec2f(boxMax.x, boxMin.y).dot(n), i2 = Vec2f(boxMin.x, boxMax.y).dot(n), p = ray.orig.dot(n);
  return contained(i1, i2, p);
 }
}*/

struct RayHitInfo {
   float dist;
   Vec2f norm;
};

struct Dist {
   Vec2f norm;
   float dist;
   
   Dist(float d, Vec2f const& n)
   {
       this->dist = d;
       this->norm = n;
   }
   
   static Dist& min(Dist& a, Dist& b)
   {
      return a.dist < b.dist ? a : b;
   }
   
   static Dist& max(Dist& a, Dist& b)
   {
      return a.dist > b.dist ? a : b;
   }
};

struct Interval {
   Dist &min, &max;
   
   Interval(Dist& a, Dist& b): min(Dist::min(a, b)), max(Dist::max(a, b))
   {
   }
   
   static bool overlap(Interval const& i1, Interval const& i2)
   {
     return !(i1.min.dist > i2.max.dist || i2.min.dist > i1.max.dist);
   }
};

bool rayVsAABB(RayHitInfo& info, Ray const& ray, Vec2f const& min, Vec2f const& max)
{
   float invDirX = 1.f / ray.dir.x;
   float invDirY = 1.f / ray.dir.y;
   
   Dist dminx = Dist((min.x - ray.orig.x) * invDirX, Vec2f(-1.f, 0.f));
   Dist dmaxx = Dist((max.x - ray.orig.x) * invDirX, Vec2f(1.f, 0.f));
   Dist dminy = Dist((min.y - ray.orig.y) * invDirY, Vec2f(0.f, -1.f));
   Dist dmaxy = Dist((max.y - ray.orig.y) * invDirY, Vec2f(0.f, 1.f));
   
   Interval i1(dminx, dmaxx);
   Interval i2(dminy, dmaxy);
   
   Dist& d1 = Dist::max(i1.min, i2.min);
   Dist& d2 = Dist::min(i1.max, i2.max);
   
   info.dist = d1.dist >= 0.f ? d1.dist : d2.dist;
   info.norm = d1.dist >= 0.f ? d1.norm : d2.norm * -1.f;
   
   return Interval::overlap(i1, i2) && i1.max.dist >= 0.f && i2.max.dist >= 0.f;
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


struct Node {
  Vec2f min, max;
  bool occupied;
  Node* children[4];
  
  
};

void findBitmapBounds(BITMAP* bitmap, Vec2f& min, Vec2f& max)
{
  min.init(FLT_MAX, FLT_MAX);
  max.init(-FLT_MAX, -FLT_MAX);
  
  for(int y = 0; y < bitmap->h; y++)
    for(int x = 0; x < bitmap->w; x++)
      if(PIXEL(bitmap, x, y))
      {
        if(x < min.x) min.x = x;
        else if(x > max.x) max.x = x;
        
        if(y < min.y) min.y = y;
        else if(y > max.y) max.y = y;
      }
}

bool isEmpty(BITMAP* bitmap, Vec2f& min, Vec2f& max)
{
   for(int y = MAX(int(min.y + 1), 0); y < MIN(int(max.y), bitmap->h); y++)
    for(int x = MAX(int(min.x + 1), 0); y < MIN(int(max.x), bitmap->w); x++)
      if(PIXEL(bitmap, x, y))
        return true;
}

int main()
{
    bool exit = false;
    
    init();

    Vec2f l1(100, 100), l2(200, 200);

    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1)
      l1.init(mouse_x, mouse_y);
     if(mouse_b == 2)
      l2.init(mouse_x, mouse_y);
      
     
     Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
        
     Vec2f p1(-200, -100),
           p2(200, -100),
           p3(200, 100),
           p4(-200, 100);
       
     p1 += c;
     p2 += c;
     p3 += c;
     p4 += c;
     
     
     Ray ray;
     ray.initFromSegment(l1, l2);
     
     if(key[KEY_UP])
     {
        ray.dir.x = -0.f;
        ray.dir.y = -1.f;
     }
     
     if(key[KEY_DOWN])
     {
        ray.dir.x = -0.f;
        ray.dir.y = 1.f;
     }
     
     if(key[KEY_LEFT])
     {
        ray.dir.x = -1.f;
        ray.dir.y = -0.f;
     }
     
     if(key[KEY_RIGHT])
     {
        ray.dir.x = 1.f;
        ray.dir.y = -0.f;
     }
     
     
     clear_to_color(buffer, 0);
     
     RayHitInfo info;
     
     if(rayVsAABB(info, ray, p1, p3))
     {
        DrawVec2f::rect(buffer, p1, p3, makecol(255, 0, 0));
        DrawVec2f::circlefill(buffer, ray.point(info.dist), 3, makecol(255, 0, 0));
        DrawVec2f::line(buffer, ray.point(info.dist), ray.point(info.dist) + info.norm * 100.f, makecol(255, 0, 0));
     }
        
     else
     DrawVec2f::rect(buffer, p1, p3, makecol(0, 0, 255));
     
     DrawVec2f::circlefill(buffer, l1, 3, makecol(255, 255, 0));
     DrawVec2f::line(buffer, l1, l1 + ray.dir * 1000, makecol(0, 255, 0));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
