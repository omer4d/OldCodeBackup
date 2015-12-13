#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>

#include "Vec2f.hpp"
#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
          set_add_blender(255, 255, 255, 255);
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct TickData {
   float dt;
   
   TickData(float dt)
   {
      this->dt = dt;
   }
};

typedef std::vector<TickData> Ticks;

Ticks const& past(Ticks const& ticks, int n)
{
  if(n < ticks.size())
    return Ticks(&ticks[0], &ticks[ticks.size() - 1 - n]);
  else
    return Ticks();
}

TickData const& present(Ticks const& ticks)
{
  return ticks.back();
}

struct Particle {
       Vec2f const p0;
       
       Particle(Vec2f p): p0(p)
       {
       }
       
       
       Vec2f pos(Ticks const& ticks)
       {
             if(ticks.empty())
                return p0;
             else
                 return pos(past(ticks, 1)) + Vec2f(100, 0) * present(ticks).dt;
       }
};

int main()
{
    bool exit = false;
    
    init();
    
    Ticks ticks;
    Particle par(Vec2f(100, 100));
    
    while(!exit)
    { 
        if(key[KEY_ESC]) exit = true;
        
        rest(100);
        ticks.push_back(TickData(0.1f));
        
        printf("%d\n", ticks.size());
        if(ticks.size() > 200)
           ticks.clear();
     
        clear_to_color(buffer, makecol(128, 128, 128));
        
        Vec2f p = par.pos(ticks);
        circlefill(buffer, int(p.x), int(p.y), 3, makecol(255, 0, 0));
        
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
