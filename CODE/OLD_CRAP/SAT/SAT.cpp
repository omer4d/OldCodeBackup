#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include "DrawVec2f.hpp"

#include "Vec2f.hpp"
#include "Mat9f.hpp"

#include <float.h>

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

std::vector<Vec2f> tempVec;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 800, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
     tempVec.reserve(200);
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct Interval {
       float min, max;
       
       Interval()
       {
          this->min = 0;
          this->max = 0;
       }
       
       Interval(float min, float max)
       {
          this->min = min;
          this->max = max;
       }
       
       static bool intersect(Interval const& i0, Interval const& i1)
       {
          return !(i0.min > i1.max || i1.min > i0.max);
       }
       
       static bool intersect(Interval const& i0, Interval const& i1, float& depth)
       {
          float d0 = i0.max - i1.min;
          float d1 = i1.max - i0.min;
          
          depth = (d0 < d1) ? -d0 : d1;
          
          return !(i0.min > i1.max || i1.min > i0.max);
       }
};

class Shape {
public:
       virtual Interval project(Mat9f const& mat, Vec2f const& v) const = 0;
       virtual ~Shape()
       {
       }
};

struct Poly: public Shape {
       std::vector<Vec2f> points;
       
       void init(float px, float py, float r, int vnum)
       {
          points.clear();
          points.reserve(vnum);
          
          for(int i = 0; i < vnum; i++)
          {
             float alpha = float(i) / float(vnum) * 2 * M_PI;
             Vec2f v(px + r * cos(alpha), py + r * sin(alpha));
             points.push_back(v);
          }
       }
       
       void draw(BITMAP* buffer, int col)
       {
          int i;
          for(i = 0; i < points.size() - 1; i++)
             DrawVec2f::line(buffer, points[i], points[i + 1], col);
          DrawVec2f::line(buffer, points[i], points[0], col);
       }
       
       Interval project(Mat9f const& mat, Vec2f const& v) const
       {
          Interval interval(FLT_MAX, -FLT_MAX);
                
          for(int i = 0; i < points.size(); i++)
          {
             float k = (points[i] * mat).dot(v);
             
             if(k < interval.min)
                interval.min = k;
             
             if(k > interval.max)
                interval.max = k;
          }
          
          return interval;
       }
};

bool SAT(Shape const& shape1, Mat9f const& mat1,
         Shape const& shape2, Mat9f const& mat2,
         std::vector<Vec2f> const& norms, Vec2f &mtd, Vec2f& mtdNorm)
{
    float depth, minDepth = FLT_MAX;
    int index = -1;
    
    for(int i = 0; i < norms.size(); i++)
    {
      if(Interval::intersect(shape1.project(mat1, norms[i]), shape2.project(mat2, norms[i]), depth))
      {
         if(fabs(depth) < fabs(minDepth))
         {
            minDepth = depth;
            index = i;
         }
      }
      
      else
        return false;
    }
    
    mtdNorm = norms[index];
    mtd = norms[index] * minDepth;
    return true;
}

bool polyVsPoly(Poly const& poly1, Mat9f const& mat1,
                Poly const& poly2, Mat9f const& mat2,
                Vec2f &mtd, Vec2f& mtdNorm)
{
    std::vector<Vec2f>& norms = tempVec;
    std::vector<Vec2f> const& p1 = poly1.points;
    std::vector<Vec2f> const& p2 = poly2.points;
    int i;
    
    //norms.reserve(p1.size() + p2.size());
    tempVec.clear();
    
    for(i = 0; i < p1.size() - 1; i++)
      norms.push_back((p1[i] - p1[i + 1]).unitNormal());
    norms.push_back((p1[i] - p1[0]).unitNormal());
      
    for(i = 0; i < p2.size() - 1; i++)
      norms.push_back((p2[i] - p2[i + 1]).unitNormal());
    norms.push_back((p2[i] - p2[0]).unitNormal());
    
    return SAT(poly1, mat1, poly2, mat2, norms, mtd, mtdNorm);
}

int main()
{   
    bool exit = false;
    
    init();
    
    Poly poly1, poly2, poly3;
    
    poly1.init(400, 400, 100, 10);
    poly2.init(500, 500, 50, 10);
    
    Mat9f mat1, mat2;
    
    int start = time(NULL);
    Vec2f mtd, norm;
    for(int i = 0; i < 2; i++)
    {
       polyVsPoly(poly1, mat1, poly2, mat2, mtd, norm);
    }
    
    printf("%d", time(NULL) - start);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     poly1.init(400, 400, 100, 10);
     poly2.init(mouse_x, mouse_y, 50, 10);
     
     clear_to_color(buffer, 0);
     
     poly1.draw(buffer, makecol(255, 255, 255));
     poly2.draw(buffer, makecol(255, 255, 255));
     
     Vec2f mtd, norm;
     
     if(polyVsPoly(poly1, mat1, poly2, mat2, mtd, norm))
     {
        poly3.points = poly2.points;
        for(int i = 0; i < poly3.points.size(); i++)
          poly3.points[i] -= mtd;
            
        poly3.draw(buffer, makecol(255, 0, 0));
     }
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
