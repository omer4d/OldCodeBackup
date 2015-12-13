#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <allegro.h>

#include <vector>
#include <list>

#include "Mat9f.hpp"
#include "Vec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

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

struct Shape {
      Mat9f transform;
      
      virtual ~Shape()
      {
      }
      
      virtual Shape* clone() const = 0;
      virtual void draw(BITMAP* buffer) const = 0;
      virtual bool contains(Vec2f const& p) const = 0;
};

class Poly: public Shape {
      std::vector<Vec2f> vertexVec;
      
public:
      Shape* clone() const
      {
          return new Poly(*this);
      }
       
      void initRect(float x1, float y1, float x2, float y2)
      {
          vertexVec.clear();
          vertexVec.reserve(4);
          vertexVec.push_back(Vec2f(x1, y1));
          vertexVec.push_back(Vec2f(x2, y1));
          vertexVec.push_back(Vec2f(x2, y2));
          vertexVec.push_back(Vec2f(x1, y2));
      }
      
      void initRegular(int n, float rx, float ry, float ang=0.f)
      {
         vertexVec.clear();
         vertexVec.reserve(n);
         
         if(n % 2) ang += M_PI / (n * 2.f);
         else      ang += M_PI / n;
         
         for(int i = 0; i < n; i++)
         {
             float k = float(i) / float(n);
             float x = rx * cos(k * 2.f * M_PI + ang);
             float y = ry * sin(k * 2.f * M_PI + ang);
             vertexVec.push_back(Vec2f(x, y));
         }
      }
      
      void initRegular(int n, float r, float ang=0.f)
      {
          initRegular(n, r, r, ang);
      }
      
      void initStar(int n, float r1, float r2, float ang=0.f)
      {
          vertexVec.clear();
          vertexVec.reserve(n * 2);
          
          if(n % 2) ang += M_PI / (n * 2.f);
          else      ang += M_PI / n;
          
          for(int i = 0; i < n * 2; i++)
         {
             float k = float(i) / float(n * 2);
             float r = i % 2 ? r1 : r2;
             float x = r * cos(k * 2.f * M_PI + ang);
             float y = r * sin(k * 2.f * M_PI + ang);
             vertexVec.push_back(Vec2f(x, y));
         }
      }
      
      bool contains(Vec2f const& p) const
      {
          int cn = 0;
          int n = vertexVec.size();

          for(int i = 0; i < n; i++)
          {
             Vec2f v0 = vertexVec[i] * transform;
             Vec2f v1 = vertexVec[(i + 1) % n] * transform;
             
             if( (v0.y <= p.y && v1.y > p.y) || (v0.y > p.y) && (v1.y <= p.y) )
             {
                 float vt = (p.y - v0.y) / (v1.y - v0.y);
                 
                 if(p.x < v0.x + vt * (v1.x - v0.x))
                    cn++;
              }
          }
          
          return cn % 2;
      }
      
      void draw(BITMAP* buffer) const
      {
         int n = vertexVec.size();
       
         for(int i = 0; i < n; i++)
         {
            Vec2f v1 = vertexVec[i] * transform;
            Vec2f v2 = vertexVec[(i + 1) % n] * transform;
            
            line(buffer, int(v1.x), int(v1.y), int(v2.x), int(v2.y), 0);
         }
      }
};

class Circle: public Shape {
      float rad;
      
public:
       Shape* clone() const
       {
          return new Circle(*this);
       }
       
       Circle(float rad)
       {
           this->rad = rad;
       }
       
       bool contains(Vec2f const& p) const
       {
           return Vec2f::squaredDist(Vec2f() * transform, p) < rad * rad;
       }
       
       void draw(BITMAP* buffer) const
       {
           Vec2f c = Vec2f() * transform;
           
           circle(buffer, int(c.x), int(c.y), int(rad), 0);
       }
};

class Canvas {
      typedef std::list<Shape*> ShapeList;
      typedef ShapeList::iterator ShapeIter;
      typedef ShapeList::const_iterator ConstShapeIter;
      
      typedef ShapeList::const_reverse_iterator ShapeHandle;
      
      ShapeList shapeList;
      
public:
       ~Canvas()
       {
           for(ShapeIter i = shapeList.begin(); i != shapeList.end(); ++i)
              delete *i;
       }
       
       Shape* getShape(Vec2f const& p) const
       {
           for(ShapeHandle i = shapeList.rbegin(); i != shapeList.rend(); ++i)
           {
               if((*i)->contains(p))
                  return *i;
           }
           
           return NULL;
       }
       
       void moveUp(Shape* shape)
       {
           ShapeIter i;
        
           for(i = shapeList.begin(); i != shapeList.end(); ++i)
              if(*i == shape)
                break;
           
           if(i != shapeList.end())
           {
               
           }
       }
       
       void add(Shape& shape)
       {
           shapeList.push_back(shape.clone());
       }
       
       void draw(BITMAP* buffer) const
       {
           for(ConstShapeIter i = shapeList.begin(); i != shapeList.end(); ++i)
                (*i)->draw(buffer);
       }
};

class Editor {
      
};

int main()
{   
    bool exit = false;
    
    init();
    
    Poly poly;
    poly.initStar(5, 100.f, 40.f);
    poly.transform.translate(SCREEN_W / 2, SCREEN_H / 2);

    Poly poly2;
    poly2.initRegular(5, 100.f);
    poly2.transform.translate(SCREEN_W / 2, SCREEN_H / 2);

    Circle circle(100.f);
    circle.transform.translate(SCREEN_W / 2, SCREEN_H / 2);
    
    Canvas canvas;
    canvas.add(poly);
    canvas.add(poly2);
    canvas.add(circle);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
    
     clear_to_color(buffer, makecol(128, 128, 128));
     
     canvas.draw(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
