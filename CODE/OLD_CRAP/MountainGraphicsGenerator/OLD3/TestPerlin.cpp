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

Vec3f pointOnTorus(float a, float b)
{
    float r = 64.f, R = 128.f;
    
    return Vec3f(1000.f, 1000.f, 1000.f) + Vec3f(  (R + r * cos(b)) * cos(a),
                   (R + r * cos(b)) * sin(a),
                    r * sin(b));
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* bmp = create_bitmap(256, 256);
    BITMAP* wbmp = create_bitmap(bmp->w, bmp->h);
    
    Perlin per1(0.6f, 100);
    per1.setScaleX(4, bmp->w);
    per1.setScaleY(4, bmp->h);
    
    for(int y = 0; y < bmp->h; y++)
       for(int x = 0; x < bmp->w; x++)
       {
           float kx = float(x) / float(bmp->w - 1),
                 ky = float(y) / float(bmp->h - 1);
        
           Vec3f v = pointOnTorus(ky * 2.f * M_PI, kx * 2.f * M_PI);
           
           int k = clamp255((per1.getValue3f(v.x / 64.f, v.y / 64.f, v.z / 64.f) + 1.0) * 0.5f * 255.f);
           
           //int k = clamp255((per1.getValue2f(x / 64.f, y / 64.f) + 1.0) * 0.5f * 255.f);
           PIXEL(bmp, x, y) = makecol32(k, k, k);
       }
    
    float w = 0.f;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
    

     clear_to_color(buffer, 0);
     
     w += 0.1f;
     
     wrap(wbmp, bmp, int(w), int(w));
     
     for(int i = 0; i < 5; i++)
     for(int j = 0; j < 5; j++)
     draw_sprite(buffer, wbmp, j * bmp->w, i * bmp->h);
     
     /*
     Vec2f oldPoint(0.f, SCREEN_H / 2 + per1.getValue1i(0) * 100.f);
     
     for(int i = 1; i < SCREEN_W; i++)
     {
         Vec2f newPoint(i, SCREEN_H / 2 + per1.getValue1i(i) * 100.f);
         DrawVec2f::line(buffer, oldPoint, newPoint, makecol(255, 255, 255));
         oldPoint = newPoint;
     }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(bmp);
    destroy_bitmap(wbmp);
    deinit();
    return 0;
}END_OF_MAIN()
