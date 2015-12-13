#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define EMPTY makecol(0, 0, 0)
#define COLLIDER makecol(255, 255, 255)
#define VISITED makecol(255, 0, 0)

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

bool isOnBorder(BITMAP* bmp, int x, int y)
{
  return PIXEL(bmp, x, y) == COLLIDER &&
         (PIXEL(bmp, x + 1, y) == EMPTY ||
          PIXEL(bmp, x - 1, y) == EMPTY ||
          PIXEL(bmp, x, y + 1) == EMPTY ||
          PIXEL(bmp, x, y - 1) == EMPTY);
}

void proc(std::vector<Vec2f>& points, BITMAP* bmp, int x, int y)
{
     if(PIXEL(bmp, x, y) != COLLIDER)
       return;
     
     while(PIXEL(bmp, x, y - 1) == COLLIDER)
       y--;
       
     bool finished = false;
     Vec2f last(-1000, -1000), curr;
     
     while(!finished)
     {
        curr.init(x, y);
        PIXEL(bmp, x, y) = VISITED;
        
        //if(Vec2f::squaredDist(curr, last) > 1.0)
        //{
           points.push_back(curr);
           //last = curr;
        //}
        
        if(isOnBorder(bmp, x + 1, y)) { x++; }
        else if(isOnBorder(bmp, x + 1, y + 1)) { x++; y++; }
        else if(isOnBorder(bmp, x, y + 1)) { y++; }
        else if(isOnBorder(bmp, x - 1, y + 1)) { x--; y++; }
        else if(isOnBorder(bmp, x - 1, y)) { x--; }
        else if(isOnBorder(bmp, x - 1, y - 1)) { x--; y--; }
        else if(isOnBorder(bmp, x, y - 1)) { y--; }
        else if(isOnBorder(bmp, x + 1, y - 1)) { x++; y--; }
        
        else finished = true;
     }
}

float vec2fang(Vec2f const& a, Vec2f const& b, Vec2f const& c)
{
  Vec2f v1 = a - b;
  Vec2f v2 = c - b;
  return atan2(v1.dot(v2.normal()), v1.dot(v2));
}

void simplify(std::vector<Vec2f>& points, int offs, int iter)
{
  std::vector<Vec2f> temp[2];
  int src = 0, dest = 1;
  temp[0] = points;

  for(int j = 0; j < iter; j++)
  {
    temp[dest].clear();
    
    for(int i = 0; i < temp[src].size() / 2; i++)
    {
      Vec2f& a = temp[src][(offs + i * 2) % temp[src].size()];
      Vec2f& b = temp[src][(offs + i * 2 + 1) % temp[src].size()];
      Vec2f& c = temp[src][(offs + i * 2 + 2) % temp[src].size()];
      Vec2f v1 = (a - b).unit(), v2 = (c - b).unitNormal();
    
      if(fabs(v1.dot(v2)) < 0.01)
      {
        temp[dest].push_back(a);
      }
    
      else
      {
        temp[dest].push_back(a);
        temp[dest].push_back(b);
      }
    }
    
    src = !src;
    dest = !dest;
  }
  
  points = temp[dest];
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* test = load_bitmap("test.bmp", NULL);
    int lmb = 0;
    std::vector<Vec2f> points, points2;
    
    while(!exit)
    {
                
     if(key[KEY_ESC]) exit = true;

     if(mouse_b && !lmb)
     {
       points.clear();
       proc(points, test, mouse_x, mouse_y);
       printf("%d - ", points.size());
       //simplify(points, 0, 100);
       //simplify(points, 1, 100);
       printf("%d\n", points.size());
     }
     
     lmb = mouse_b;

     clear_to_color(buffer, 0);
     
     if(!key[KEY_SPACE])
       draw_sprite(buffer, test, 0, 0);
     
     /*
     if(points.size())
     {
      int i;
     for(i = 0; i < (int)(points.size()) - 1; i++)
       DrawVec2f::line(buffer, points[i], points[i + 1], makecol(0, 255, 0));
     DrawVec2f::line(buffer, points[i], points[0], makecol(0, 255, 0));
     
     //DrawVec2f::circlefill(buffer, points[i], 2, makecol(0, 255, 0));
     //DrawVec2f::circlefill(buffer, points[0], 2, makecol(0, 255, 0));
     }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(test);
    deinit();
    return 0;
}END_OF_MAIN()
