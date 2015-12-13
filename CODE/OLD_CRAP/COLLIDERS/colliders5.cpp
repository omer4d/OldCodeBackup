#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define EMPTY makecol(0, 0, 0)
#define COLLIDER makecol(255, 0, 0)
#define VISITED makecol(0, 0, 255)

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

std::list<Vec2f>::iterator nextPoint(Vec2f const& a, Vec2f const& b, std::list<Vec2f>& points)
{
   float dist, minDist = 1000000000;
   
   for(std::list<Vec2f>::iterator i = points.begin(); i != points.end(); ++i)
     if((dist = Vec2f::squaredDist(*i, b)) < minDist)
        minDist = dist;

   std::list<Vec2f>::iterator minProjIter = points.end();
   float proj, minProj = 100.0;
   Vec2f v1 = (a - b).unitNormal();
   
   for(std::list<Vec2f>::iterator i = points.begin(); i != points.end(); ++i)
     if(fabs(Vec2f::squaredDist(*i, b) - minDist) < 0.001)
     {
        Vec2f v2 = (*i - b).unit();
        if((proj = fabs(v1.dot(v2))) < minProj)
        {
           minProj = proj;
           minProjIter = i;
        }
     }
     
   return minProjIter;
}


struct Point {
  float x, y, a;
  
  Point(float x, float y, float a)
  {
     this->x = x;
     this->y = y;
     this->a = a;
  }
};

bool comparePoints(Point const& p1, Point const& p2)
{
   return p1.a < p2.a;
}

void proc(std::vector<Vec2f>& points, BITMAP* bmp)
{
     std::list<Point> pointList;
     Vec2f center;
 
     for(int j = 1; j < bmp->h - 1; j++)
       for(int i = 1; i < bmp->w - 1; i++)
       {
          if(isOnBorder(bmp, i, j))
          {
            pointList.push_back(Point(i, j, 0));
            center += Vec2f(i, j);
            PIXEL(bmp, i, j) = VISITED;
          }
       }
       
     center *= 1.0f / pointList.size();
     
     for(std::list<Point>::iterator i = pointList.begin(); i != pointList.end(); ++i)
     {
       i->a = atan2(i->y - center.y, i->x - center.x);
     }
     
     pointList.sort(comparePoints);
     
     while(!pointList.empty())
     {
       Point p = pointList.front();
       
       points.push_back(Vec2f(p.x, p.y));
       pointList.pop_front();
     }
     
     /*
     points.push_back(pointList.front());
     pointList.pop_front();
     points.push_back(pointList.front());
     pointList.pop_front();
     
    while(!pointList.empty())
     {
        std::list<Vec2f>::iterator j = nextPoint(points[points.size() - 2], points[points.size() - 1], pointList);
        
        points.push_back(*j);
        pointList.erase(j);
     }*/
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
    
    for(int i = 0; i < (temp[src].size() + 1) / 2; i++)
    {
      Vec2f& a = temp[src][(offs + i * 2) % temp[src].size()];
      Vec2f& b = temp[src][(offs + i * 2 + 1) % temp[src].size()];
      Vec2f& c = temp[src][(offs + i * 2 + 2) % temp[src].size()];
      Vec2f v1 = (a - b).unit(), v2 = (c - b).unit();
      float cosa = v1.dot(v2);
      if(cosa < -0.999999) cosa = -0.999999;
      if(cosa > 0.999999) cosa = 0.999999;
    
      if(acos(cosa) > 2.9)
      {
        temp[dest].push_back(a);
      }
    
      else
      {
        temp[dest].push_back(a);
        
        int idx = offs + i * 2 + 1;
        if(idx < temp[src].size() || idx % temp[src].size() < offs)
        temp[dest].push_back(b);
      }
    }
   
    //if(temp[src].size() % 2)
       //temp[dest].push_back(temp[src][temp[src].size() - 1]);
    
    src = !src;
    dest = !dest;
  }
  
  points = temp[src];
}

float squared(float x)
{
  return x * x;
}

int wrapi(int n, int min, int max)
{
    int t = n - min, l = max - min;
    int mod = t % l;
    return mod >= 0 ? min + mod : min + l + mod;
}

