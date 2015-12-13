#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include "Vec2f.hpp"

struct Road {
 Vec2f a, b;
};

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

float randf()
{
      return float(rand()) / float(RAND_MAX);
}

float randf(float min, float max)
{
      return min + (max - min) * randf();
}

int round(float f)
{
    if(f < 0.0) return int(f - 0.5f);
    else return int(f + 0.5f);
}

int randi(int min, int max)
{
    return round(randf(min, max));
}

void blah(Road const& base, std::list<Road>& roadList, int branching, int depth)
{
     if(!depth) return;
     
     for(int i = 0; i < branching; i++)
     {       
      Road branch;
      float dir;
      
      if(rand() % 2) dir = 1.f;
      else dir = -1.f;
      
      branch.a = base.a + (base.b - base.a) * (float(i) / float(branching - 1)); //randf();
      branch.b = branch.a + (base.b - base.a).normal() * randf(0.7f, 1.f) * dir;

      roadList.push_back(branch);
      
      blah(branch, roadList, branching, depth - 1);
     }

}

int main()
{   
    bool exit = false;
    
    init();
    
    std::list<Road> roadList;
    
    while(!exit)
    {
     Road base;
     base.a.init(SCREEN_W / 2, SCREEN_H / 2);
     base.b.init(mouse_x, mouse_y);
                
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_SPACE])
      {
       roadList.clear();
       roadList.push_back(base);
       
       blah(base, roadList, 3, 3);
      }

     clear_to_color(buffer, 0);
     
     line(buffer, int(base.a.x), int(base.a.y), int(base.b.x), int(base.b.y), makecol(255, 0, 0));
     
     for(std::list<Road>::iterator i = roadList.begin(); i != roadList.end(); ++i)
     {
      line(buffer, int(i->a.x), int(i->a.y), int(i->b.x), int(i->b.y), makecol(255, 255, 255));
      }
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
