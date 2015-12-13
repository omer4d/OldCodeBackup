#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include <string>
#include <map>

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#include "Particle.hpp"
#include "SpringCon.hpp"
#include "StickCon.hpp"
#include "StaticStickCon.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct Branch {
       std::vector<Particle*> particleVec;
       std::vector<Constraint*> constraintVec;

       Branch(Vec2f const& orig, Vec2f dir,
              float r1, float r2, float m1, float m2, float len, float stiffness=1.f)
       {
          int n = int((len + r1 + r2) / (r1 + r2));
          float r = r1, dr = (r2 - r1) / (n - 1), x = 0.f;
          float m = m1, dm = (m2 - m1) / (n - 1);
          
          particleVec.reserve(n);
          dir.normalize();
          
          for(int i = 0; i < n; i++)
          {
             Particle* p = new Particle(orig + dir * x, m, r);
             particleVec.push_back(p);
             x += r;
             r += dr;
             m += dm;
             x += r;
          }
          
          for(int i = 0; i < n - 1; i++)
          {                      
             StickCon* c = new StickCon(particleVec[i], particleVec[i + 1], 1.0);
             constraintVec.push_back(c);
          }
          
          for(int i = 0; i < n - 2; i++)
          {
             StickCon* c = new StickCon(particleVec[i], particleVec[i + 2], 1.0);
             constraintVec.push_back(c);
          }
          
          for(int i = 0; i < n - 3; i++)
          {                                            
             StickCon* c = new StickCon(particleVec[i], particleVec[i + 3], 1.0);
             constraintVec.push_back(c);
          }
          
          constraintVec.push_back(new StickCon(particleVec[0], particleVec[n - 1], stiffness));
       }
       
       ~Branch()
       {
          for(int i = 0; i < particleNum(); i++)
             delete particleVec[i];
          
          for(int i = 0; i < constraintNum(); i++)
             delete constraintVec[i];
       }
       
       Particle& at(int i)
       {
           return *particleVec.at(i);
       }
       
       Particle& first()
       {
           return *particleVec[0];
       }
       
       Particle& last()
       {
           return *particleVec[particleVec.size() - 1];
       }
       
       int particleNum()
       {
           return particleVec.size();
       }
       
       int constraintNum()
       {
           return constraintVec.size();
       }
       
       void tick()
       {
            for(int i = 0; i < particleNum(); i++)
                particleVec[i]->tick();
            
            for(int j = 0; j < 10; j++)
            for(int i = 0; i < constraintNum(); i++)
                constraintVec[i]->apply();
       }
       
       void draw(BITMAP* buffer, int c)
       {
          for(int i = 0; i < particleNum(); i++)
             particleVec[i]->draw(buffer, c);
       }
};

struct Tree {
       Branch* trunk;
       std::vector<Branch*> branchVec;
       std::vector<Constraint*> conVec;
       
       Tree(Vec2f const& rootPos)
       {
             trunk = new Branch(rootPos, Vec2f(0, -1),  20, 10, 10, 1, 500);
       }
       
       ~Tree()
       {
              for(int i = 0; i < conVec.size(); i++)
                  delete conVec[i];
              
              for(int i = 0; i < branchVec.size(); i++)
                  delete branchVec[i];
              delete trunk;
       }
       
       void tick()
       {
       }
};

int main()
{   
    bool exit = false;
    
    init();
    
    //float thrust = 1000.0;
    
    Vec2f rootPos(SCREEN_W / 2, SCREEN_H - 200);
    Branch br(rootPos, Vec2f(0, -1),  20, 10, 10, 1, 500);
    
    StaticStickCon s1(&br.at(1), rootPos + Vec2f(-100, 0), 0.0001);
    StaticStickCon s2(&br.at(1), rootPos + Vec2f(100, 0), 0.0001);
    
    Branch br2(br.at(3).pos, Vec2f(20, -10), 10, 5, 1, 1, 200, 0.01);
    
    SpringCon s3(&br.at(3), &br2.first(), 1);
    StickCon s4(&br.last(), &br2.at(2), 1.0);
    
    while(!exit)
    {
     Particle& root = br.first();
     Particle& tip = br2.last();
     
     if(key[KEY_ESC]) exit = true;
     
     //if(key[KEY_LEFT]) root.force.x -= thrust;
     //if(key[KEY_RIGHT]) root.force.x += thrust;
     //if(key[KEY_UP]) root.force.y -= thrust;
     //if(key[KEY_DOWN]) root.force.y += thrust;
     
     Vec2f delta = Vec2f(mouse_x, mouse_y) - tip.pos;
     
     if(mouse_b == 1)
     tip.pos += delta * 0.001;
     
     root.pos = rootPos;
     
     br.tick();
     br2.tick();
     
     for(int i = 0; i < 10; i++)
     {
        br.constrain();
        s1.apply();
        s2.apply();
        s3.apply();
        s4.apply();
        
        br2.constrain();
     }
     
     clear_to_color(buffer, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     br.draw(buffer, makecol(255, 255, 255));
     br2.draw(buffer, makecol(255, 0, 0));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
