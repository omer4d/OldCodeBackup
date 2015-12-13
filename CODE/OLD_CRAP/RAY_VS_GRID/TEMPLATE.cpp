#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include <vector>

#include "DrawVec2f.hpp"

#include "Vec2f.hpp"
#include "Util.hpp"

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

int sgni(float f)
{
   return f < 0 ? -1 : 1;
}

#define W_GRID_SIZE 10
#define H_GRID_SIZE 3

Vec2f drawRay(BITMAP* buffer, Ray const& ray)
{
     int x = int(ray.orig.x), y = int(ray.orig.y);
     int stepX = sgni(ray.dir.x), stepY = sgni(ray.dir.y);    
     
     // Distance along the ray to cross the nearest X boundry:
     float tMaxX = (int(ray.orig.x + (stepX > 0)) - ray.orig.x) / ray.dir.x;
     // Distance along the ray to cross the nearest Y boundry:
     float tMaxY = (int(ray.orig.y + (stepY > 0)) - ray.orig.y) / ray.dir.y;
     // Distance along the ray to cr
     float tDeltaX = float(stepX) / ray.dir.x;
     float tDeltaY = float(stepY) / ray.dir.y;
     float t = 0.0;
     
     while(x >= 0 && y >= 0 && x < buffer->w && y < buffer->h)
     {
      
      if(getpixel(buffer, x, y) == makecol(0, 128, 0))
        {
          break;
        }
        
        putpixel(buffer, x, y, makecol(255, 0, 0));
      
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
     
     printf("%f - ", t * W_GRID_SIZE);
     return ray.point(t);
}



int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* grid = create_bitmap(SCREEN_W / W_GRID_SIZE, SCREEN_H / H_GRID_SIZE);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     Ray ray;
     ray.init(Vec2f(320, 240), Vec2f(mouse_x, mouse_y));
     Ray gridRay = ray;
     
     gridRay.dir.y *= float(W_GRID_SIZE) / float(H_GRID_SIZE);
     
     gridRay.orig.x /= W_GRID_SIZE;
     gridRay.orig.y /= H_GRID_SIZE;
     
     clear_bitmap(grid);
     
     circle(grid, 320/W_GRID_SIZE, 240/H_GRID_SIZE, 20, makecol(0, 128, 0));
     Vec2f p = drawRay(grid, gridRay);
     p.x *= W_GRID_SIZE;
     p.y *= H_GRID_SIZE;
     printf("%f\n", (p - ray.orig).length());
     
     
     clear_to_color(buffer, makecol(64, 64, 64));
     stretch_blit(grid, buffer, 0, 0, grid->w, grid->h, 0, 0, SCREEN_W, SCREEN_H);
     
     circlefill(buffer, int(p.x), int(p.y), 1, makecol(255, 255, 0));
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     line(buffer, int(ray.orig.x), int(ray.orig.y), mouse_x, mouse_y, makecol(0, 0, 255));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    
    destroy_bitmap(grid);
    deinit();
    return 0;
}END_OF_MAIN()
