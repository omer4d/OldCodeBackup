#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include <list>

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

struct Point {
   int x, y;
   
   Point(int px, int py): x(px), y(py)
   {
   }
};

void fillRow(std::list<Point>& queue, BITMAP* bmp, int x0, int y0, int src, int dest)
{
   int x, firstX, lastX;
   bool lastTopFillable = false, lastBottomFillable = false;

   for(x = x0 - 1; x >= 0 && PIXEL(bmp, x, y0) == src; --x)
      PIXEL(bmp, x, y0) = dest;
   
   firstX = x + 1;
 
   for(x = x0; x < bmp->w && PIXEL(bmp, x, y0) == src; ++x)
      PIXEL(bmp, x, y0) = dest;
   
   lastX = x - 1;
   
   for(int x = firstX + 1; x <= lastX; ++x)
   {
        if(y0 > 0)
        {
           if(PIXEL(bmp, x, y0 - 1) == src)
           {
               if(!lastTopFillable)
                  queue.push_back(Point(x, y0 - 1));
               lastTopFillable = true;
           }
           
           else
               lastTopFillable = false;
        }
        
        
        if(y0 < bmp->h - 1)
        {
           if(PIXEL(bmp, x, y0 + 1) == src)
           {
               if(!lastBottomFillable)
                  queue.push_back(Point(x, y0 + 1));
               lastBottomFillable = true;
           }
           
           else
               lastBottomFillable = false;
        }
   }
}

void floodFill(BITMAP* bmp, int x0, int y0, int dest)
{
   std::list<Point> queue;
   int src = PIXEL(bmp, x0, y0);
   
   if(src == dest)
     return;
   
   queue.push_back(Point(x0, y0));
   
   int rowCounter = 0;
   
   while(!queue.empty())
   {
      ++rowCounter;
      Point p = queue.front();
      queue.pop_front();
      
      fillRow(queue, bmp, p.x, p.y, src, dest);
   }
   
   printf("%d\n", rowCounter);
}

int main()
{   
    bool exit = false;
   
    init();
    
    BITMAP* bmp = load_bitmap("test.bmp", NULL);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b)
       floodFill(bmp, mouse_x, mouse_y, makecol(255, 0, 0));
        //fillRow(bmp, mouse_x, mouse_y, PIXEL(bmp, mouse_x, mouse_y), makecol(255, 0, 0));
     
     clear_to_color(buffer, 0);
     draw_sprite(buffer, bmp, 0, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
