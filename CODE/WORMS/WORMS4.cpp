#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
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

#define PLAYER_W 4
#define PLAYER_H 4

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
      vel.y += 2.f * dt;
      pos += vel * dt;
   }
};

BITMAP* create_minkowsky(BITMAP* in, int rx, int ry, int col)
{
    int w = in->w, h = in->h;
    BITMAP* out = create_bitmap(w, h);
        
    blit(in, out, 0, 0, 0, 0, w, h);
    
    for(int j = 1; j < h - 1; ++j)
      for(int i = 1; i < w - 1; ++i)
          if(isOutlinePixel(in, i, j))
             ellipsefill(out, i, j, rx, ry, col);
    
    return out;
}

std::vector<Vec2f> outline(BITMAP* in)
{
   std::vector<Vec2f> out;
   
   for(int j = 1; j < in->h - 1; ++j)
      for(int i = 1; i < in->w - 1; ++i)
          if(isOutlinePixel2(in, i, j))
             out.push_back(Vec2f(i, j));
   
   return out;
}

float sum(std::vector<float> const& v)
{
    float s = 0.0;
    for(int i = 0; i < v.size(); ++i)
       s += v[i];
    return s;
}

float dot_product(std::vector<float> const& v1, std::vector<float> const& v2)
{
    float s = 0.0;
    int n = v1.size() < v2.size() ? v1.size() : v2.size();
    
    for(int i = 0; i < n; ++i)
       s += v1[i] * v2[i];
    return s;
}

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
    const float s_x   = sum(xs);
    const float s_y   = sum(ys);
    const float s_xx  = dot_product(xs, xs);
    const float s_xy  = dot_product(xs, ys);
    const float numer = n * s_xy - s_x * s_y;
    const float denom = n * s_xx - s_x * s_x;
    
    Vec2f norm;
    
    if(fabs(denom) < 0.0001)
      norm = Vec2f(-1.f, 0.f);
    else
      norm = Vec2f(denom, numer).normal().unit();
      
    Vec2f p2 = p + norm * 5;
      
    if(PIXEL(bmp, int(p2.x), int(p2.y)) != 0)
      norm *= -1.f;
    
    return norm;
}

float sgn(float x)
{
   return x < 0 ? -1 : 1;
}

int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* terrain = load_bitmap("terrain.bmp", NULL);
    BITMAP* minkowsky = create_minkowsky(terrain, PLAYER_W, PLAYER_H, makecol(0, 255, 0));
    BITMAP* skin_terrain = create_minkowsky(terrain, 2, 2, makecol(255, 0, 0));
    BITMAP* skin_minkowsky = create_minkowsky(skin_terrain, PLAYER_W, PLAYER_H, makecol(0, 255, 0));
  
    std::vector<Vec2f> points = outline(minkowsky);
    std::vector<Vec2f> points2 = outline(skin_minkowsky);
    
    Player player(Vec2f(100, 100));
    bool collision = false;
    Vec2f n;

    while(!exit)
    { 
        if(key[KEY_ESC]) exit = true;
     
        if(key[KEY_RIGHT]) player.pos.x += 0.1;
        if(key[KEY_LEFT]) player.pos.x -= 0.1;
     
        if(mouse_b == 1)
        {
            player.pos.init(mouse_x, mouse_y);
            player.vel.init(0, 0);
        }
     
        player.logic(0.01f);
        
        if(player.pos.x >= 0 && player.pos.y >= 0 &&
           player.pos.x < minkowsky->w && player.pos.y < minkowsky->h &&
           PIXEL(minkowsky, (int)player.pos.x, (int)player.pos.y) != 0)
        {
           player.pos = findClosestPoint(points, player.pos);
        }
        
        if(player.pos.x >= 0 && player.pos.y >= 0 &&
           player.pos.x < minkowsky->w && player.pos.y < minkowsky->h &&
           PIXEL(skin_minkowsky, (int)player.pos.x, (int)player.pos.y) != 0)
        {
              n = findNormal(minkowsky, player.pos, 5.f);
              Vec2f t = n.normal();
           
              float ln = player.vel.dot(n);
              float lt = player.vel.dot(t);
              
              if(ln < 0.f)
              {
                 ln *= -0.5f;
              }
              
              //lt = MAX(fabs(lt) - 0.008, 0) * sgn(lt);
              
              Vec2f vn = n * ln;
              Vec2f vt = t * lt;
           
              player.vel = vn + vt;
           collision = true;
        }
        
        else
            collision = false;
     
        clear_to_color(buffer, makecol(0, 0,0));

        draw_sprite(buffer, terrain, 0, 0);
        ellipsefill(buffer, (int)player.pos.x, (int)player.pos.y, PLAYER_W, PLAYER_H, collision ? makecol(0, 255, 0) : makecol(0, 255, 0));
        

        //if(collision)
           line(buffer, (int)player.pos.x, (int)player.pos.y,
                        (int)(player.pos.x + n.x * 100), (int)(player.pos.y + n.y * 100), makecol(255, 0, 0));
        
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(skin_minkowsky);
    destroy_bitmap(skin_terrain);
    destroy_bitmap(minkowsky);
    destroy_bitmap(terrain);
    
    deinit();
    return 0;
}END_OF_MAIN()
