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

struct Path {
   std::vector<Vec2f> pointVec;
   int curr;
   
   Path()
   {
      curr = 0;
   }
   
   void pushPoint(Vec2f const& p)
   {
      pointVec.push_back(p);
   }
   
   void inc()
   {
             curr++;
         curr = curr % pointVec.size();
   }
   
   void update(Vec2f const& pos, float dist)
   {
       int next = (curr + 1) % pointVec.size();
    
       if((pointVec[curr] - pos).unit().dot((pointVec[next] - pointVec[curr]).unit()) < -0.0 /*||
           Vec2f::squaredDist(pointVec[curr], pos) < dist * dist*/ )
       {
         curr++;
         curr = curr % pointVec.size();
      }
   }
   
   Vec2f getCurr()
   {  
      return pointVec[curr];
   }
   
   Vec2f getNext(int n)
   {
      return pointVec[(curr + n) % pointVec.size()];
   }
   
   
   int bestPoint(Vec2f const& pos, Vec2f const& vel, int ignore)
   {
      int mini = -1;
      float minScore = 10000000000.0;
    
      for(int i = 0; i < pointVec.size(); i++)
      {
       
         if(i != ignore)
         {
              float ang = (pointVec[i] - pos).unit().dot(vel.unit());
              float dist = (pointVec[i] - pos).length();
         
              float score = (1 - ang) * 200 + dist;
         
              if(score < minScore)
              {
                       minScore = score;
                       mini = i;
              }
         }
      }
      
     return mini;
   }
};

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
     vel *= 0.99;
     pos += vel * dt;
     
     //vel.y += 5;
  }
};


void parabola(Path& path, Vec2f p0, Vec2f p1, int n)
{
 
 float a=-(p1.y-p0.y)/(p1.x*p1.x -2*p0.x *p1.x+ p0.x*p0.x);
 
 if(p1.x < p0.x)
 {
   Vec2f temp = p1;
   p1 = p0;
   p0 = temp;
 }
 
 /*
  float a=-1000 / ((p0-p1).length() * (p0-p1).length());
  float b=(p1.y-p0.y-a*p1.x*p1.x+a*p0.x*p0.x) / (p1.x-p0.x);
  float c=p0.y-a*p0.x*p0.x-b*p0.x;
  */
  
  float dx = (p1.x - p0.x) / n;
  
  for(float x = p0.x; x < p1.x; x += dx)
  {
    Vec2f t;
    dx += dx * (p0 - p1).length() / 1000.0;

    
   
     path.pushPoint(Vec2f(x, a*(x-p1.x)*(x-p1.x)+p1.y));
  }
}




int main()
{
    bool exit = false;
    
    init();
    
    Boid boid;
    boid.pos.init(SCREEN_W / 2, SCREEN_H / 2);
    
    Path path;
    
    /*
    for(int i = 0; i < 100; i++)
    {
       float ang = i * 2*M_PI / 100.0;
       
       Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
       Vec2f p = c + Vec2f(200 * cos(ang * 5), 200 * sin(ang * 3));
       path.pushPoint(p);
    }*/
    

    
    boid.vel.init(0.1, 0.0);
    parabola(path, Vec2f(100, 100), Vec2f(400, 400), 20 );
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     Vec2f mouse(mouse_x, mouse_y);
     
     if(mouse_b == 1)
     {
      path.pointVec.clear();
            parabola(path, Vec2f(100, 100), mouse, 20 );
      
        //boid.vel += (mouse - boid.pos) * 0.02;
     }
      
     //path.update(boid.pos, 10);

      int bp1 = path.bestPoint(boid.pos, boid.vel, -1);
      int bp2 = path.bestPoint(boid.pos, boid.vel, bp1);
      
     Vec2f d1 = path.pointVec[bp1] - boid.pos;
     Vec2f d2 = path.pointVec[bp2] - boid.pos;
     
     float k = d1.unit().dot(d2.unit());
     
     boid.vel += d1 * 0.008 + d2 * 0.008; //+ d3 * 0.002;
     
     
     
     boid.simulate(0.01);
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);

     
     for(int i = 0; i < path.pointVec.size(); i++)
       DrawVec2f::circlefill(buffer, path.pointVec[i], 3, makecol(255, 0, 0));
     
     DrawVec2f::circlefill(buffer, path.pointVec[bp1], 4, makecol(0, 255, 255));
     DrawVec2f::circlefill(buffer, path.pointVec[bp2], 4, makecol(0, 255, 255));
     //DrawVec2f::circlefill(buffer, path.getNext(2), 4, makecol(0, 0, 255));
     
          DrawVec2f::circlefill(buffer, boid.pos, 3, makecol(255, 255, 0));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
