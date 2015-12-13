#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>
#include <algorithm>

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

#define PLAYER_W 5
#define PLAYER_H 10

bool isOutlinePixel(BITMAP* src, int x, int y)
{
   return  PIXEL(src, x, y) != 0 &&
             (PIXEL(src, x + 1, y) == 0 ||
              PIXEL(src, x - 1, y) == 0 ||
              PIXEL(src, x, y + 1) == 0 ||
              PIXEL(src, x, y - 1) == 0);
}

bool isOutlinePixel2(BITMAP* src, int x, int y)
{
   return  PIXEL(src, x, y) == 0 &&
             (PIXEL(src, x + 1, y) != 0 ||
              PIXEL(src, x - 1, y) != 0 ||
              PIXEL(src, x, y + 1) != 0 ||
              PIXEL(src, x, y - 1) != 0);
}

float fakeDist(Vec2f a, Vec2f b)
{
   float dx = (a.x - b.x);
   float dy = a.y - b.y;
   
   return dx*dx + dy*dy;
}

Vec2f findClosestPoint(std::vector<Vec2f> const& vec, Vec2f p)
{
   float mind = fakeDist(vec[0], p);
   Vec2f closest = vec[0];
   
   for(int i = 1; i < vec.size(); ++i)
   {
      float d = fakeDist(vec[i], p);
    
      if(d < mind)
      {
          mind = d;
          closest = vec[i];
      }
   }
   
   return closest;
}

struct Player {
   Vec2f pos, vel;
   
   Player(Vec2f p): pos(p)
   {
   }
   
   void logic(float dt)
   {
      vel.y += 10.f * dt;
      pos += vel * dt;
   }
};

Vec2f findNormal(BITMAP* bmp, Vec2f p, float rad)
{
   std::vector<float> xs, ys;
   
   for(int y = int(p.y - rad); y < int(p.y + rad); ++y)
      for(int x = int(p.x - rad); x < int(p.x + rad); ++x)
         if(Vec2f::squaredDist(p, Vec2f(x, y)) <= rad * rad)
         {
             if(isOutlinePixel(bmp, x, y))
             {
                 xs.push_back(x);
                 ys.push_back(y);
             }
         }
         
    const float n     = xs.size();
    const float s_x   = std::accumulate(xs.begin(), xs.end(), 0.0);
    const float s_y   = std::accumulate(ys.begin(), ys.end(), 0.0);
    const float s_xx  = std::inner_product(xs.begin(), xs.end(), xs.begin(), 0.0);
    const float s_xy  = std::inner_product(xs.begin(), xs.end(), ys.begin(), 0.0);
    const float numer = n * s_xy - s_x * s_y;  // The same regardless of inversion (both terms here are commutative)
    const float denom = n * s_xx - s_x * s_x;  // Will change if inverted; use this for now
    
    Vec2f norm;
    
    if(fabs(denom) < 0.0001)
      norm = Vec2f(-1.f, 0.f);
    else
      norm = Vec2f(denom, numer).normal().unit();
      
    Vec2f p2 = p + norm * 2;
      
    if(PIXEL(bmp, int(p2.x), int(p2.y)) != 0)
      norm *= -1.f;
    
    return norm;
}

int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* terrain = load_bitmap("terrain.bmp", NULL);
    BITMAP* minkowsky = create_bitmap(terrain->w, terrain->h);
    
    blit(terrain, minkowsky, 0, 0, 0, 0, terrain->w, terrain->h);
    
    for(int j = 1; j < minkowsky->h - 1; ++j)
      for(int i = 1; i < minkowsky->w - 1; ++i)
          if(isOutlinePixel(terrain, i, j))
             ellipsefill(minkowsky, i, j, PLAYER_W, PLAYER_H, makecol(0, 255, 0));
    
    std::vector<Vec2f> points;
    std::vector<Vec2f> normals;

    for(int j = 1; j < minkowsky->h - 1; ++j)
      for(int i = 1; i < minkowsky->w - 1; ++i)
          if(isOutlinePixel2(minkowsky, i, j))
          {
             points.push_back(Vec2f(i, j));
             normals.push_back(findNormal(minkowsky, Vec2f(i, j), 5.f));
          }
    
    Player player(Vec2f(100, 100));
    
    Vec2f p, n;
    bool collision = false;
    
    float dx = 0, dy = 0;
    
    while(!exit)
    { 
        if(key[KEY_ESC]) exit = true;
     
        if(key[KEY_RIGHT]) dx += 1;
        if(key[KEY_LEFT]) dx -= 1;
        if(key[KEY_UP]) dy -= 1;
        if(key[KEY_DOWN]) dy += 1;
        
     
        if(mouse_b == 1)
        {
           //p = Vec2f(mouse_x, mouse_y);
           //n = findNormal(terrain, p, 5.f);
         
          player.pos.init(mouse_x, mouse_y);
          player.vel.init(0, 0);
        }
     
        player.logic(0.01f);
        
        if(player.pos.x >= 0 && player.pos.y >= 0 &&
           player.pos.x < minkowsky->w && player.pos.y < minkowsky->h &&
                    PIXEL(minkowsky, (int)player.pos.x, (int)player.pos.y) != 0)
        {
           collision = true;
           Vec2f p1 = findClosestPoint(points, player.pos);
           
           if(Vec2f::squaredDist(p1, player.pos) > 0.f)
           {
               n =  findNormal(minkowsky, player.pos, 5.f); //(p1 - player.pos).unit();
              Vec2f t = n.normal();
              player.pos = p1;
           
              float ln = player.vel.dot(n);
              float lt = player.vel.dot(t);
              
              if(ln < 0.f)
              {
                 ln *= -0.2f;
              }
              
              
              
              lt *= 0.9f;
              
              Vec2f vn = n * ln;
              Vec2f vt = t * lt;
           
              
              //printf("(%f, %f)\n", n.x, n.y);
           
              player.vel = vn + vt;
              
              //if(player.vel.y > 0.f)
                //player.vel.y = 0.f;
              
              if(fabs(n.x) > 0.4f)
              printf("%f, (%f %f)\n", ln, n.x, n.y);
           }
        }
        
        else
          collision = false;
     
        clear_to_color(buffer, makecol(0, 0,0));

        //draw_sprite(buffer, minkowsky, 0, 0);
        ellipsefill(buffer, (int)player.pos.x, (int)player.pos.y, PLAYER_W, PLAYER_H, makecol(0, 0, 255));
        
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        
        if(collision == true)
        {
           Vec2f p = player.pos;
          //line(buffer, int(p.x), int(p.y), int(p.x + n.x * 10), int(p.y + n.y * 10), makecol(255, 0, 0));
        }
        
        for(int i = 0; i < normals.size(); ++i)
        {
           line(buffer, (int)(points[i].x * 4 - dx), (int)(points[i].y * 4 - dy),
                        (int)(points[i].x * 4 + normals[i].x * 20 - dx), (int)(points[i].y * 4 + normals[i].y * 20 - dy),
                        makecol(50, 50, 0));
        }
        
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(minkowsky);
    destroy_bitmap(terrain);
    
    deinit();
    return 0;
}END_OF_MAIN()