float lineReg(std::vector<Vec2f> const& points, int first, int n)
{
   Vec2f avg(0, 0);
   
   for(int i = 0; i < n; i++)
     avg += points[wrapi(first + i, 0, points.size())];
   avg *= 1.0f / float(n);
   
   float sx = 0, sy = 0, sxy = 0;
   
   for(int i = 0; i < n; i++)
   {
      Vec2f const& p = points[wrapi(first + i, 0, points.size())];
      sx += squared(p.x - avg.x);
      sy += squared(p.y - avg.y);
      sxy += (p.x - avg.x) * (p.y - avg.y);
   }
   
   if(fabs(sx) > 0.001)
   {
     float m = sxy / sx;
     float b = avg.y - m * avg.x;
     float dist = 0.0;
   
     for(int i = 0; i < n; i++)
     {
       Vec2f const& p = points[wrapi(first + i, 0, points.size())];
       dist += fabs(p.y - m * p.x - b) / sqrt(m * m + 1);
     }
   
     return dist;
   }
   
   else
   {
     float dist = 0;
     
     for(int i = 0; i < n; i++)
     {
       Vec2f const& p = points[wrapi(first + i, 0, points.size())];
       dist += fabs(p.x - avg.x);
     }
     
     return dist;
   }
}



void simplify2(std::vector<Vec2f>& points, int offs, int iter)
{
  std::vector<Vec2f> temp[2];
  int src = 0, dest = 1;
  temp[0] = points;
  
  int n = 8;

    temp[dest].clear();
    
    for(int i = 0; i < (temp[src].size() + n - 1) / n; i++)
    {
      if(lineReg(temp[src], i * n, n) / float(n) < 0.5)
        temp[dest].push_back(temp[src][i * n]);
        
      else
        for(int j = 0; j < n; j++)
         if(i * n + j < temp[src].size())
         {
           temp[dest].push_back(temp[src][i * n + j]);
         }
         else
         printf("!");
    }
    
    src = !src;
    dest = !dest;
  
  points = temp[src];
}


float randf()
{
      return float(rand()) / float(RAND_MAX);
}

float randf(float min, float max)
{
      return min + (max - min) * randf();
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* test = load_bitmap("test.bmp", NULL);
    int lmb = 0;
    
    std::vector<Vec2f> points, points2;
    
    /*
    Vec2f a(100.0, 100.0), b(220.0, 340.0), n = (b - a).normal().unit();
    std::vector<Vec2f> temp;
    int num = 6;
    float slope = (a.y - b.y) / (a.x - b.x), inter = a.y - slope * a.x;
    
    printf("%f %f\n", slope, inter);
    
    for(int i = 0; i < num; i++)
    {
       float k = float(i) / float(num - 1);
       float t = i % 2 ? -1 : 1;
       Vec2f p = a + (b - a) * k + n * randf(-1000, 1000);
       temp.push_back(p);
    }
    
    printf("%f", lineReg(temp, 0, num));*/
    
    Vec2f center;
    
    while(!exit)
    {
                
     if(key[KEY_ESC]) exit = true;

     

     if(mouse_b && !lmb)
     {
       points.clear();
       proc(points, test);
       
       for(int i = 0; i < points.size(); i++)
         center += points[i];
         
       center *= 1.0f / points.size();
       
       
       printf("%d - \n", points.size());
       simplify2(points, 0, 1);
       
       
       for(int j = 0; j < 100; j++)
       {
         simplify(points, 0, 1);
         simplify(points, 1, 1);
         printf("%d\n", points.size());
       }
       
       //simplifyDebug(points, 0, 1);
     }
     
     lmb = mouse_b;

     clear_to_color(buffer, 0);
     
     if(!key[KEY_SPACE])
       draw_sprite(buffer, test, 0, 0);
       
     if(points.size())
     {
      int i;
      for(i = 0; i < (int)(points.size()) - 1; i++)
        DrawVec2f::line(buffer, points[i], points[i + 1], makecol(0, 0, 255));
      DrawVec2f::line(buffer, points[i], points[0], makecol(0, 0, 255));
     }
     
     //if(points.
     for(int i = 0; i < points.size(); i++)
       DrawVec2f::circlefill(buffer, points[i], 0, makecol(0, 255, 0));
     
     //DrawVec2f::circlefill(buffer, center, 10, makecol(0, 255, 0));
     
     Vec2f a(200, 200), b(300, 100), c(400, 0);
     
     /*
     DrawVec2f::line(buffer, a, b, makecol(255, 255, 255));
     DrawVec2f::line(buffer, c, b, makecol(255, 255, 255));
     Vec2f v1 = (a - b).unit(), v2 = (c - b).unit();
     
     printf("(%f %f, %f %f) = %f\n", v1.x, v1.y, v2.x, v2.y, v1.dot(v2));*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(test);
    deinit();
    return 0;
}END_OF_MAIN()
