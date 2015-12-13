#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

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

void graph(BITMAP* buffer, int dx, int dy, float scale, float buff[], int n, int col)
{
    float lastX = 0, lastY = buff[0];
    
    for(int i = 1; i < n; ++i)
    {
       line(buffer, int(dx + lastX), int(dy - lastY * scale), int(dx + i), int(dy - buff[i] * scale), col);
       lastX = i;
       lastY = buff[i]; 
    }
}

int nearest(float f)
{
   return f > 0 ? int(f + 0.5f) : int(f - 0.5f);
}

void quantize(float buff[], int n, float step)
{
     for(int i = 0; i < n; ++i)
     {
        buff[i] = nearest(buff[i] / step) * step;
     }
}

struct Params {
   int x;
   float y;
   char const* z;
};

void function(Params const& params)
{
   
}

int main()
{   
    bool exit = false;
   
    function({.x = 0, .y = 0});
    
    init();
    
    float buff[512];
    float qbuff[512];
    float f = 2.61f;
    
    for(int i = 0; i < 512; ++i)
    {
       float t = float(i) / 512;
       buff[i] = sin(3.14 * 2.f * f * t);
       qbuff[i] = buff[i];
    }
    
    quantize(qbuff, 512, 0.5);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, 0);
     
     graph(buffer, 130, SCREEN_H / 2, 100, buff, 512, makecol(255, 0, 0));
     graph(buffer, 130, SCREEN_H / 2, 100, qbuff, 512, makecol(0, 255, 0));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
