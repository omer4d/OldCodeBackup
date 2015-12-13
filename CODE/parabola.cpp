#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>

#include "Buffer.hpp"

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
}

void deinit()
{
     destroy_bitmap(buffer);
}

class ParabolaMover {
public:
      float p, dp;
      float a, b, c;
      float x0, y0;
      float x2, y2;
      float y1;
      
      ParabolaMover(float x0, float y0,
                    float x2, float y2,
                    float y1, float g)
      {
           this->x0 = x0;
           this->y0 = y0;
           
           this->x2 = x2;
           this->y2 = y2;
           
           this->y1 = y1;
           
          float sgn = y1 > y0 ? 1.f : -1.f;
          b = sgn * 2.f * sqrt((y1 - y0) * y1 - (y1 - y0) * y2) + 2.f * (y1 - y0);
          a = y2 - y0 - b;
          c = y0;
           
           p = 0.f;
           dp = 1.f / (sqrt(sgn * 2.f * (y1 - y0) / g) + sqrt(sgn * 2.f * (y1 - y2) / g));
           
           printf("%f", dp);
      }
      
      void logic(float dt)
      {
           p += dp * dt;
      }
      
      float x()
      {
          return x0 + (x2 - x0) * p;
      }
      
      float y()
      {
          return a*p*p + b*p + c;
      }
      
      float x(float p)
      {
          return x0 + (x2 - x0) * p;
      }
      
      float y(float p)
      {
            return a*p*p + b*p + c;
      }
      
      float velX()
      {
            return (x2 - x0) * dp;
      }
      
      float velY()
      {
            return b * dp;
      }
};

struct Particle {
    float px, py, vx, vy, g;
    
    Particle(float px, float py, float vx, float vy, float g)
    {
       this->px = px;
       this->py = py;
       this->vx = vx;
       this->vy = vy;
       this->g = g;
    }
    
    void logic(float dt)
    {
       vy += g * dt;
       px += vx * dt;
       py += vy * dt;
    }
};

int main()
{
    bool exit = false;
    
    init();
    
    float g = 30;
    
    ParabolaMover mover(100, 100,
                         200, 100,
                         50, g);
                         
    Particle particle(100, 100, mover.velX(), mover.velY(), g);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));

     if(key[KEY_SPACE])
        mover.p = 0.f;
                         
     mover.logic(0.001f);
     particle.logic(0.001f);
     
     for(float t = 0; t < 1; t += 0.001)
     {
        putpixel(buffer, int(mover.x(t)), int(mover.y(t)), makecol(255, 0, 0));
        
        circlefill(buffer, int(mover.x0), int(mover.y0), 1, makecol(0, 255, 0));
        circlefill(buffer, int(mover.x2), int(mover.y2), 1, makecol(0, 255, 0));
        
        line(buffer, 0, int(mover.y1), SCREEN_W, int(mover.y1), makecol(255, 255, 255));
     }
     
     circlefill(buffer, int(mover.x()), int(mover.y()), 2, makecol(0, 0, 255));
     circlefill(buffer, int(particle.px), int(particle.py), 1, makecol(255, 255, 0));

     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
