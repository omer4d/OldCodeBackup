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


struct Boid {
  Vec2f vel, pos;
  
  Boid()
  {
  }
  
  Boid(Vec2f const& pos)
  {
     this->pos = pos;
  }
  
  void simulate(float dt)
  {
     pos += vel * dt;
  }
};


int main()
{
    bool exit = false;
    
    init();
    
    Boid boid;
    boid.pos.init(SCREEN_W / 2, SCREEN_H / 2);

    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     Vec2f mouse(mouse_x, mouse_y);
     
     float g = 9.81; // gravity
     
     if(mouse_b == 1)
     {
         boid.pos.init(SCREEN_W / 2, SCREEN_H / 2);
      
        Vec2f t = mouse - boid.pos;
        
        //float o = -M_PI/4; // launch angle
       //float s = (sqrt(g) * sqrt(t.x) * sqrt((tan(o)*tan(o))+1)) / sqrt(2 * tan(o) - (2 * g * t.y) / t.x);
      
      float v = 40;
      float s = (v * v * v * v) - g * (g * (t.x * t.x) + 2 * t.y * (v * v));
      float o = atan(((v * v) + sqrt(s)) / (g * t.x)); // launch angle 
      
        printf("%f\n", s);
        boid.vel.init(v * cos(o), v * sin(o));
        
     }
     
     boid.vel.y += g * 0.01;
     
     boid.simulate(0.01);
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     float o = -M_PI / 4;
          DrawVec2f::circlefill(buffer, boid.pos, 3, makecol(255, 255, 0));
          DrawVec2f::line(buffer, boid.pos, boid.pos + Vec2f(100 * cos(o), 100 * sin(o)), makecol(255, 0, 0));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
