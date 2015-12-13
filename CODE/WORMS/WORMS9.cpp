#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <vector>
#include <algorithm>

#include "Vec2f.hpp"
#include "Buffer.hpp"

struct Rect {
   int x, y, w, h;
   
   Rect(int px, int py, int pw, int ph): x(px), y(py), w(pw), h(ph)
   {
   }
};

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
     
     set_add_blender(255, 255, 255, 255);
     //drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     
     install_int_ex(&tick, MSEC_TO_TIMER(1000));
}

void deinit()
{
     destroy_bitmap(buffer);
}

void add_bitmaps(BITMAP* dest, BITMAP* src)
{
   int w = MIN(dest->w, src->w);
   int h = MIN(dest->h, src->h);
   for(int y = 0; y < h; ++y)
     for(int x = 0; x < w; ++x)
       PIXEL(dest, x, y) += PIXEL(src, x, y);
}

BITMAP* make_brush()
{
    int rmin = 3, rstep = 2, rmax = rmin + rstep * 31;
    BITMAP* brush = create_bitmap(rmax * 2 + 1, rmax * 2 + 1);
    BITMAP* tmp = create_bitmap(brush->w, brush->h);
    
    clear_to_color(brush, 0);
    clear_to_color(tmp, 0);
    
    int col = 1, r = rmin;
    
    for(int i = 0; i < 32; ++i)
    {
      circlefill(tmp, tmp->w / 2, tmp->h / 2, r, col);
      add_bitmaps(brush, tmp);
      col *= 2;
      r += rstep;
    }
    
    //for(int x = 0; x < brush->w; ++x)
      //for(int y = 0; y < brush->h; ++y)
        // if(PIXEL(brush, x, y) == 0)
          //  PIXEL(brush, x, y) = makecol(255, 0, 255);
         //else
            //PIXEL(brush, x, y) = makecol(255, 0, 0);
    
    destroy_bitmap(tmp);
    return brush;
}

void draw_layer(BITMAP* dest, BITMAP* src, int x0, int y0, int n)
{
    int mask = 1 << n;
    int minx = MAX(0, x0), maxx = MIN(dest->w, x0 + src->w);
    int miny = MAX(0, y0), maxy = MIN(dest->h, y0 + src->h);
    int w = maxx - minx;

    for(int y = miny; y < maxy; ++y)
    {
       long* src_row = &((long*)src->line[y - y0])[minx - x0];
       long* dest_row = &((long*)dest->line[y])[minx];
       long* dest_row_max  = dest_row + w;
       
       while(dest_row < dest_row_max)
       {
          *dest_row = *src_row;// & n ? -1 : 1;
          ++dest_row;
          ++src_row;
       }
    }
}

bool outline_pixel(BITMAP* src, int x, int y)
{
   return  PIXEL(src, x, y) != MASK_COLOR_24 &&
             (PIXEL(src, x + 1, y) == MASK_COLOR_32 ||
              PIXEL(src, x - 1, y) == MASK_COLOR_32 ||
              PIXEL(src, x, y + 1) == MASK_COLOR_32 ||
              PIXEL(src, x, y - 1) == MASK_COLOR_32);
}

void collision_map_blit(BITMAP* dest, BITMAP* src, int x0, int y0, int w, int h)
{
    int minx = MAX(0, x0), maxx = MIN(dest->w, x0 + w);
    int miny = MAX(0, y0), maxy = MIN(dest->h, y0 + h);
    
    for(int y = miny; y < maxy; ++y)
      for(int x = minx; x < maxx; ++x)
         PIXEL(dest, x, y) = PIXEL(src, x, y) != MASK_COLOR_32 ? -1 : 0;
}

void draw_sprite_or(BITMAP* dest, BITMAP* src, int x0, int y0)
{
    int minx = MAX(0, x0), maxx = MIN(dest->w, x0 + src->w);
    int miny = MAX(0, y0), maxy = MIN(dest->h, y0 + src->h);

    for(int y = miny; y < maxy; ++y)
       for(int x = minx; x < maxx; ++x)
       {
          PIXEL(dest, x, y) = PIXEL(dest, x, y) | PIXEL(src, x - x0, y - y0);
       }
}

void init_collision_map(BITMAP* out, BITMAP* in, Rect const& rect, BITMAP* brush)
{
    int minx = MAX(rect.x - brush->w, 1);
    int miny = MAX(rect.y - brush->h, 1);
    int maxx = MIN(rect.x + rect.w + brush->w, in->w - 1);
    int maxy = MIN(rect.y + rect.h + brush->h, in->h - 1);
    int w2 = brush->w / 2;
    int h2 = brush->h / 2;
    
    collision_map_blit(out, in, rect.x - w2, rect.y - h2, rect.w + w2 * 2, rect.h + h2 * 2);
    
    for(int y = miny; y < maxy; ++y)
      for(int x = minx; x < maxx; ++x)
          if(outline_pixel(in, x, y))
              draw_sprite_or(out, brush, x - w2, y - h2);
}

struct Terrain {
   BITMAP* color_map;
   BITMAP* brush;
   BITMAP* collision_map;
   
   Terrain()
   {
      color_map = load_bitmap("terrain.bmp", NULL);
      brush = make_brush();
      collision_map = create_bitmap(color_map->w, color_map->h);
      init_collision_map(collision_map, color_map, Rect(0, 0, color_map->w, color_map->h), brush);
   }
   
   ~Terrain()
   {
      destroy_bitmap(color_map);
      destroy_bitmap(brush);
      destroy_bitmap(collision_map);
   }
};

int main()
{
    bool exit = false;
    
    init();

    Terrain terrain;
    int counter = 0;

    while(!exit)
    {           
        if(key[KEY_ESC]) exit = true;
        
        if(mouse_b == 2)
        {
           int cx = mouse_x, cy = mouse_y, rad = 20;
           circlefill(terrain.color_map, cx, cy, rad, MASK_COLOR_32);
           Rect rect(cx - rad, cy - rad, rad * 2 + 1, rad * 2 + 1);        
           
           init_collision_map(terrain.collision_map, terrain.color_map, rect, terrain.brush);
        }
        
        clear_to_color(buffer, 0);
        draw_sprite(buffer, terrain.collision_map, 0, 0);
        //draw_sprite(buffer, terrain.color_map, 0, 0);
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        
        ++counter;
        ++fps;
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
