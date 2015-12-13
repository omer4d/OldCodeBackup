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

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

void wrap(BITMAP* dest, BITMAP* src, int wx, int wy)
{
     for(int j = 0; j < dest->h; j++)
       for(int i = 0; i < dest->w; i++)
          PIXEL(dest, i, j) = PIXEL(src, wrapi(i + wx, 0, src->w), wrapi(j + wy, 0, src->h));
}

int main()
{   
    bool exit = false;
    
    init();
    
    Perlin per1(0.6f, 1);
    per1.setScaleX(4, 32);
    per1.setScaleY(4, 32);
    
    BITMAP* bmp = create_bitmap(128, 128);
    BITMAP* wbmp = create_bitmap(bmp->w, bmp->h);
    
    for(int y = 0; y < bmp->h; y++)
       for(int x = 0; x < bmp->w; x++)
       {
           int k = clamp255((per1.getValue2i(x, y) + 1.0) * 0.5f * 255.f);
           PIXEL(bmp, x, y) = makecol32(k, k, k);
       }
    
    save_bitmap("texture.bmp", bmp, NULL);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0);
     
     wrap(wbmp, bmp, SCREEN_W / 2 - mouse_x, SCREEN_H / 2 - mouse_y);
     
     for(int y = 0; y < 10; y++)
       for(int x = 0; x < 10; x++)
         draw_sprite(buffer, wbmp, x * wbmp->w, y * wbmp->h);
     
     /*
     Vec2f oldPoint(0.f, SCREEN_H / 2 + per1.getTiledValue1i(0) * 100.f);
     
     for(int i = 1; i < 400; i++)
     {
         Vec2f newPoint(i, SCREEN_H / 2 + per1.getTiledValue1i(i % 200) * 100.f);
         DrawVec2f::line(buffer, oldPoint, newPoint, makecol(255, 255, 255));
         oldPoint = newPoint;
     }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    
    deinit();
    return 0;
}END_OF_MAIN()
