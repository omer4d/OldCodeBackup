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

BITMAP* create_minkowsky(BITMAP* in, int rx, int ry, int col)
{
    int w = in->w, h = in->h;
    BITMAP* out = create_bitmap(w, h);
        
    blit(in, out, 0, 0, 0, 0, w, h);
    
    for(int j = 1; j < h - 1; ++j)
      for(int i = 1; i < w - 1; ++i)
          if(isOutlinePixel(in, i, j))
             ellipsefill(out, i, j, rx, ry, col);
}

void outline(std::vector<Vec2f>& out, BITMAP* in)
{
   out.clear();
       
   for(int j = 1; j < in->h - 1; ++j)
      for(int i = 1; i < in->w - 1; ++i)
          if(isOutlinePixel2(in, i, j))
             out.push_back(Vec2f(i, j));
}

int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* terrain = load_bitmap("terrain.bmp", NULL);
    BITMAP* minkowsky = create_bitmap(terrain->w, terrain->h);
    
    BITMAP* skin_terrain = create_bitmap(terrain->w, terrain->h);
    BITMAP* skin_minkowsky = create_bitmap(terrain->w, terrain->h);
    
    blit(terrain, minkowsky, 0, 0, 0, 0, terrain->w, terrain->h);
    blit(terrain, skin_terrain, 0, 0, 0, 0, terrain->w, terrain->h);
    
    for(int j = 1; j < terrain->h - 1; ++j)
      for(int i = 1; i < terrain->w - 1; ++i)
          if(isOutlinePixel(terrain, i, j))
          {
             ellipsefill(minkowsky, i, j, PLAYER_W, PLAYER_H, makecol(0, 255, 0));
             ellipsefill(skin_terrain, i, j, 5, 5, makecol(0, 255, 0));
          }
    
    blit(skin_terrain, skin_minkowsky, 0, 0, 0, 0, terrain->w, terrain->h);
    
    for(int j = 1; j < skin_terrain->h - 1; ++j)
      for(int i = 1; i < skin_terrain->w - 1; ++i)
          if(isOutlinePixel(skin_terrain, i, j))
          {
             ellipsefill(skin_minkowsky, i, j, PLAYER_W, PLAYER_H, makecol(255, 0, 0));
          }
    
    std::vector<Vec2f> points;

    for(int j = 1; j < minkowsky->h - 1; ++j)
      for(int i = 1; i < minkowsky->w - 1; ++i)
          if(isOutlinePixel2(minkowsky, i, j))
          {
             points.push_back(Vec2f(i, j));
          }
    
    Player player(Vec2f(100, 100));
    bool collision = false;

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
     
        Vec2f n;
        Vec2f p1 = player.pos + Vec2f(0, 5);
        p1.x = (int)p1.x;
        p1.y = (int)p1.y;
        n = ( findClosestPoint(points, p1) - p1).unit();
        
        if(player.pos.x >= 0 && player.pos.y >= 0 &&
           player.pos.x < minkowsky->w && player.pos.y < minkowsky->h &&
           PIXEL(skin_minkowsky, (int)player.pos.x, (int)player.pos.y) != 0)
        {
           player.pos = findClosestPoint(points, player.pos);
           player.vel.y = 0;
           collision = true;
        }
        
        else
            collision = false;
     
        clear_to_color(buffer, makecol(0, 0,0));

        draw_sprite(buffer, terrain, 0, 0);
        ellipsefill(buffer, (int)player.pos.x, (int)player.pos.y, PLAYER_W, PLAYER_H, collision ? makecol(255, 255, 0) : makecol(0, 255, 0));
        
        if(collision)
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
