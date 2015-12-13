#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include "Vec2f.hpp"
#include "Poly.hpp"
#include "DrawVec2f.hpp"

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

static bool clipSeg(Vec2f& p, Vec2f const& v1, Vec2f const& v2, Vec2f const& c1, Vec2f const& c2)
{
  Vec2f n = (c1 - c2).normal();
  float d1 = (v1 - c1).dot(n);
  float d2 = (v2 - c1).dot(n);
  
  if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
  {
     float k = fabs(d1) / (fabs(d1) + fabs(d2));
     p = v1 + (v2 - v1) * k;
     return true;
  }
  
  return false;
}

template <class T>
void circularErase(std::list<T>& list, typename std::list<T>::iterator i0, typename std::list<T>::iterator i1)
{
     typename std::list<T>::iterator i;
     i0++;
     
     if(i0 == list.end())
       i0 = list.begin();
     
     for(i = i0; i != i1;)
     {
        i = list.erase(i);
        
        if(i == list.end())
          i = list.begin();
     }
}

    int whichSide(Vec2f const& c1, Vec2f const& c2, Vec2f const& p)
    {
        Vec2f n = (c1 - c2).unitNormal();
        float d1 = (p - c1).dot(n);
        
        if(d1 < 0)
          return -1;
        else
          return 1;
    }

struct Clipper {
      typedef std::list<Vec2f> PointList;
      typedef std::list<Vec2f>::iterator PointListIter;
      
      PointList pointList;
      
      void pushPoint(Vec2f const& p)
      {
           pointList.push_back(p);
      }
      
      Vec2f popPoint()
      {
           Vec2f temp = pointList.front();
           
           if(!pointList.empty())
              pointList.pop_front();
           return temp;
      }
      
      bool isEmpty()
      {
           return pointList.empty();
      }
      
      void clear()
      {
           pointList.clear();
      }
      
      void clip(Vec2f const& c1, Vec2f const& c2, int clipSide=-1)
      {
           Vec2f p[2];
           PointListIter ip[2];
           int ipNum = 0;
           
           for(PointListIter i = pointList.begin(); i != pointList.end(); ++i)
           {
               PointListIter i2 = i;
               ++i2;
               
               if(i2 == pointList.end())
                  i2 = pointList.begin();
               
               if(clipSeg(p[ipNum], *i, *i2, c1, c2))
               {
                  ip[ipNum] = i2;
                  ipNum++;
               }
           }
           
           if(ipNum == 2)
           {
             Vec2f temp = *ip[0];
             
             ip[0] = pointList.insert(ip[0], p[0]);
             ip[1] = pointList.insert(ip[1], p[1]);
             
             if(whichSide(c1, c2, temp) == clipSide) circularErase(pointList, ip[1], ip[0]);
             else                                    circularErase(pointList, ip[0], ip[1]);
           }
      }
};



int main()
{
    bool exit = false;
    
    init();
    
    std::list<int> test;
    test.push_back(1);
    test.push_back(2);
    test.push_back(3);
    test.push_back(4);
    test.push_back(5);
    test.push_back(6);
    test.push_back(7);
    test.push_back(8);
    
    circularErase<int>(test, ++++++test.begin(), ++test.begin());
    
    for(std::list<int>::iterator i = test.begin(); i != test.end(); i++)
      printf("%d ", *i);
    
    Vec2f a(100, 100), b(200, 200);
    Clipper clipper;
    
    Poly poly;
    poly.init(320, 240, 100, 5);
    
    Poly poly2;
    poly2.init(310, 240, 100, 4);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1)
       a.init(mouse_x, mouse_y);
     if(mouse_b == 2)
       b.init(mouse_x, mouse_y);
       
     clipper.clear();
     
     for(int i = 0; i < poly.vertexVec.size(); i++)
       clipper.pushPoint(poly.vertexVec[i]);
     
     int n = poly2.vertexVec.size();
     for(int i = 0; i < poly2.vertexVec.size(); i++)
       clipper.clip(poly2.vertexVec[i], poly2.vertexVec[(i + 1) % n]);
     clipper.clip(a, b);  
     
     Poly result;
     
     while(!clipper.isEmpty())
     {
         result.vertexVec.push_back(clipper.popPoint());
     }
     
     clear_to_color(buffer, 0);

     DrawVec2f::line(buffer, a, b, makecol(255, 255, 255));
     DrawVec2f::circlefill(buffer, a, 4, makecol(0, 0, 255));
     
     poly.draw(buffer, -1, makecol(255, 0, 0));
     result.draw(buffer, makecol(255, 0, 0));
     
     poly2.draw(buffer, -1, makecol(0, 0, 255));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
