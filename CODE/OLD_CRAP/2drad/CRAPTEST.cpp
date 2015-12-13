#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <algorithm>

#include "Keyboard.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#include <vector>

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

Vec2f genPoint(Vec2f const& a, Vec2f const& b, Vec2f const& c, float w)
{
 Vec2f v1 = a - b, v2 = c - b;
 Vec2f n1 = (b - a).unitNormal() * w, n2 = (c - b).unitNormal() * w;
 Vec2f dn = n2 - n1;
 float k2 = dn.cross(v1) / v1.cross(v2);
 return b + v2 * k2 + n2;
}

struct Poly {
 std::vector<Vec2f> pointVec;
};

void drawPoly(BITMAP* buffer, Poly const& poly, int c)
{
 int i;
 for(i = 0; i < int(poly.pointVec.size()) - 1; i++)
 {
  Vec2f n = (poly.pointVec[i + 1] - poly.pointVec[i]).unitNormal() * 20;
  Vec2f m = (poly.pointVec[i + 1] + poly.pointVec[i]) * 0.5;
  
  DrawVec2f::line(buffer, poly.pointVec[i], poly.pointVec[i + 1], c);
  //DrawVec2f::line(buffer, m, m + n, c);
 }
 
 Vec2f n = (poly.pointVec[0] - poly.pointVec[i]).unitNormal() * 20;
 Vec2f m = (poly.pointVec[0] + poly.pointVec[i]) * 0.5;
 
 DrawVec2f::line(buffer, poly.pointVec[i], poly.pointVec[0], c);
 //DrawVec2f::line(buffer, m, m + n, c);
}

float polygonArea(std::vector<Vec2f> const& poly)
{
 float s = 0;
 
 for(int i = 0; i < poly.size(); i++)
 {
  int j = (i + 1) % poly.size();
  s += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
 }
 
 s *= 0.5;

 return s;
}

int main()
{
    bool exit = false;
    
    init();
    Keyboard keyboard;
    
    int lastMb = 0;
    
    std::vector<Vec2f> pointVec;
    std::list<Poly> polyList;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     int n = pointVec.size();
     
     if(keyboard.keyPressed(KEY_C) && n > 2)
     {
      if(polygonArea(pointVec) < 0)
      {
       reverse(pointVec.begin(), pointVec.end()); 
       }
      
      Poly poly;
      poly.pointVec = pointVec;
      
      
      polyList.push_back(poly);
      pointVec.clear();
     }
     
     if(keyboard.keyPressed(KEY_Z) && n > 1)
     {
      if(polygonArea(pointVec) < 0)
      {
       reverse(pointVec.begin(), pointVec.end()); 
       }
      
      Poly poly;
      poly.pointVec = pointVec;
      

      
      
      poly.pointVec.push_back(pointVec[n - 1] + (pointVec[n - 1] - pointVec[n - 2]).unitNormal() * 10);
      
      if(n > 2)
       for(int i = n - 2; i > 0; i--)
        poly.pointVec.push_back(genPoint(pointVec[i - 1], pointVec[i], pointVec[i + 1], 10));
        
      poly.pointVec.push_back(pointVec[0] + (pointVec[1] - pointVec[0]).unitNormal() * 10);
      
      
      polyList.push_back(poly);
      pointVec.clear();
     }

     keyboard.update();
     
     if(mouse_b == 1 && !lastMb)
     {
      pointVec.push_back(Vec2f(mouse_x, mouse_y));
     }
     
     lastMb = mouse_b;
     
     clear_to_color(buffer, makecol(128, 128, 128));
     
     for(std::list<Poly>::iterator i = polyList.begin(); i != polyList.end(); ++i)
      drawPoly(buffer, *i, makecol(0, 0, 128));
     
     for(int i = 0; i < n; i++)
      DrawVec2f::circlefill(buffer, pointVec[i], 2, 0);
      
     for(int i = 0; i < int(n) - 1; i++)
     {
      DrawVec2f::line(buffer, pointVec[i], pointVec[i + 1], 0);
     }
     
     if(n > 0)
      DrawVec2f::line(buffer, pointVec[n - 1], Vec2f(mouse_x, mouse_y), makecol(100, 100, 100));
     
     if(n > 2)
      DrawVec2f::line(buffer, pointVec[0], pointVec[n - 1], makecol(100, 0, 0));
     

     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
