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
};

#define BOID_NUM 50

struct Flock {
  std::vector<Boid> boidVec;
  
  Flock()
  {
         boidVec.reserve(BOID_NUM);
         float cx = SCREEN_W / 2, cy = SCREEN_H / 2, r = 400;
         
         for(int i = 0; i < BOID_NUM; i++)
         {
           Vec2f p(cx + randf() * r, cy + randf() * r);
           boidVec.push_back(Boid(p));
         }
  }
  
  Vec2f moveTowardsCenter(int self)
  {
       Vec2f center;
       
       for(int i = 0; i < boidVec.size(); i++)
          if(i != self)
            center += boidVec[i].pos;
       
       center *= 1.f / (BOID_NUM - 1);
       return (center - boidVec[self].pos) * 0.001;
  }
  
  Vec2f avoidCollisions(int self)
  {
     Vec2f disp;
     float rest = 40.0;

		   for(int i = 0; i < boidVec.size(); i++)
		     if(i != self)
		     {
          Vec2f delta = boidVec[i].pos - boidVec[self].pos;
          float len = delta.length();
        
          if(len < 0.1)
          {
              printf("!");
              float ang = randf(0, M_PI * 2.0);
              disp.x -= rest * 10 * cos(ang);
              disp.y -= rest * 10 * sin(ang);
          }
        
		        else if(len < rest)
		        {
            disp -= delta;
          }
       }
            
     return disp;
  }
  
  Vec2f matchVel(int self)
  {
       Vec2f avgVel;
       
       for(int i = 0; i < boidVec.size(); i++)
          if(i != self)
            avgVel += boidVec[i].vel;
       
       avgVel *= 1.f / (BOID_NUM - 1);
       return (avgVel - boidVec[self].vel) * 0.00125;
  }
  
  Vec2f moveTowards(int self, Vec2f target)
  {
        return (target - boidVec[self].pos) * 0.02;
  }
  
  void simulate(float dt, std::vector<Boid> const& danger)
  {
       for(int i = 0; i < boidVec.size(); i++)
       {
          boidVec[i].vel += (moveTowardsCenter(i) + avoidCollisions(i)  + matchVel(i)
                             + moveTowards(i, Vec2f(mouse_x, mouse_y))) * dt;
          
          //boidVec[i].vel += Vec2f(20 * randf(), 20 * randf()) * dt;
          
          //for(int j = 0; j < danger.size(); j++)
           //if((boidVec[i].pos - danger[j].pos).length() < 100 )
             //boidVec[i].vel -= moveTowards(i, danger[j].pos) * 5.0 * dt;
          
          boidVec[i].vel *= 0.95;
          boidVec[i].pos += boidVec[i].vel * dt;
       }
  }
  
  void render(BITMAP* buffer)
  {
       for(int i = 0; i < boidVec.size(); i++)
       {
          DrawVec2f::circlefill(buffer, boidVec[i].pos, 3, 0);
       }
  }
};

class A {
      class B {
            friend class A;
            
            A* a;
            
            void func()
            {
               a->func();
            }
      };
      
      B* b;
      
      void func()
      {
         b->func();
      }
};

int main()
{
    bool exit = false;
    
    init();

    
    Flock flock;
    std::vector<Boid> danger;
    
    float cx = SCREEN_W / 2, cy = SCREEN_H / 2, r = 150;
         
    for(int i = 0; i < 5; i++)
    {
       Boid boid;
       boid.pos.init(cx + randf() * r, cy + randf() * r);
       boid.vel.init(4 + randf() * 30, 4 + randf() * 30 );
       
       danger.push_back(boid);
    }
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     flock.simulate(0.02, danger);
      
     clear_to_color(buffer, makecol(64, 64, 64));
     flock.render(buffer);
     
     /*
     for(int i = 0; i < danger.size(); i++)
       {
          danger[i].pos += danger[i].vel * 0.02;
          if(danger[i].pos.x < 0 || danger[i].pos.y < 0 || danger[i].pos.x > SCREEN_W || danger[i].pos.y > SCREEN_H)
          {
          danger[i].pos.init(rand() % SCREEN_W, rand() % SCREEN_H);
          danger[i].vel.init(10 + randf(-30, 30), 10 + randf(-30, 30));
          }
          
          DrawVec2f::circlefill(buffer, danger[i].pos, 3, makecol(255, 0, 0));
       }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
