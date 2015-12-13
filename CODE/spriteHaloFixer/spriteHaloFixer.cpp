
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>
#include <functional>

#include <loadpng.h>

#include "DirCrawler.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     register_png_file_type();
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void deinit()
{
     destroy_bitmap(buffer);
}

void maskify(BITMAP* dest, BITMAP* src, int thresh, int mask)
{
   for(int j = 0; j < src->h; ++j)
       for(int i = 0; i < src->w; ++i)
       {
          int p = PIXEL(src, i, j);
          if(geta(p) < thresh)
             PIXEL(dest, i, j) = mask;
          else
             PIXEL(dest, i, j) = p;
       }
}

void count(int col, int mask, int& rsum, int& gsum, int& bsum, int& counter)
{
     if(col != mask)
     {
         rsum += getr32(col);
         gsum += getg32(col);
         bsum += getb32(col);
         ++counter;
     }
}

void outline(BITMAP* dest, BITMAP* src, int mask)
{
   for(int j = 1; j < src->h - 1; ++j)
      for(int i = 1; i < src->w - 1; ++i)
      {
         int c = PIXEL(src, i, j);
         int l = PIXEL(src, i - 1, j);
         int r = PIXEL(src, i + 1, j);
         int t = PIXEL(src, i, j - 1);
         int b = PIXEL(src, i, j + 1);
       
         int rsum = 0, gsum = 0, bsum = 0, counter = 0;
         
         count(l, mask, rsum, gsum, bsum, counter);
         count(r, mask, rsum, gsum, bsum, counter);
         count(t, mask, rsum, gsum, bsum, counter);
         count(b, mask, rsum, gsum, bsum, counter);
       
         if(c == mask && counter != 0)
                PIXEL(dest, i, j) = makecol32(rsum / counter, gsum / counter, bsum / counter);
         else
             PIXEL(dest, i, j) = c;
      }
}

void copyAlpha(BITMAP* dest, BITMAP* src)
{
   for(int j = 0; j < src->h; ++j)
     for(int i = 0; i < src->w; ++i)
     {
       int ps = PIXEL(src, i, j);
       int pd = PIXEL(dest, i, j);
       PIXEL(dest, i, j) = makeacol32(getr32(pd), getg32(pd), getb32(pd), geta32(ps));
     }
}

void process(char const* path)
{   
    printf("Processing: %s\n", path);
    
    BITMAP* orig = load_bitmap(path, NULL);
    BITMAP* src = create_bitmap(orig->w, orig->h);
    BITMAP* dest = create_bitmap(orig->w, orig->h);
    
    int mask = makeacol32(0, 255, 255, 0);
    maskify(src, orig, 50, mask);
    //blit(src, orig, 0, 0, 0, 0, orig->w, orig->h);
    
    clear_to_color(dest, mask);
    
    for(int i = 0; i < 30; ++i)
    {
       outline(dest, src, mask);
       BITMAP* temp = dest;
       dest = src;
       src = temp;
    }
    
    copyAlpha(src, orig);
    save_bitmap(path, src, NULL);
    
    destroy_bitmap(orig);
    destroy_bitmap(src);
    destroy_bitmap(dest);
}

int main()
{
    bool exit = false;
    
    init();
    
    DirCrawler::crawlFiles("input/", [](char const* path) {
       process(path);
    });
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
