#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include "Vec2f.hpp"
#include "Poly.hpp"
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
  
  bool intersect(float& res, Vec2f const& v1, Vec2f const& v2) const
    {
        Vec2f n = dir.normal();
        float d1 = (v1 - orig).dot(n);
        float d2 = (v2 - orig).dot(n);
        
        if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
        {
            float k = fabs(d1) / (fabs(d1) + fabs(d2)), q = dist(v1 + (v2 - v1) * k);
            
            if(q >= 0)
            {
                res = q;
                return true;
            }
        }

        return false;
    }
};

static bool clip(Vec2f& p, Vec2f const& v1, Vec2f const& v2, Ray const& ray)
{
  Vec2f n = ray.dir.normal();
  float d1 = (v1 - ray.orig).dot(n);
  float d2 = (v2 - ray.orig).dot(n);
  
  if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
  {
     float k = fabs(d1) / (fabs(d1) + fabs(d2));
     p = v1 + (v2 - v1) * k;
     return true;
  }
  
  return false;
}

bool rayVsPoly(std::vector<float>& ip, Poly const& poly, Ray const& ray)
{
     int n = poly.vertexVec.size();
     Vec2f p;
     float dist;
 
     ip.clear();
 
     for(int i = 0; i < poly.vertexVec.size(); i++)
     {
        if(ray.intersect(dist, poly.vertexVec[i], poly.vertexVec[(i + 1) % n]))
          ip.push_back(dist);
     }
     
     switch(ip.size())
     {
        case 0:
             return false;
             break;
      
        case 1:
             return true;
             break;
      
        case 2:
             if(ip[0] > ip[1])
             {
                dist = ip[0];
                ip[0] = ip[1];
                ip[1] = dist;
             }
             
             return true;
             break;
     }
}

float Poly::intersect(Vec2f& norm, Ray const& ray) const
    {
        int n = vertexVec.size(), counter = 0;
        float dist, minDist = FLT_MAX;
        
        for(int i = 0; i < vertexVec.size(); i++)
        {
            if(ray.intersect(dist, vertexVec[i], vertexVec[(i + 1) % n]))
            {
                counter++;
                
                if(dist < minDist)
                {
                norm = (vertexVec[i] - vertexVec[(i + 1) % n]).normal();
                minDist = dist;
                }
            }
        }
        
        if(counter == 1)
          return 0;
        
        return minDist;
    }

int main()
{
    bool exit = false;
    
    init();

    Poly poly;
    poly.init(400, 300, 100, 7);
    
    Vec2f a(100, 100), b(200, 200);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1)
       a.init(mouse_x, mouse_y);
     if(mouse_b == 2)
       b.init(mouse_x, mouse_y);
     
     Ray ray;
     ray.init(a, b);
     
     clear_to_color(buffer, 0);
     
     int i;
     for(i = 0; i < poly.vertexVec.size() - 1; i++)
       DrawVec2f::line(buffer, poly.vertexVec[i], poly.vertexVec[i + 1], makecol(255, 255, 255));
     DrawVec2f::line(buffer, poly.vertexVec[i], poly.vertexVec[0], makecol(255, 255, 255));
     
     DrawVec2f::line(buffer, a, b, makecol(255, 255, 255));
     
     DrawVec2f::circlefill(buffer, a, 4, makecol(0, 0, 255));
     
     /*
     std::vector<float> ip;
     if(rayVsPoly(ip, poly, ray))
     {
          Vec2f p = ray.point(ip[0]);
         printf("%f\n", (p - ray.orig).length() - ip[0]);
      
       for(int i = 0; i < ip.size(); i++)
       {
        Vec2f p = ray.point(ip[i]);
         DrawVec2f::circlefill(buffer, p, 2, makecol(255, 0, 0));
       }
     }*/
     
     Vec2f norm;
     float dist;
     
     if((dist = poly.intersect(norm, ray)) < FLT_MAX)
     {
        Vec2f p = ray.point(dist);
        printf("%f\n", (p - ray.orig).length() - dist);
        DrawVec2f::circlefill(buffer, p, 2, makecol(255, 0, 0));
        DrawVec2f::line(buffer, p, p + norm, makecol(255, 0, 0));
     }
     
     
     /*
     for(i = 0; i < poly.vertexVec.size(); i++)
     {
        int n = poly.vertexVec.size();
        Vec2f p;
        
        if(clip(p, poly.vertexVec[i], poly.vertexVec[(i + 1) % n], ray) && ray.dist(p) >= 0)
          DrawVec2f::circlefill(buffer, p, 2, makecol(255, 0, 0));
     }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
