#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include <inttypes.h>
#include <float.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"
#include "Perlin.hpp"
#include "Buffer.hpp"

#include "GaussBlur.hpp"
#include "ClipRect.hpp"

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

template <class T>
class Sampler1D {
public:
    virtual T at(float x) = 0;
};

template <class T>
class Irregular1D {
      struct Point {
             float x;
             T val;
             float wratio;
      };
      
      typedef std::list<Point> PointList;
      typedef typename PointList::iterator PointIter;
      
      PointList pointList;
      
public:
    void push(float x, T const& val, float wratio = 0.5f)
    {
        bool found = false;

        for(PointIter i = pointList.begin(); i != pointList.end(); ++i)
            if(i->x > x)
            {
                pointList.insert(i, Point(x, val, wratio));
                found = true;
                break;
            }

        if(!found)
            pointList.push_back(Point(x, val, wratio));
    }
    
    PointIter begin()
    {
        return pointList.begin();
    }
    
    PointIter end()
    {
        return pointList.end();
    }
};

template <class T>
class Rounded: public Sampler1D<T> {
      Irregular1D<T> const& grid;
      
public:
       Rounded(Irregular1D<T> const& g): grid(g)
       {
       }
       
       T at(float x)
       {
         return grid.pointList.begin()->val;
       }
};

int main()
{   
    bool exit = false;
    
    init();
    
    Irregular1D<float> grid;
    Rounded<float> sampler(grid);
    
    printf("%f", sampler.at(0));
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, 0);

     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
