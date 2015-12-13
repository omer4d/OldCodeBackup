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

struct Interval {
   float min, max;
   bool solid;
   
   Interval(float min, float max, bool solid)
   {
       this->min = min;
       this->max = max;
       this->solid = solid;
   }
};

typedef std::vector<Interval> Strip;

Strip vstrip(BITMAP* map, float x1, float y1, float y2)
{
    Strip strip;
    int x = int(x1);
    int miny = int(MIN(y1, y2));
    int maxy = int(MAX(y1, y2));
    
    bool lastSol = getpixel(map, x, miny);
    int lastY = miny, y;
    
    for(y = miny + 1; y <= maxy; y++)
    {
       bool sol = getpixel(map, x, y);
     
       if(sol != lastSol)
       {
         strip.push_back(Interval(lastY, y, lastSol));
         lastSol = sol;
         lastY = y;
      }
    }
    
    strip.push_back(Interval(lastY, maxy, lastSol));
    
    strip[0].min = MIN(y1, y2);
    strip[strip.size() - 1].max = MAX(y1, y2);
    
    return strip;
}

/*
void drawStrip(BITMAP* buffer, Strip const& strip, int x)
{
  for(int i = 0; i < strip.size(); i++)
     if(strip[i].solid)
        vline(buffer, x, strip[i].min, strip[i].max, makecol(255, 0, 0));
     else
        vline(buffer, x, strip[i].min, strip[i].max, makecol(0, 0, 255));
}*/

float vsearch(BITMAP* bitmap, bool solidity, Vec2f const& pos, int dir, int limit)
{
   int px = int(pos.x), py = int(pos.y);
   
   if(px >= 0 && px < bitmap->w && py >= 0 && py < bitmap->h && bool(PIXEL(bitmap, px, py)) == solidity)
   {
       int y;
       for(y = py + dir; limit && y >= 0 && y < bitmap->h && bool(PIXEL(bitmap, px, y)) == solidity; y += dir)
       {
          limit--;
       }
       
       return y - pos.y;
   }
   
   return 0.f;
}

struct Particle {
       float invMass;
       Vec2f force, vel, pos;
       
       static Vec2f gravity;
       static float dt;
       
       Particle(Vec2f const &pos, float mass)
       {
           this->pos = pos;
           this->invMass = 1.f / mass;
       }
       
       void logic()
       {
           vel += (Particle::gravity + force * invMass) * dt;
           pos += vel * dt;
           force.init(0.f, 0.f);
       }
};

Vec2f Particle::gravity(0.f, 3000.f);
float Particle::dt = 0.001f;

struct Ray {
  Vec2f orig, dir;
  
  void init(Vec2f const& src, Vec2f const& dest)
  {
     orig = src;
     dir = (dest - src).unit();
  }
  
  float dist(Vec2f const& p) const
  {
     if(fabs(dir.x) > 0.01)
        return (p.x - orig.x) / dir.x;
     else
        return (p.y - orig.y) / dir.y;
  }
  
  Vec2f point(float k) const
  {
     return orig + dir * k;
  }
};

int sgni(float f)
{
   return f < 0 ? -1 : 1;
}

bool rayVsMap(Vec2f& ip, BITMAP* buffer, Ray const& ray, float maxDist)
{
     int x = int(ray.orig.x), y = int(ray.orig.y);
     int stepX = sgni(ray.dir.x), stepY = sgni(ray.dir.y);    
     
     float tMaxX = (int(ray.orig.x + (stepX > 0)) - ray.orig.x) / ray.dir.x;
     float tMaxY = (int(ray.orig.y + (stepY > 0)) - ray.orig.y) / ray.dir.y;
     
     float tDeltaX = float(stepX) / ray.dir.x;
     float tDeltaY = float(stepY) / ray.dir.y;
     float t = 0.0;
     
     while(x >= 0 && y >= 0 && x < buffer->w && y < buffer->h && t <= maxDist)
     {
        if(PIXEL(buffer, x, y))
        {
           ip = ray.point(t);
           return true;
        }
        
        if(tMaxX < tMaxY)
        {
           t = tMaxX;
           tMaxX += tDeltaX;
           x += stepX;
        }
        
        else
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
        }
     }
     
     return false;
}

