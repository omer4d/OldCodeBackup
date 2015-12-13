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

int fps = 0;

void tick()
{
   printf("FPS = %d\n", fps);
   fps = 0;
}

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     install_int_ex(&tick, MSEC_TO_TIMER(1000));
     
     //set_add_blender(255, 255, 255, 255);
     //drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct Run {
   short min, max;
   
   Run()
   {
   }
   
   Run(short pmin, short pmax): min(pmin), max(pmax)
   {
   }
};

struct Point {
   short x, y;
   
   Point(short px, short py): x(px), y(py)
   {
   }
};

struct Brush {
   std::vector<Run> rows;
   
   Brush(BITMAP* bmp)
   {
      for(int y = 0; y < bmp->h; ++y)
      {
         int min = bmp->w, max = -1;
         
         for(int x = 0; x < bmp->w; ++x)
            if(PIXEL(bmp, x, y) != MASK_COLOR_32)
            {
               min = MIN(min, x);
               max = MAX(max, x);
            }
         
         rows.push_back(Run(min, max + 1));
      }
   }
};

struct RLE {
   int w, h;
   std::vector<std::vector<Run> > rows;
   
   RLE(BITMAP* in): w(in->w), h(in->h), rows(in->h)
   {
      for(int y = 0; y < h; ++y)
      {
         bool inRun = false;
         Run run;
        
         for(int x = 0; x < w; ++x)
         {
             int c = PIXEL(in, x, y);
             
             if(c != MASK_COLOR_32)
             {
                 if(!inRun)
                 {
                    run.min = x;
                    run.max = run.min + 1;
                    inRun = true;
                 }
                 
                 else
                    ++run.max;
             }
             
             else if(inRun)
             {
                rows[y].push_back(run);
                inRun = false;
             }
         }
      }
   }
   
   void draw_on(BITMAP* buffer)
   {
      int col = makecol(64, 64, 64);
        
      for(int y = 0; y < h; ++y)
      {
         Run const* run = &rows[y][0];
         Run const* runs_max = run + rows[y].size();
         long* row = (long*)buffer->line[y];
        
         while(run != runs_max)
         {
            hline(buffer, run->min, y, run->max - 1, col);
            //memset(row + run->min, -1, (run->max - run->min) << 2);
            ++run;
         }
      }
   }
   
   void outline(std::vector<Point>& points)
   {
       points.clear();
        
       for(int i = 1; i < rows.size() - 1; ++i)
       {
          std::vector<Run> const& curr_row = rows[i];
          std::vector<Run> const& upper_row = rows[i - 1];
          std::vector<Run> const& lower_row = rows[i + 1];
          
          Run const *curr_run = &curr_row[0], *curr_run_end = curr_run + curr_row.size();
          Run const *upper_run = &upper_row[0], *upper_run_end = upper_run + upper_row.size();
          Run const *lower_run = &lower_row[0], *lower_run_end = lower_run + lower_row.size();
          
          while(curr_run != curr_run_end)
          {
             points.push_back(Point(curr_run->min, i));
             
             while(upper_run != upper_run_end && upper_run->max <= curr_run->min)
                ++upper_run;
             
             if(upper_run != upper_run_end)
             {
                for(int x = curr_run->min + 1; x < MIN(upper_run->min, curr_run->max - 1); ++x)
                   points.push_back(Point(x, i));
                
                if(upper_run->min < curr_run->max)
                {
                   int last_max = upper_run->max;
                   ++upper_run;
                
                   while(upper_run != upper_run_end && upper_run->min < curr_run->max)
                   {
                      for(int x = last_max; x < upper_run->min; ++x)
                        points.push_back(Point(x, i));
                   
                        last_max = upper_run->max;
                        ++upper_run;
                   }
                
                   for(int x = last_max; x < curr_run->max - 1; ++x)
                     points.push_back(Point(x, i));
                   
                   --upper_run;
                }
             }
             
             else
                 for(int x = curr_run->min + 1; x < curr_run->max - 1; ++x)
                    points.push_back(Point(x, i));
             
             while(lower_run != lower_run_end && lower_run->max <= curr_run->min)
                ++lower_run;
             
             if(lower_run != lower_run_end)
             {
                for(int x = curr_run->min + 1; x < MIN(lower_run->min, curr_run->max - 1); ++x)
                   points.push_back(Point(x, i));
                
                if(lower_run->min < curr_run->max)
                {
                   int last_max = lower_run->max;
                   ++lower_run;
                
                   while(lower_run != lower_run_end && lower_run->min < curr_run->max)
                   {
                      for(int x = last_max; x < lower_run->min; ++x)
                        points.push_back(Point(x, i));
                   
                        last_max = lower_run->max;
                        ++lower_run;
                   }
                
                   for(int x = last_max; x < curr_run->max - 1; ++x)
                     points.push_back(Point(x, i));
                   
                   --lower_run;
                }
             }
             
             else
                 for(int x = curr_run->min + 1; x < curr_run->max - 1; ++x)
                    points.push_back(Point(x, i));
             
             if(curr_run->max - curr_run->min > 1)
                points.push_back(Point(curr_run->max - 1, i));
             
             ++curr_run;
          }
       }
   }
   
