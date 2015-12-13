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

bool clipPoly(Poly& dest1, Poly& dest2, Poly const& src, Ray const& ray)
{
     Poly* curr = &dest1;
     Poly* other = &dest2;
     
     for(int i = 0; i < src.vertexVec.size(); i++)
     {
        int n = src.vertexVec.size();
        Vec2f p;
        
        curr->vertexVec.push_back(src.vertexVec[i]);
        
        if(clip(p, src.vertexVec[i], src.vertexVec[(i + 1) % n], ray))
        {
           curr->vertexVec.push_back(p);
           other->vertexVec.push_back(p);
           Poly* temp = curr;
           curr = other;
           other = temp;
        }
     }
     
     return dest2.vertexVec.size() != 0;
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
     
     Poly poly1, poly2;
     
     if(clipPoly(poly1, poly2, poly, ray))
     {
       poly1.draw(buffer, makecol(255, 0, 0));
       poly2.draw(buffer, makecol(0, 255, 0));
     }
     
     else
     {
       poly1.draw(buffer, makecol(255, 0, 0));
     }
       
     
     
     DrawVec2f::line(buffer, a, b, makecol(255, 255, 255));
     DrawVec2f::circlefill(buffer, a, 4, makecol(0, 0, 255));
     
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