bool lineVsMap(Vec2f& ip, BITMAP* buffer, Vec2f const& a, Vec2f const& b)
{
    Ray ray;
    Vec2f delta = b - a;
    float len = delta.length();
    
    ray.orig = a;
    ray.dir = delta * (1.f / len);
    
    return rayVsMap(ip, buffer, ray, len);
}

bool binLineVsMap(Vec2f& ip, BITMAP* map, Vec2f min, Vec2f max)
{ 
    if(!PIXEL(map, int(max.x), int(max.y)))
       return false;
       
       for(int i = 0; i < 100; i++)
       {
           Vec2f mid = (min + max) * 0.5f;
           if( PIXEL(map, int(mid.x), int(mid.y)) )
           {
               max = mid;
           }
           else
           {
               min = mid;
           }
       }
       
    ip = (min + max) * 0.5;
    return true;
}

struct Player {
       Particle phys;
       Vec2f oldPos;
       float width, height;
       bool grounded, walking;
       float dir;
       
       Player(): phys(Vec2f(SCREEN_W / 2, 200), 1.f)
       {
          width = 10;
          height = 15;
          grounded = false;
          walking = false;
          dir = 1.f;
       }
       
       void logic()
       {
           float accel = 30, velLimit = 100;
           walking = false;
           
           if(grounded)
           {
              phys.vel.x *= 0.9;
            
              if(key[KEY_RIGHT] && phys.vel.x < velLimit)
              {
                  phys.vel.x += accel;
                  walking = true;
                  dir = 1.f;
              }
                  
              if(key[KEY_LEFT] && phys.vel.x > -velLimit)
              {
                  phys.vel.x -= accel;
                  walking = true;
                  dir = -1.f;
              }
                  
              if(key[KEY_ENTER])
              {
                 phys.vel.x = 300 * dir;
                 phys.vel.y = -300;
              }
           }
        
           oldPos = phys.pos;
           phys.logic();
       }
       
       void draw(BITMAP* buffer)
       {
           Vec2f a = phys.pos + Vec2f(-width * 0.5, 0),
                 b = phys.pos + Vec2f(width * 0.5, 0),
                 c = phys.pos + Vec2f(0, -height);
           
           //DrawVec2f::line(buffer, a, b, makecol(0, 255, 0));
           //DrawVec2f::line(buffer, phys.pos, c, makecol(0, 255, 0));
           
           ellipsefill(buffer, int(phys.pos.x), int(phys.pos.y - height * 0.5),
                               int(width * 0.5), int(height * 0.7), makecol(0, 255, 0));
       }
       
       void collide(BITMAP* map)
       {
           grounded = false;
           Vec2f& pos = phys.pos;
           Strip strip = vstrip(map, pos.x, pos.y, pos.y - height);
       }
};

int main()
{   
    bool exit = false;
    
    init();
    
    Player player;
    
    BITMAP* map = load_bitmap("map.bmp", NULL);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     if(mouse_b == 1)
     {
       player.phys.pos.init(mouse_x, mouse_y);
       player.phys.vel.init(0.f, 0.f);
     }

     player.logic();
     player.collide(map);

     clear_to_color(buffer, makecol(128, 128, 128));
     draw_sprite(buffer, map, 0, 0);
     
     
     /*
     Vec2f ip, a(SCREEN_W / 2, 200), b(mouse_x, mouse_y);
     if(binLineVsMap(ip, map, a, b))
        DrawVec2f::line(buffer, a, ip, makecol(255, 0, 0));
     else
        DrawVec2f::line(buffer, a, b, makecol(0, 255, 0));*/
     
     player.draw(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     //Strip strip = vstrip(map, mouse_x, mouse_y - 20, mouse_y + 20);
     //drawStrip(buffer, strip, mouse_x);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(map);
    deinit();
    return 0;
}END_OF_MAIN()
