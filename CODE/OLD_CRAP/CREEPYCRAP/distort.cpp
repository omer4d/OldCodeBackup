#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include "Util.hpp"
#include "Col3f.hpp"
#include "FloatMap.hpp"
#include "GaussBlur.hpp"

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"
#include "Vec3f.hpp"


#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}


int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* background = load_bitmap("background.bmp", NULL);
    BITMAP* normalBmp = load_bitmap("normals.bmp", NULL);
    Buffer<Vec2f> offsetMap(normalBmp->w, normalBmp->h);
    
    for(int j = 0; j < normalBmp->h; j++)
      for(int i = 0; i < normalBmp->w; i++)
      {
          int c = PIXEL(normalBmp, i, j);
          Vec3f v;
          
          v.x = (getr32(c) / 255.f) * 2.f - 1.f;
          v.y = (getg32(c) / 255.f) * 2.f - 1.f;
          v.z = (getb32(c) / 255.f) * 2.f - 1.f;
          
          Vec2f a = Vec2f::slerp(Vec2f(0.f, 1.f), Vec2f(v.x, v.z), 0.1f);
          Vec2f b = Vec2f::slerp(Vec2f(0.f, 1.f), Vec2f(v.y, v.z), 0.1f);          
          
          float h = 50.f;
          float dx = (a.x / a.y) * h;
          float dy = (b.x / b.y) * h;
          offsetMap.at(i, j).init(dx, dy);
      }
    
    destroy_bitmap(normalBmp);
    
    int tx = 0, ty = 0;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     
     clear_to_color(buffer, 0);
     
     tx += 3;
     ty += 3;
     
     for(int j = 0; j < background->h; j++)
      for(int i = 0; i < background->w; i++)
      {         
         float dx = offsetMap.wat(i + tx, j + ty).x;
         float dy = offsetMap.wat(i + tx, j + ty).y;
         
         int sx = int(i + dx);
         int sy = int(j + dy);
         
         if(sx < 0 || sy < 0 || sx >= background->w || sy >= background->h)
          PIXEL(buffer, i, j) = 0;
         else
         PIXEL(buffer, i, j) = PIXEL(background, sx, sy);
      }
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(background);
    deinit();
    return 0;
}END_OF_MAIN()
