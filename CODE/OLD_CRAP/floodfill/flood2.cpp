#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

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

struct Point {
  int x, y;
  
  Point(int x, int y)
  {
     this->x = x;
     this->y = y;
  }
};

class PixelTest {
public:
  virtual bool result(int p) const = 0;
};

class MyTest: public PixelTest {
  int col;

public:
  MyTest(int col)
  {
    this->col = col;
  }
       
  virtual bool result(int p) const
  {
    return p == col;
  }
};

void myflood(BITMAP* bmp, int x0, int y0, int dest, PixelTest const& ppt)
{
  std::list<Point> pointList;
  
  if(!ppt.result(getpixel(bmp, x0, y0)))
    return;
  
  pointList.push_back(Point(x0, y0));
  
  while(!pointList.empty())
  {
    Point p = pointList.back();
    bool currTop, currBottom;
    bool lastTop = false, lastBottom = false;
    
    pointList.pop_back();
   
    for(int x = p.x; x < bmp->w && ppt.result(getpixel(bmp, x, p.y)); x++)
    {
      putpixel(bmp, x, p.y, dest);
    
      if(p.y > 0 && (currTop = ppt.result(getpixel(bmp, x, p.y - 1))) && !lastTop)
        pointList.push_back(Point(x, p.y - 1));
      if(p.y < bmp->h - 1 && (currBottom = ppt.result(getpixel(bmp, x, p.y + 1))) && !lastBottom)
        pointList.push_back(Point(x, p.y + 1));
    
      lastTop = currTop;
      lastBottom = currBottom;
    }
  
    for(int x = p.x - 1; x > -1 && ppt.result(getpixel(bmp, x, p.y)); x--)
    {
      putpixel(bmp, x, p.y, dest);
    
      if(p.y > 0 && (currTop = ppt.result(getpixel(bmp, x, p.y - 1))) && !lastTop)
        pointList.push_back(Point(x, p.y - 1));
      if(p.y < bmp->h - 1 && (currBottom = ppt.result(getpixel(bmp, x, p.y + 1))) && !lastBottom)
        pointList.push_back(Point(x, p.y + 1));
    
      lastTop = currTop;
      lastBottom = currBottom;
    }
  }
}



int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* test = load_bitmap("test.bmp", NULL);
    int lastMb = 0;
    
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
      
     clear_to_color(buffer, 0);
     draw_sprite(buffer, test, 0, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     if(mouse_b == 1 && !lastMb)
       myflood(test, mouse_x, mouse_y, makecol(0, 255, 0), MyTest(getpixel(test, mouse_x, mouse_y)));
     
     lastMb = mouse_b;
     
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(test);
    deinit();
    return 0;
}END_OF_MAIN()
