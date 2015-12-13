#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include <vector>

#include "DrawVec2f.hpp"

#include "Vec3f.hpp"
#include "Vec2f.hpp"
#include "Util.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]
#define MAX_ARMS 8

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 800, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

int normalizeColor(int c)
{
   Vec3f v(getr(c), getg(c), getb(c));
   v.normalize();
   v *= 254;
   
   return makecol(int(v.x), int(v.y), int(v.z));
}

BITMAP* createBrush(int size)
{
   BITMAP* brush = create_bitmap(size, size);
   
   for(int j = 0; j < brush->h; j++)
     for(int i = 0; i < brush->w; i++)
     {
       float rad = sqrt(SQUARED(i - size / 2) + SQUARED(j - size / 2));
       float k = 1.f - rad / (size * 0.5);
       
       if(k > 1) k = 1;
       if(k < 0) k = 0;
       
       k = k * k * k;
       
       PIXEL(brush, i, j) = int(k * 30);
     }
       
   return brush;
}

void drawBrush(BITMAP* buffer, BITMAP* brush, int x, int y, int c)
{
   for(int j = 0; j < brush->h; j++)
     for(int i = 0; i < brush->w; i++)
     {
       int sx = i + x - brush->w / 2;
       int sy = j + y - brush->h / 2;
       
       if(sx > -1 && sy > -1 && sx < buffer->w && sy < buffer->h) 
       {
           int sp = PIXEL(buffer, sx, sy);
           float k = PIXEL(brush, i, j) / 255.0f;
       
       
           int dr = getr32(sp) + int(getr32(c) * k);
           int dg = getg32(sp) + int(getg32(c) * k);
           int db = getb32(sp) + int(getb32(c) * k);

           dr = dr > 255 ? 255 : dr;
           dg = dg > 255 ? 255 : dg;
           db = db > 255 ? 255 : db;
           
           PIXEL(buffer, sx, sy) = makecol32(dr, dg, db);
       }
     }
}

void drawBrushStroke(BITMAP* buffer, BITMAP* brush, Vec2f const& v1, Vec2f const& v2, float space, int c)
{
 /*
     float len = (v2 - v1).length();
     Vec2f dir = (v2 - v1).unit();
     
     for(float k = 0.0; k < len - space; k += space)
     {
         Vec2f p = v1 + dir * k;
         drawBrush(buffer, brush, int(p.x), int(p.y), c);
     }
     
     drawBrush(buffer, brush, int(v2.x), int(v2.y), c);*/
     
     line(buffer, v1.x, v1.y, v2.x, v2.y, makecol(255, 255, 255));
}

void genArms(Vec2f arms[MAX_ARMS], int n)
{
   if(n < 2) n = 2;
   if(n > MAX_ARMS) n = MAX_ARMS;
   
   for(int i = 0; i < n; i++)
    {
       float ang = float(i) / float(n) * 2.0 * M_PI;
       arms[i].x = cos(ang);
       arms[i].y = sin(ang);
    }
}

int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* image = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP* brush = createBrush(16);
    int sym = 6;
    
    clear_bitmap(image);
    
    Vec2f center(SCREEN_W / 2, SCREEN_H / 2);
    Vec2f arms[MAX_ARMS];
    
    genArms(arms, sym);
    
    int color = makecol(255, 255, 255);
    int lastMb = 0;
    
    bool lastPlus = 0;
    bool lastMinus = 0;
    int timeout = 200;
    
    Vec2f src;
    
    while(!exit)
    {
     //rest(1);
      
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_EQUALS])
     {
        if(!lastPlus)
        {
           timeout = 200;
           sym++;
           if(sym > MAX_ARMS) sym = MAX_ARMS;
           genArms(arms, sym);
        }
        
        lastPlus = true;
     } else lastPlus = false;
     
     if(key[KEY_MINUS])
     {
        if(!lastMinus)
        {
           timeout = 200;
           sym--;
           if(sym < 2) sym = 2;
           genArms(arms, sym);
        }
        
        lastMinus = true;
     } else lastMinus = false;
     
     timeout--;
     
     float lp = mouse_x - center.x, ln = mouse_y - center.y;
     //printf("%f %f\n", lp, ln);
     
     clear_to_color(buffer, makecol(64, 64, 64));
     draw_sprite(buffer, image, 0, 0);
     
     if(timeout > 0)
     for(int i = 0; i < sym; i++)
     {
        DrawVec2f::line(buffer, center, center + arms[i] * 200, makecol(100, 100, 100));
     }
     
     if(mouse_b == 2 && lastMb != 2)
     {
        color = normalizeColor(makecol(rand() % 255, rand() % 255, rand() % 255));
     }
     
     if(mouse_b == 1 && !lastMb != 1)
     {
        
     }
     
     if(mouse_b == 1 && lastMb == 1)
        for(int i = 0; i < sym; i++)
        {
                
                Vec2f oldP1 = 
                
                
                Vec2f p1 = center + arms[i] * lp + arms[i].normal() * ln;
                Vec2f p2 = center + arms[i] * lp - arms[i].normal() * ln;
                
                //line(buffer, mouse_x, mouse_y, src.x, src.y, makecol(255, 255, 255));
                
                drawBrushStroke(image, brush, src, p1, 5, color);
                drawBrushStroke(image, brush, src, p2, 5, color);
                
                //drawBrush(image, brush, int(p1.x), int(p1.y), color);
                //drawBrush(image, brush, int(p2.x), int(p2.y), color);
        }
     
     lastMb = mouse_b;
     src.init(mouse_x, mouse_y);
     
     //draw_sprite(buffer, brush, mouse_x, mouse_y);
     circlefill(buffer, mouse_x, mouse_y, 2, color);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(brush);
    destroy_bitmap(image);
    deinit();
    return 0;
}END_OF_MAIN()