   int bytes()
   {
       int count = 0;
       
       for(int i = 0; i < rows.size(); ++i)
          count += rows[i].size() * 4;
       
       return count;
   }
   
   
   /*
   void add(Brush const& brush, int x0, int y0)
   {
       int miny = MAX(0, y0), maxy = MIN(rows.size(), y0 + brush.rows.size());
       
       for(int y = miny; y < maxy; ++y)
       {
          Run new_run(brush.rows[y - y0].min + x0, brush.rows[y - y0].max + x0);
          
          std::vector<Run>& row = rows[y];
          
          if(new_run.min < new_run.max)
          {
             std::vector<Run>::iterator iter = row.begin();
             
             while(iter != row.end() && iter->max <= new_run.min)
             {
                ++iter;
             }
             
             std::vector<Run>::iterator first = iter;
             
             while(iter != row.end() && iter->min < new_run.max)
             {
                ++iter;
             }
             
             std::vector<Run>::iterator last = iter;
             
             if(first == last)
             {
                row.insert(last, new_run);
             }
             
             else
             {
                 first->min = MIN(first->min, new_run.min);
                 first->max = MAX((last - 1)->max, new_run.max);
                 row.erase(first + 1, last);
             }
          }
       }
   }*/
   
   void add(Brush const& brush, int x0, int y0)
   {
       int miny = MAX(0, y0), maxy = MIN(rows.size(), y0 + brush.rows.size());
       
       for(int y = miny; y < maxy; ++y)
       {
          Run new_run(brush.rows[y - y0].min + x0, brush.rows[y - y0].max + x0);
          
          if(new_run.min < new_run.max)
          {
             std::vector<Run>& row = rows[y];
             Run* curr = &row[0];
             Run* end = curr + row.size();
             std::vector<Run>::iterator begin = row.begin();
             
             while(curr != end && curr->max <= new_run.min)
             {
                ++curr;
             }
             
             Run* first = curr;
             
             while(curr != end && curr->min < new_run.max)
             {
                ++curr;
             }
             
             if(first == curr)
             {
                row.insert(begin + (curr - &row[0]), new_run);
             }
             
             else
             {
                 first->min = MIN(first->min, new_run.min);
                 first->max = MAX((curr - 1)->max, new_run.max);
                 row.erase(begin + (first - &row[0] + 1), begin + (curr - &row[0]));
             }
          }
       }
   }
};

void outline_pixels(std::vector<Point>& points, BITMAP* bmp)
{
   for(int y = 1; y < bmp->h - 1; ++y)
      for(int x = 1; x < bmp->w - 1; ++x)
         if(PIXEL(bmp, x, y) != MASK_COLOR_32 &&
            (PIXEL(bmp, x + 1, y) == MASK_COLOR_32 ||
             PIXEL(bmp, x - 1, y) == MASK_COLOR_32 ||
             PIXEL(bmp, x, y + 1) == MASK_COLOR_32 ||
             PIXEL(bmp, x, y - 1) == MASK_COLOR_32))
               points.push_back(Point(x, y));
}

int main()
{
    bool exit = false;
    
    init();

    BITMAP* bmp = load_bitmap("terrain.bmp", NULL);
    RLE rle(bmp);

    int brush_rad = 15;
    BITMAP* brush_bmp = create_bitmap(brush_rad * 2 + 1, brush_rad * 2 + 1);
    clear_to_color(brush_bmp, MASK_COLOR_32);
    circlefill(brush_bmp, brush_bmp->w / 2, brush_bmp->h / 2, brush_rad, -1);
    Brush brush(brush_bmp);


    printf("%d\n", rle.bytes());
    int col = makecol(0, 255, 0);

    std::vector<Point> points;
    
    long t1;
    time(&t1);
    
    rle.outline(points);
    
    BITMAP* tmp = create_bitmap(bmp->w, bmp->h);
    
     RLE rle_copy = rle;
    
    for(int i = 0; i < 10000; ++i)
    {
       rle_copy = rle;
       Point const* point = &points[0];
        Point const* point_end = point + points.size();
        
        blit(bmp, tmp, 0, 0, 0, 0, bmp->w, bmp->h);
        while(point != point_end)
        {
           rle_copy.add(brush, point->x - brush_rad, point->y - brush_rad);
           //draw_sprite(tmp, brush_bmp, point->x - brush_rad, point->y - brush_rad);
           ++point;
        }
    }
    long t2;
    time(&t2);
    printf("%d\n", t2 - t1);
    
    destroy_bitmap(brush_bmp);

    while(!exit)
    {           
        if(key[KEY_ESC]) exit = true;
 
        //if(mouse_b)
           //rle.add(brush, mouse_x - brush_rad, mouse_y - brush_rad);
 
        clear_to_color(buffer, makecol(0, 0, 0));
        
        //RLE rle_copy = rle;
        //rle.draw_on(buffer);
        //rle.outline(points);
        
        /*
        Point const* point = &points[0];
        Point const* point_end = point + points.size();
        while(point != point_end)
        {
           rle_copy.add(brush, point->x - brush_rad, point->y - brush_rad);
           ++point;
        }*/
        
        rle_copy.draw_on(buffer);
        
        //for(int i = 0; i < points.size(); ++i)
           //putpixel(buffer, points[i].x, points[i].y, col);
        //count_outline_pixels(bmp);
        
        //draw_sprite(buffer, tmp, 0, 0);
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        ++fps;
    }
    
    destroy_bitmap(bmp);
    
    deinit();
    return 0;
}END_OF_MAIN()
