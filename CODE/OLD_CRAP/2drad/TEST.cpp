#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include "Vec2f.hpp"

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
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

Mat9f projectionMatrix(float b, float t, float n, float f)
{
 
 /*
    Mat9f proj;
    proj(0, 0) = 1; proj(1, 0) = 0;     proj(2, 0) = 0;
    proj(0, 1) = 0; proj(1, 1) = near;  proj(2, 1) = 0; 
    proj(0, 2) = 1; proj(1, 2) = 0;     proj(2, 2) = 0;
    

    Mat9f ndc;
    ndc(0, 0) = 1 / (far - near); ndc(1, 0) = 0;                  ndc(2, 0) = -near / (far - near);
    ndc(0, 1) = 0;                ndc(1, 1) = 1 / (top - bottom); ndc(2, 1) = -bottom / (top - bottom); 
    ndc(0, 2) = 0;                ndc(1, 2) = 0;                  ndc(2, 2) = 1;
    
    return proj * ndc;*/
    
    Mat9f proj;
    proj(0, 0) = 1 / (f - n); proj(1, 0) = 0;     proj(2, 0) = -n / (f - n) + 0.5;
    proj(0, 1) = -b / (t - b); proj(1, 1) = n / (t - b);  proj(2, 1) = 0; 
    proj(0, 2) = 1; proj(1, 2) = 0;     proj(2, 2) = 0;
    
    return proj;
}

void lineVec2f(BITMAP* buffer, Vec2f a, Vec2f b, int col)
{
     line(buffer, int(a.x + SCREEN_W / 2), int(a.y + SCREEN_H / 2),
                  int(b.x + SCREEN_W / 2), int(b.y + SCREEN_H / 2), col);
}



struct Line {
       Vec2f a, b;
       
       Line()
       {
       }
       
       Line(Vec2f const& a, Vec2f const& b)
       {
        this->a = a;
        this->b = b;
       }
       
       Line(float x1, float y1, float x2, float y2)
       {
        a.init(x1, y1);
        b.init(x2, y2);
       }
       
       Vec2f point(float k) const
       {
        return a + (b - a) * k;
       }
       
       bool clip(Line const& other)
       {
        Vec2f n = (other.b - other.a).normal();
        float d1 = (this->a - other.a).dot(n);
        float d2 = (this->b - other.a).dot(n);
        
        if((d1 < 0 && d2 > 0) || (d2 < 0 && d1 > 0))
        {
         float k = fabs(d1) / (fabs(d1) + fabs(d2));
         
         if(d1 < 0) this->a = point(k);
         else       this->b = point(k);
         
         return true;
        }
        
        else if(d1 <= 0 && d2 <= 0) return false;
        return true;
       }
};

int main()
{
    bool exit = false;
    
    init();
    
    float near = 50, far = 200, top = -50, bottom = 50;
    
    Mat9f proj;
    proj = projectionMatrix(bottom, top, near, far);
    
    //Vec2f v1(312, -57), v2(169, 25);
    Line ln(312, -57, 169, 25);
    
    Vec2f f0(0, 0);
    Vec2f f1(near, top);
    Vec2f f4(near, bottom);
    Vec2f f2 = (f1 - f0) * (far / near);
    Vec2f f3 = (f4 - f0) * (far / near);
    
    Vec2f temp;
    
    temp = f3 * proj;
    printf("%f %f\n", temp.x, temp.y);

    
    
    Line clippers[4] = {Line(f1, f2), Line(f2, f3), Line(f3, f4), Line(f4, f1)};
    
    while(!exit)
    {
     
     if(key[KEY_ESC]) exit = true;
      
     if(mouse_b == 1)
      ln.a.init(mouse_x - SCREEN_W / 2, mouse_y - SCREEN_H / 2);
     if(mouse_b == 2)
      ln.b.init(mouse_x - SCREEN_W / 2, mouse_y - SCREEN_H / 2);
      
      Line clipped = ln;
      bool vis;
      
      for(int i = 0; i < 4 && (vis = clipped.clip(clippers[i])); i++);
      
     clear_to_color(buffer, 0);
     
     lineVec2f(buffer, f1, f2, makecol(0, 255, 0));
     lineVec2f(buffer, f2, f3, makecol(0, 255, 0));
     lineVec2f(buffer, f3, f4, makecol(0, 255, 0));
     lineVec2f(buffer, f4, f1, makecol(0, 255, 0));
     
     lineVec2f(buffer, f0, f1, makecol(0, 255, 0));
     lineVec2f(buffer, f0, f4, makecol(0, 255, 0));
     
     lineVec2f(buffer, ln.a, ln.b, makecol(0, 255, 255));
     
     if(vis)
     {
     Vec2f vr1 = clipped.a * proj;
     Vec2f vr2 = clipped.b * proj;
     
     vr1.x = near;
     vr1.y = vr1.y * (top - bottom) + bottom;
     
     vr2.x = near;
     vr2.y = vr2.y * (top - bottom) + bottom;
      
     lineVec2f(buffer, f0, ln.a, makecol(255, 255, 0));
     lineVec2f(buffer, f0, ln.b, makecol(255, 255, 0));
     lineVec2f(buffer, clipped.a, clipped.b, makecol(0, 0, 255));
                  
     lineVec2f(buffer, vr1, vr2, makecol(255, 0, 0));
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
