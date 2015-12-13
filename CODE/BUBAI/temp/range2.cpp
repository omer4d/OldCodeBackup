#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>
#include <array>
#include <random>

#include "Buffer.hpp"
#include "GridMatrix.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     //install_int(tick, 1);
     
     srand(time(NULL));
     //srand(100);
}

void deinit()
{
     destroy_bitmap(buffer);
}

void tangents(Vec2f& t1, Vec2f& t2, Vec2f const& c, float r, Vec2f const& p)
{
   Vec2f d = c - p;
   float invlen = 1.f / d.length();
   d *= invlen;
   float sina = r * invlen;
   float cosa = cos(asin(sina));
   
   t1 = d.rotated(cosa, sina);
   t2 = d.rotated(cosa, -sina);
}

Vec2f tangent(Vec2f const& c, float r, Vec2f const& p, float sgn)
{
   Vec2f d = c - p;
   float invlen = 1.f / d.length();
   d *= invlen;
   float sina = r * invlen;
   float cosa = cos(asin(sina));
   
   Vec2f t = d.rotated(cosa, sina * sgn);
}

bool lineVsCircle(Vec2f const& src, Vec2f const& dir, Vec2f const& c, float r)
{
    Vec2f n = dir.normal();
    float proj = (c - src).dot(n);
    
    return proj > -r && proj < r;
}

bool block(Vec2f& t1, Vec2f& t2, Vec2f const& src, Vec2f const& c, float rad)
{
     bool f1 = lineVsCircle(src, t1, c, rad);
     bool f2 = lineVsCircle(src, t2, c, rad);
     
     if(f1 && !f2)
     {
       t1 = tangent(c, rad, src, -1.f);
       return false;
     }
     
     else if(f2 && !f1)
     {
       t2 = tangent(c, rad, src, 1.f);
       return false;
     }
     
     else if(f2 && f1)
     {
       return true;
     }
     
     else
       return false;
}

std::vector<Vec2f> genPoints(int n)
{
    std::vector<Vec2f> points;
    
    for(int i = 0; i < n; ++i)
        points.push_back(Vec2f(Util::randf(100, SCREEN_W), Util::randf(100, SCREEN_H)));
        
    return points;
}

int main()
{
    bool exit = false;
    
    init();
    
    Vec2f src(100, 100), dest;
    float rad = 50;
    
    std::vector<Vec2f> circles = genPoints(5);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(key[KEY_SPACE])
     {
        circles = genPoints(20);
     }
     
     std::vector<Vec2f> occluders;
     Vec2f n = (dest - src).normal().unit();
     
     for(int i = 0; i < circles.size(); ++i)
        if((circles[i] - dest).dot((dest - src).unit()) < 0.f)
          occluders.push_back(circles[i]);
     
     dest.init(mouse_x, mouse_y);
     
     Vec2f t1, t2;
     tangents(t1, t2, dest, rad, src);
     
     bool blocked = false;
     
     for(int i = 0; i < occluders.size(); ++i)
        blocked |= block(t1, t2, src, occluders[i], rad);
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     for(int i = 0; i < circles.size(); ++i)
        DrawVec2f::circle(buffer, circles[i], rad / 2, makecol(0, 0, 255));
        
     for(int i = 0; i < occluders.size(); ++i)
        DrawVec2f::circle(buffer, occluders[i], rad / 2, makecol(255, 0, 0));
     
     DrawVec2f::circle(buffer, dest, rad / 2, makecol(0, 255, 0));
     DrawVec2f::circlefill(buffer, src, rad / 2, makecol(255, 255, 0));
     
     DrawVec2f::line(buffer, dest - n * 1000, dest + n * 1000, makecol(255, 0, 255));
     
     if(!blocked)
     {
       DrawVec2f::line(buffer, src, src + t1 * 1000, makecol(255, 255, 255));
       DrawVec2f::line(buffer, src, src + t2 * 1000, makecol(255, 255, 255));
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
