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

int blockAvg(BITMAP* src, int x0, int y0, int w, int h)
{
    int rsum = 0, gsum = 0, bsum = 0;
 
    for(int y = y0; y < y0 + h; ++y)
      for(int x = x0; x < x0 + w; ++x)
      {
          int p = PIXEL(src, x, y);
          rsum += getr32(p);
          gsum += getg32(p);
          bsum += getb32(p);
      }
    
   return makecol32(rsum / (w * h), gsum / (w * h), bsum / (w * h));
}

BITMAP* downsample(BITMAP* src, int dw, int dh)
{
    if(dw > src->w || dh > src->h)
       return nullptr;
    
    BITMAP* dest = create_bitmap(dw, dh);
    
    float wrat = (float)src->w / dw;
    float hrat = (float)src->h / dh;
    int wrati = int(wrat);
    int hrati = int(hrat);
    float wratf = wrat - wrati;
    float hratf = hrat - hrati;
    
    float waccum = 0.f;
    float haccum = 0.f;
    
    int sx = 0, sy = 0;
    
    clear_to_color(dest, 0);

    for(int y = 0; y < dest->h; ++y)
    {
        int bh = int(hrati + haccum);
        haccum = haccum - int(haccum) + hratf;
        waccum = 0;
        sx = 0;
     
      for(int x = 0; x < dest->w; ++x)
      {
           int bw = int(wrati + waccum);
           waccum = waccum - int(waccum) + wratf;
           PIXEL(dest, x, y) = blockAvg(src, sx, sy, bw, bh);
           sx += bw;
      }
      
      sy += bh;
    }
    
    return dest;
}

int main()
{   
    bool exit = false;
   
    init();
    
    BITMAP* bmp = load_bitmap("lena.bmp", NULL);

    BITMAP* dest = downsample(bmp, 491, 491);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, 0);
     draw_sprite(buffer, bmp, 0, 0);
     draw_sprite(buffer, dest, 0, 0);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
