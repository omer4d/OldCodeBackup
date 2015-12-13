#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include "Vec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W / 2, SCREEN_H / 2);
     
     srand(time(NULL));
     
     set_add_blender(0, 0, 0, 255);
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
}

void deinit()
{
     destroy_bitmap(buffer);
}

float randf() // 0 to 1
{
      return float(rand()) / float(RAND_MAX);
}

int randomDither(float v)
{
    int vi = int(v);
    float error = v - vi;
    return randf() > error ? vi : (vi + 1);
}

int colSub(int c, int v)
{
    int r = getr32(c) - v;
    int g = getg32(c) - v;
    int b = getb32(c) - v;
    
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    
    return makecol32(r, g, b);
}

void fade(BITMAP* buffer, float k)
{
    for(int j = 0; j < buffer->h; ++j)
      for(int i = 0; i < buffer->w; ++i)
      {
         int c = PIXEL(buffer, i, j);
         PIXEL(buffer, i, j) = makecol32(int(getr32(c) * k), int(getg32(c) * k), int(getb32(c) * k));
         //PIXEL(buffer, i, j) = colSub(c, 10);
      }
}

void dithered(BITMAP* dest, BITMAP* src, int col)
{
    for(int j = 0; j < buffer->h; ++j)
      for(int i = 0; i < buffer->w; ++i)
      {
         int c = PIXEL(src, i, j);
         float v = (getr32(c) * 0.3 + getg32(c) * 0.6 + getb32(c) * 0.1) / 255.f;
         
         PIXEL(buffer, i, j) = randomDither(v) ? col : 0;
      }
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* accum = create_bitmap(buffer->w, buffer->h);
    Vec2f pos(buffer->w / 2, buffer->h / 2), vel(300, 300);
    float dt = 0.0075;
    
    clear_bitmap(accum);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     vel.y += 1000 * dt;
     pos += vel * dt;
     
     if(pos.x < 0)
     {
       pos.x = 0;
       vel.x *= -1;
     }
     
     if(pos.x > buffer->w - 1)
     {
        pos.x = buffer->w - 1;
        vel.x *= -1;
     }
     
     if(pos.y > buffer->h - 1)
     {
        pos.y = buffer->h - 1;
        vel.y *= -1;
     }
    
      fade(accum, 0.95);
      
      circlefill(accum, int(pos.x), int(pos.y), 15, makecol(255, 255, 255));
      
      dithered(buffer, accum, makecol(128, 0, 0));
      stretch_blit(buffer, screen, 0, 0, buffer->w, buffer->h, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(accum);
    deinit();
    return 0;
}END_OF_MAIN()
